#include "stdafx.h"
#include "MyStrategy.h"
#include <iostream>
using namespace std;

//根据实时行情进行交易
void MyStrategy::TradeOnMarketData(map<string, vector<FT_DATA>> &market_data) {

}

//处理报单回报
void MyStrategy::OnRtnOrder(MyOrder *order) {
	cout << "=========报单返回=========" << endl;
	cout << "Order submitted! Order ID: " << order->OrderSysID << endl;
	cout << "FrontID is: " << order->FrontID
		<< " SessionID is: " << order->SessionID
		<< " OrderRef is: " << order->OrderRef
		<< endl;
}

//处理成交回报
void MyStrategy::OnRtnTrade(MyTrade *trade) {
	cout << "=========成交返回=========" << endl;
	cout << "InstrumentID is: " << trade->InstrumentID
		<< " Price is: " << trade->Price
		<< " Volume is: " << trade->Volume
		<< endl;
}