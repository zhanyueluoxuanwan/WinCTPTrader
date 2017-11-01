#include "stdafx.h"
#include "TdSpi.h"
#include <fstream>
#include <string>
#include <iostream>
#include <thread>
using namespace std;

//构造函数
TdSpi::TdSpi(CThostFtdcTraderApi *my_tdapi) {
	this->my_tdapi = my_tdapi;
	//初始化一系列查询域
	my_login = new CThostFtdcReqUserLoginField();
	my_account = new CThostFtdcQryTradingAccountField();
	my_confirm = new CThostFtdcQrySettlementInfoConfirmField();
	my_pos = new CThostFtdcQryInvestorPositionField();
	my_settlement = new CThostFtdcQrySettlementInfoField();
	my_settleconfirm = new CThostFtdcSettlementInfoConfirmField();
	my_day = "";	//初始化交易日期，用于后面作比较
	//初始化合约持仓信息
	pos.clear();
	ifstream instrumentFile(INSTRUMENT_FILE);
	POS_INFO init_pos = {0, 0, 0, 0};
	if (instrumentFile.is_open()) {
		string len;
		getline(instrumentFile, len);
		int instrumentNum = stoi(len);
		for (int line = 0; line < instrumentNum; line++) {
			string instrument;
			getline(instrumentFile, instrument);
			pos.insert(make_pair(instrument, init_pos));
			cout << "Current instrument is: " << instrument << endl;
		}
	}
	else
		cout << "Cannot read instruments!" << endl;
	instrumentFile.close();
	//更新合约交易参数
	cout << "更新交易合约参数！" << endl;
	UpdateTradeParam();

	//启动后台报单线程
	cout << "启动后台报单线程！" << endl;
	LittleTrader = thread(&TdSpi::OrderAction, this);
}

//更新合约交易参数
void TdSpi::UpdateTradeParam() {
	ifstream feeFile(TRADE_PARAM_FILE);
	string param;
	while (getline(feeFile, param)) {
		string tmp = "";
		int field_index = 0;
		string InstrumentID;
		fee new_fee;
		for (int i = 0; i < param.size(); i++) {
			if (param[i] == ',') {
				switch (field_index) {
				case 0:
					cout << "InstrumentID is: " << tmp << endl;
					InstrumentID = tmp;
					break;
				case 1:
					cout << "Multiplier is: " << atof(tmp.c_str()) << endl;
					new_fee.multiplier = atof(tmp.c_str());
					break;
				case 2:
					cout << "Commission is: " << atof(tmp.c_str()) << endl;
					new_fee.commission = atof(tmp.c_str());
					break;
				}
				field_index++;
				tmp = "";
			}
			else
				tmp += param[i];
		}
		cout << "Deposit is: " << atof(tmp.c_str()) << endl;
		new_fee.deposit_percent = atof(tmp.c_str());
		trade_param.insert(make_pair(InstrumentID, new_fee));
	}
	feeFile.close();
}

//析构函数，释放指针资源，等待线程退出
TdSpi::~TdSpi() {
	ORDER tmp;
	tmp.id = "EOF";
	order_queue.push_back(tmp);
	empty_signal.notify_all();
	//等待后台线程结束
	LittleTrader.join();
	//删除指针资源
	delete my_login, my_auth, my_account, my_confirm, my_pos, my_settlement, my_settleconfirm;	//结算确认信息
	my_strategy.reset();
}

//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TdSpi::OnFrontConnected() {
	cout << "请求登陆" << endl;
	ifstream uid(UID);
	if (uid.is_open()) {
		getline(uid, user_id);
		getline(uid, password);
		cout << "完成载入用户名和密码，开始进行交易！" << endl;
	}
	else {
		cout << "无用户名和密码文件!" << endl;
		exit(0);
	}
	uid.close();
	broker = BROKER_ID;
	strcpy_s(my_login->BrokerID, broker.c_str());
	strcpy_s(my_login->UserID, user_id.c_str());
	strcpy_s(my_login->Password, password.c_str());
	//拷贝账户信息
	strcpy_s(my_account->BrokerID, broker.c_str());
	strcpy_s(my_account->InvestorID, user_id.c_str());
	AlertInfo(my_tdapi->ReqUserLogin(my_login, 0), "ReqUserLogin");
	//查询账户资金信息
	cout << "登录完成！" << endl;
	
}

