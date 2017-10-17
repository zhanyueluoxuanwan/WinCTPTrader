//����ӿ�ʵ��
#include "stdafx.h"
#include "MdSpi.h"
#include "TraderInfo.h"


MdSpi::MdSpi(CThostFtdcMdApi *mdapi) {
	//init api
	fd = new FT_DATA();
	my_mdapi = mdapi;
	my_loginID = 10;
	//reading instruments
	ifstream instrumentFile(INSTRUMENT_FILE);
	if (instrumentFile.is_open()) {
		string len;
		getline(instrumentFile, len);
		instrumentNum = stoi(len);
		instrumentID = (char **)malloc(sizeof(char *) * instrumentNum);
		for (int line = 0; line < instrumentNum; line++) {
			string instrument;
			getline(instrumentFile, instrument);
			instrumentID[line] = (char *)malloc(sizeof(char) * instrument.size());
			strcpy_s(instrumentID[line], instrument.size()+1, instrument.c_str());
			cout << "Current instrument is: " << instrumentID[line] << endl;
		}
	}
	else
		cout << "Cannot read instruments!" << endl;
	instrumentFile.close();
};

MdSpi::~MdSpi() {
	for (int i = 0; i < instrumentNum; i++)
		delete[] instrumentID[i];
	delete[] instrumentID;
	delete fd;
};

void MdSpi::OnFrontConnected() {
	//processing connected operation
	//��ȡ�Զ���ȡ�û��������룬����ϵͳ��������
	ifstream uid(UID);
	if (uid.is_open()) {
		getline(uid, user_id);
		getline(uid, password);
		cout << "��������û��������룬��ʼ�������飡" << endl;
	}
	else {
		cout << "���û����������ļ�!" << endl;
		exit(0);
	}
	uid.close();
	broker = BROKER_ID;
	my_login = new CThostFtdcReqUserLoginField();
	strcpy_s(my_login->BrokerID, broker.c_str());
	strcpy_s(my_login->UserID, user_id.c_str());
	strcpy_s(my_login->Password, password.c_str());
	my_mdapi->ReqUserLogin(my_login, my_loginID);
	std::cout << "Connected" << endl;

};

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cout << "Request login\n";
	cout << "Trading day:" << my_mdapi->GetTradingDay() << endl;
	if (pRspInfo->ErrorID == 0) {
		cout << "Success login-->ID: " << my_loginID << endl;
		cout << "Requiring market data!" << endl;
		int ret = my_mdapi->SubscribeMarketData(instrumentID, instrumentNum);
		cout << ((ret == 0) ? "Successful subscribe!" : "Failed subscribe!") << endl;
	}
};

void MdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cout << "Market data respones" << endl;
	cout << "Instruments:" << pSpecificInstrument->InstrumentID << endl;
	cout << "ResponseInfo:" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
};

//��������Ϣ���뽻�׵�Ԫ��
//�����߼��ܹ����
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *MarketData) {
	
};

void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	
};