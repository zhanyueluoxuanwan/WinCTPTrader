//行情接收单元头文件
//CTP Trader中不设置数据库读写服务
#pragma once
#include "MyStrategy.h"
#include ".\win64api\ThostFtdcMdApi.h"
#include <ctime>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>

class MdSpi : public CThostFtdcMdSpi {
public:
	//use mdapi to construct an instance
	MdSpi(CThostFtdcMdApi *mdapi);
	~MdSpi();
	//construct connection
	void OnFrontConnected();
	//Response the login request
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//Response the logout request
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//require market data
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//receive data
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *MarketData);

	//cancel market data
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//disconnected
	void OnFrontDisconnected(int nReason) {};

	//与策略耦合的接口
	void RegisterStrategy(shared_ptr<MyStrategy> my_strategy) { this->my_strategy = my_strategy; };

private:
	CThostFtdcMdApi *my_mdapi;
	CThostFtdcReqUserLoginField *my_login;
	int my_loginID;
	string broker;
	string user_id;
	string password;
	int instrumentNum;
	char **instrumentID;
	shared_ptr<MyStrategy> my_strategy;
};