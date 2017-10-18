//交易接口文件
//查询的合约持仓一定要和设置的合约一致
#pragma once
#include "stdafx.h"
#include "MyStrategy.h"
#include ".\win64api\ThostFtdcTraderApi.h"
#include <map>

class TdSpi :public CThostFtdcTraderSpi {
public:
	//构造函数
	TdSpi(CThostFtdcTraderApi *my_tdapi);
	~TdSpi();
	//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询结算信息确认响应
	void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//请求查询投资者结算结果响应
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	///用户口令更新请求响应
	void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询行情响应
	///此处应为与MdSpi异步的行情查询，每秒一次，未想好如何设计
	void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者持仓响应
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	///请求查询成交响应
	void OnRspQryTrade(CThostFtdcTradeField *pTrade,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//查询资金帐户响应
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	
	//信息提示
	//依据CPT API标准
	void AlertInfo(int res, string func);

	//基本的交易接口
	//报单录入,TdSpi主动调用，使用线程LittleTrader调用
	int InsertOrder();
	//报单操作（改撤单）,TdSpi主动调用
	int OrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);
	//报单录入应答，CTP回调函数
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单操作应答，CTP回调函数
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单回报，CTP回调函数
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	//成交回报，CTP回调函数
	void OnRtnTrade(CThostFtdcTradeField *pTrade);

	//与策略耦合的接口
	void RegisterStrategy(shared_ptr<MyStrategy> my_strategy) { this->my_strategy = my_strategy; }

private:
	CThostFtdcTraderApi *my_tdapi;							//交易API
	CThostFtdcReqUserLoginField *my_login;					//登录接口
	CThostFtdcReqAuthenticateField *my_auth;				//验证信息
	CThostFtdcQryTradingAccountField *my_account;			//查询资金账户信息
	CThostFtdcQrySettlementInfoConfirmField *my_confirm;;	//查询信息状态量
	CThostFtdcQryInvestorPositionField *my_pos;				//持仓信息字段
	CThostFtdcQrySettlementInfoField *my_settlement;		//结算信息
	CThostFtdcSettlementInfoConfirmField *my_settleconfirm;	//结算确认信息

	std::thread LittleTrader;								//报单线程
	string broker;											//经纪商ID
	string user_id;											//用户名
	string password;										//登录密码
	string my_day;											//交易日期
	map<string, POS_INFO> pos;								//持仓信息
	//double money;											//资金信息采用全局变量

	shared_ptr<MyStrategy> my_strategy;						//交易策略指针
};