///登录请求响应
void TdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	std::chrono::milliseconds sleepDuration(1 * 1000);
	cout << "登录请求回调OnRspUserLogin" << endl;
	cout << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
	cout << "前置编号:" << pRspUserLogin->FrontID << endl
		<< "会话编号" << pRspUserLogin->SessionID << endl
		<< "最大报单引用:" << pRspUserLogin->MaxOrderRef << endl
		<< "上期所时间：" << pRspUserLogin->SHFETime << endl
		<< "大商所时间：" << pRspUserLogin->DCETime << endl
		<< "郑商所时间：" << pRspUserLogin->CZCETime << endl
		<< "中金所时间：" << pRspUserLogin->FFEXTime << endl
		<< "交易日：" << my_tdapi->GetTradingDay() << endl;
	cout << "--------------------------------------------" << endl << endl;

	my_strategy->UpdateTradeBasis(pRspUserLogin->FrontID, pRspUserLogin->SessionID);
	//在开始交易前查询资金账户
	//相应后会回调OnRspQryTradingAccount函数
	 //strcpy_s(my_account->BrokerID, broker.c_str());
	 //strcpy_s(my_account->InvestorID, user_id.c_str());
	 //my_tdapi->ReqQryTradingAccount(my_account, 999);	//此处可能需要考虑查询失败的情况

	 //今天还没确定,第一次发送交易指令前，查询投资者结算结果
	if (my_day=="" || my_day != my_tdapi->GetTradingDay()) {
		//发送结算信息查询请求
		//cout << "结算查询请求！" << endl;
		std::this_thread::sleep_for(sleepDuration);
		//strcpy_s(my_confirm->BrokerID, broker.c_str());
		//strcpy_s(my_confirm->InvestorID, user_id.c_str());
		//AlertInfo(my_tdapi->ReqQrySettlementInfoConfirm(my_confirm, 0), "ReqQrySettlementInfoConfirm");
		//cout << "结算查询请求确认！" << endl;

		//查询结算信息
		cout << "结算信息初始化！" << endl;
		strcpy_s(my_settlement->BrokerID, broker.c_str());
		strcpy_s(my_settlement->InvestorID, user_id.c_str());
		strcpy_s(my_settlement->TradingDay, my_tdapi->GetTradingDay());
		std::this_thread::sleep_for(sleepDuration);
		AlertInfo(my_tdapi->ReqQrySettlementInfo(my_settlement, 1), "ReqQrySettlementInfo");	//第二个输入参数用于标识

		//更新交易日期
		my_day = my_tdapi->GetTradingDay();
		cout << "查询结算信息完成！" << endl;
	}

}

//请求查询结算信息确认响应
//经测试，这个函数是傻逼
void TdSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	
}

//请求查询投资者结算结果响应
void TdSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	//cout << pSettlementInfo->Content << endl;
	if (bIsLast == true) {
		//确认投资者结算结果
		strcpy_s(my_settleconfirm->BrokerID, broker.c_str());
		strcpy_s(my_settleconfirm->InvestorID, user_id.c_str());
		cout << "确认结算信息！" << endl;
		AlertInfo(my_tdapi->ReqSettlementInfoConfirm(my_settleconfirm, 2),"ReqSettlementInfoConfirm");
		cout << "结算信息确认完成！" << endl;
	}
}


