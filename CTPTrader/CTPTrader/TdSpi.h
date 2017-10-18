//���׽ӿ��ļ�
//��ѯ�ĺ�Լ�ֲ�һ��Ҫ�����õĺ�Լһ��
#pragma once
#include "stdafx.h"
#include "MyStrategy.h"
#include ".\win64api\ThostFtdcTraderApi.h"
#include <map>

class TdSpi :public CThostFtdcTraderSpi {
public:
	//���캯��
	TdSpi(CThostFtdcTraderApi *my_tdapi);
	~TdSpi();
	//���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�����ѯ������Ϣȷ����Ӧ
	void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//�����ѯͶ���߽�������Ӧ
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	///�û��������������Ӧ
	void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ������Ӧ
	///�˴�ӦΪ��MdSpi�첽�������ѯ��ÿ��һ�Σ�δ���������
	void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	///�����ѯ�ɽ���Ӧ
	void OnRspQryTrade(CThostFtdcTradeField *pTrade,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

	//��ѯ�ʽ��ʻ���Ӧ
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	
	//��Ϣ��ʾ
	//����CPT API��׼
	void AlertInfo(int res, string func);

	//�����Ľ��׽ӿ�
	//����¼��,TdSpi�������ã�ʹ���߳�LittleTrader����
	int InsertOrder();
	//�����������ĳ�����,TdSpi��������
	int OrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);
	//����¼��Ӧ��CTP�ص�����
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//��������Ӧ��CTP�ص�����
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//�����ر���CTP�ص�����
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	//�ɽ��ر���CTP�ص�����
	void OnRtnTrade(CThostFtdcTradeField *pTrade);

	//�������ϵĽӿ�
	void RegisterStrategy(shared_ptr<MyStrategy> my_strategy) { this->my_strategy = my_strategy; }

private:
	CThostFtdcTraderApi *my_tdapi;							//����API
	CThostFtdcReqUserLoginField *my_login;					//��¼�ӿ�
	CThostFtdcReqAuthenticateField *my_auth;				//��֤��Ϣ
	CThostFtdcQryTradingAccountField *my_account;			//��ѯ�ʽ��˻���Ϣ
	CThostFtdcQrySettlementInfoConfirmField *my_confirm;;	//��ѯ��Ϣ״̬��
	CThostFtdcQryInvestorPositionField *my_pos;				//�ֲ���Ϣ�ֶ�
	CThostFtdcQrySettlementInfoField *my_settlement;		//������Ϣ
	CThostFtdcSettlementInfoConfirmField *my_settleconfirm;	//����ȷ����Ϣ

	std::thread LittleTrader;								//�����߳�
	string broker;											//������ID
	string user_id;											//�û���
	string password;										//��¼����
	string my_day;											//��������
	map<string, POS_INFO> pos;								//�ֲ���Ϣ
	//double money;											//�ʽ���Ϣ����ȫ�ֱ���

	shared_ptr<MyStrategy> my_strategy;						//���ײ���ָ��
};