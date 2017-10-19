//行情接口实现
#include "stdafx.h"
#include "MdSpi.h"
#include <iomanip>   
using namespace std;

MdSpi::MdSpi(CThostFtdcMdApi *mdapi) {
	//init api
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
	my_strategy.reset();
};

void MdSpi::OnFrontConnected() {
	//processing connected operation
	//采取自动读取用户名和密码，进行系统内置重连
	//第一个版本，默认每次登陆清空原始行情和报单队列
	order_queue.clear();
	market_data.clear();
	//载入用户名和密码
	ifstream uid(UID);
	if (uid.is_open()) {
		getline(uid, user_id);
		getline(uid, password);
		cout << "完成载入用户名和密码，开始接收行情！" << endl;
	}
	else {
		cout << "无用户名和密码文件!" << endl;
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

//将行情信息传入交易单元中
//依据逻辑架构设计
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *MarketData) {
	/*
	cout << "Time is: " << MarketData->TradingDay
		<< " " << MarketData->UpdateTime
	    << " " << std::setw(3) << MarketData->UpdateMillisec
		<< " Instrument is: " << MarketData->InstrumentID
		<< " Price is: " << MarketData->LastPrice
		<< endl;
		*/
	FT_DATA fd;
	fd.id = MarketData->InstrumentID;
	fd.time = MarketData->TradingDay;
	fd.time += '-';
	fd.time += MarketData->UpdateTime;
	fd.time += '-';
	fd.time += std::to_string(MarketData->UpdateMillisec);
	fd.open = MarketData->OpenPrice;
	fd.high = MarketData->HighestPrice;
	fd.low = MarketData->LowestPrice;
	fd.close = MarketData->LastPrice;
	fd.ask1 = MarketData->AskPrice1;
	fd.bid1 = MarketData->BidPrice1;
	fd.askvol1 = MarketData->AskVolume1;
	fd.bidvol1 = MarketData->BidVolume1;
	fd.vol = MarketData->Volume;
	fd.interest = MarketData->Turnover;
	fd.holding = MarketData->OpenInterest;
	fd.upper_limit = MarketData->UpperLimitPrice;
	fd.lower_limit = MarketData->LowerLimitPrice;
	market_data[fd.id].push_back(fd);	//测试market_data map的初始化形式
	//交易接口
	my_strategy->TradeOnMarketData(market_data, MarketData->InstrumentID);
};

void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	 
};