//投资者结算结果确认响应
void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cout << endl << "OnRspSettlementInfoConfirm, ID: " << nRequestID << endl;
	cout << "========结算信息详情========" << endl;
	if (pRspInfo != nullptr) {
		cout << pRspInfo->ErrorID << ends << pRspInfo->ErrorMsg << endl;
	}
	cout << "经纪公司代码:" << pSettlementInfoConfirm->BrokerID << endl
		<< "用户账号:" << pSettlementInfoConfirm->InvestorID << endl
		<< "确定日期：" << pSettlementInfoConfirm->ConfirmDate << endl
		<< "确定时间：" << pSettlementInfoConfirm->ConfirmTime << endl;

	//my_tdapi->ReqQryTradingAccount(my_account, 999);

	//查询请求查询成交
	//std::chrono::milliseconds sleepDuration(5 * 1000);
	//std::this_thread::sleep_for(sleepDuration);
	//CThostFtdcQryTradeField *a = new CThostFtdcQryTradeField();
	//strcpy_s(a->BrokerID, broker.c_str());
	//strcpy_s(a->InvestorID, user_id.c_str());
	//strcpy_s(a->InstrumentID, "cu1409");
	//strcpy_s(a->TradeTimeStart, "20140101");
	//strcpy_s(a->TradeTimeEnd, "20140720");
	//my_tdapi->ReqQryTrade(a, 10);

	std::chrono::milliseconds sleepDuration(1 * 1000);
	std::this_thread::sleep_for(sleepDuration);
	cout << "========初始化账户信息========" << endl;
	AlertInfo(my_tdapi->ReqQryTradingAccount(my_account, 999), "ReqQryTradingAccount");
	cout << "========账户信息查询完成========" << endl;

	
	

	////休息两秒再发
	//std::chrono::milliseconds sleepDuration(1*1000);
	//std::this_thread::sleep_for(sleepDuration);
	//cout << "X.X" << endl;
	//int result=my_tdapi->ReqQryInvestorPosition(a, 3);
	//cout << "result:" << result << endl;
}

///请求查询成交响应
void TdSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0) {
		cout << pTrade->BrokerID << endl
			<< pTrade->BrokerOrderSeq << endl;
	}
}

///登出请求响应
void TdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

///用户口令更新请求响应
void TdSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cout << "回调用户口令更新请求响应OnRspUserPasswordUpdate" << endl;
	if (pRspInfo->ErrorID == 0) {
		cout << "更改成功 " << endl
			<< "旧密码为:" << pUserPasswordUpdate->OldPassword << endl
			<< "新密码为:" << pUserPasswordUpdate->NewPassword << endl;
	}
	else {
		cout << pRspInfo->ErrorID << ends << pRspInfo->ErrorMsg << endl;
	}
}

///请求查询行情响应
void TdSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	
}

//查询资金帐户响应
void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr || pRspInfo->ErrorID == 0) {
		cout << "nRequestID: " << nRequestID << endl;
		cout << "可用资金 RMB: " << pTradingAccount->Available << endl;
		money = pTradingAccount->Available;
	}
	else {
		cout << "Failed to query investor's account!" << endl;
	}
	
	//请求查询投资者持仓明细
	cout << "========持仓信息详情========" << endl;
	strcpy_s(my_pos->BrokerID, broker.c_str());
	strcpy_s(my_pos->InvestorID, user_id.c_str());
	std::chrono::milliseconds sleepDuration(1 * 1000); //防止查询过快
	std::this_thread::sleep_for(sleepDuration);
	for (map<string, POS_INFO>::iterator it = pos.begin(); it != pos.end(); it++) {
		strcpy_s(my_pos->InstrumentID, (it->first).c_str());
		AlertInfo(my_tdapi->ReqQryInvestorPosition(my_pos, 1), "ReqQryInvestorPosition");
	}
}
//请求查询投资者持仓响应
//此处可能会遇到空指针，需要继续debug
void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo != nullptr) {
		cout << "错误代码：" << pRspInfo->ErrorID << "错误信息:" << pRspInfo->ErrorMsg;
	}
	else if (pInvestorPosition != nullptr) {
		pos[pInvestorPosition->InstrumentID].YdPosition = pInvestorPosition->YdPosition * (pInvestorPosition->PosiDirection == '2' ? 1 : -1);
		pos[pInvestorPosition->InstrumentID].Position = pInvestorPosition->Position * (pInvestorPosition->PosiDirection == '2' ? 1 : -1);
	}
	if (bIsLast)	//最后一笔回报
		for (map<string, POS_INFO>::iterator it = pos.begin(); it != pos.end(); it++)
			cout << "InstrumentID: " << it->first << " position: " << it->second.Position << endl;
}

//返回信息提示
void TdSpi::AlertInfo(int res, string func) {
	cout << "Function " << func << " status->";
	switch (res) {
	case 0:
		cout << "Success!" << endl;
		break;
	case -1:
		cout << "Network failure! Error code: -1" << endl;
		break;
	case -2:
		cout << "Unprocessed request overflow! Error code: -2" << endl;
		break;
	case -3:
		cout << "Per sec requests exceed limit! Error code: -3" << endl;
		break;
	default:
		cout << "Unknown error, maybe new standard. Error code: " << res << endl;
	}

}

//报单录入,后台自动由LittleTrader调用
//根据报单状态信息判断是报单还是撤单
void TdSpi::OrderAction() {
	while (true) {
		std::unique_lock<std::mutex> lck(mtx);
		while (order_queue.size() == 0) {
			empty_signal.wait(lck);
		}
		if (order_queue[0].id == "EOF")			//报单的合约代码为EOF表示线程结束
			break;
		else if (order_queue.size() == 0) {		//假唤醒，为以后的无锁高效报单队列做准备
			lck.unlock();
			continue;
		}
		cout << "Get new order!" << endl;
		if (order_queue[0].order_type == ORDER_COMMIT) {
			if (trade_param.count(order_queue[0].id) == 0) 
				cout << "Little trader has no information about: " << order_queue[0].id << endl;
			else if (money - 
				order_queue[0].volume * order_queue[0].price * trade_param[order_queue[0].id].multiplier * 
				(trade_param[order_queue[0].id].deposit_percent + trade_param[order_queue[0].id].commission * 2) < money * base_percent) {		//控制开仓风险
				cout << "Little trader has no money to commit so many orders!" << endl;
			}
			else {
				CThostFtdcInputOrderField ord;
				memset(&ord, 0, sizeof(ord));
				///经纪公司代码
				strcpy_s(ord.BrokerID, broker.c_str());
				///投资者代码
				strcpy_s(ord.InvestorID, user_id.c_str());
				///合约代码
				strcpy_s(ord.InstrumentID, order_queue[0].id.c_str());
				///报单引用
				//order_reference++;
				//sprintf_s(ORDER_REF, "%d", order_reference);
				//strcpy_s(ord.OrderRef, ORDER_REF);
				strcpy_s(ord.OrderRef, order_queue[0].ORDER_REF);
				///用户代码
				//	TThostFtdcUserIDType	UserID;
				///报单价格条件: 限价
				ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
				///买卖方向: 
				ord.Direction = order_queue[0].direction;
				///组合开平标志: 开仓
				ord.CombOffsetFlag[0] = order_queue[0].type;
				///组合投机套保标志
				ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
				///价格
				ord.LimitPrice = order_queue[0].price;
				///数量: 1
				ord.VolumeTotalOriginal = order_queue[0].volume;
				///有效期类型: 当日有效
				ord.TimeCondition = THOST_FTDC_TC_GFD;
				///GTD日期
				//	TThostFtdcDateType	GTDDate;
				///成交量类型: 任何数量
				ord.VolumeCondition = THOST_FTDC_VC_AV;
				///最小成交量: 1
				ord.MinVolume = 1;
				///触发条件: 立即
				ord.ContingentCondition = THOST_FTDC_CC_Immediately;
				///止损价
				//	TThostFtdcPriceType	StopPrice;
				///强平原因: 非强平
				ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
				///自动挂起标志: 否
				ord.IsAutoSuspend = 0;
				///业务单元
				//	TThostFtdcBusinessUnitType	BusinessUnit;
				///请求编号
				//	TThostFtdcRequestIDType	RequestID;
				///用户强评标志: 否
				ord.UserForceClose = 0;

				//报单
				AlertInfo(my_tdapi->ReqOrderInsert(&ord, ++order_request), "ReqOrderInsert");
			}
		}
		else if(order_queue[0].order_type == ORDER_CANCEL) {
			CThostFtdcInputOrderActionField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy_s(req.BrokerID, broker.c_str());
			///投资者代码
			strcpy_s(req.InvestorID, user_id.c_str());
			///报单操作引用
			//	TThostFtdcOrderActionRefType	OrderActionRef;
			///报单引用
			cout << "报单引用：" << order_queue[0].ORDER_REF << endl;
			strcpy_s(req.OrderRef, order_queue[0].ORDER_REF);
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///前置编号
			cout << "前置编号：" << order_queue[0].front_id << endl;
			req.FrontID = order_queue[0].front_id;
			///会话编号
			cout << "会话编号：" << order_queue[0].session_id << endl;
			req.SessionID = order_queue[0].session_id;
			///交易所代码
			//	TThostFtdcExchangeIDType	ExchangeID;
			///报单编号
			//	TThostFtdcOrderSysIDType	OrderSysID;
			///操作标志
			req.ActionFlag = THOST_FTDC_AF_Delete;
			///价格
			//	TThostFtdcPriceType	LimitPrice;
			///数量变化
			//	TThostFtdcVolumeType	VolumeChange;
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///合约代码
			cout << "合约代码：" << order_queue[0].id << endl;
			strcpy_s(req.InstrumentID, order_queue[0].id.c_str());

			//撤单
			AlertInfo(my_tdapi->ReqOrderAction(&req, ++order_request), "ReqOrderCancel");
		}
		else {
			cout << "Wrong Order Type! Little Trader Waits!" << endl;
		}
		//弹出单子队列
		order_queue.pop_front();
		lck.unlock();
	}
};

//报单录入错误应答，CTP回调函数
void TdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr)
		cout << "No information returned!" << endl;
	else {
		cout << "Error ID is: " << pRspInfo->ErrorID << endl;
		cout << "Returned information is: " << pRspInfo->ErrorMsg << endl;
	}
}

//撤单操作错误应答，CTP回调函数
void TdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo == nullptr)
		cout << "No information returned!" << endl;
	else {
		cout << "Error ID is: " << pRspInfo->ErrorID << endl;
		cout << "Returned information is: " << pRspInfo->ErrorMsg << endl;
	}
}

//报单回报，CTP回调函数
//只计算了一次性成交的情况，复杂情况需要以后添加
void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	//更新资金信息
	if (strcmp(pOrder->OrderSysID, "") != 0) {
		if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled) {	//撤单成功
			money += pOrder->LimitPrice * pOrder->VolumeTotalOriginal * trade_param[pOrder->InstrumentID].deposit_percent * trade_param[pOrder->InstrumentID].multiplier;
			cout << "Order Canceled current money is: " << money << endl;
			my_strategy->UpdateOnRtnOrder(pOrder, false);		//更新本地报单队列
		}
		else if(my_strategy->ReturnOrderStatus(atoi(pOrder->OrderRef))==0) {	//报单成功
			money -= pOrder->LimitPrice * pOrder->VolumeTotalOriginal * trade_param[pOrder->InstrumentID].deposit_percent * trade_param[pOrder->InstrumentID].multiplier;
			cout << "Order Submitted current money is: " << money << endl;
			my_strategy->UpdateOnRtnOrder(pOrder, true);		//更新本地报单队列
		}
	}
	my_strategy->OnRtnOrder(pOrder);
}
//成交回报，CTP回调函数
void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	my_strategy->UpdateOnRtnTrade(pTrade);	//更新本地报单队列
	my_strategy->OnRtnTrade(pTrade);		//返回成交回报
}