#include "stdafx.h"
#include "MyStrategy.h"
#include <iostream>
using namespace std;

//����ʵʱ������н���
void MyStrategy::TradeOnMarketData(map<string, vector<FT_DATA>> &market_data) {

}

//�������ر�
void MyStrategy::OnRtnOrder(MyOrder *order) {
	cout << "=========��������=========" << endl;
	cout << "Order submitted! Order ID: " << order->OrderSysID << endl;
	cout << "FrontID is: " << order->FrontID
		<< " SessionID is: " << order->SessionID
		<< " OrderRef is: " << order->OrderRef
		<< endl;
}

//����ɽ��ر�
void MyStrategy::OnRtnTrade(MyTrade *trade) {
	cout << "=========�ɽ�����=========" << endl;
	cout << "InstrumentID is: " << trade->InstrumentID
		<< " Price is: " << trade->Price
		<< " Volume is: " << trade->Volume
		<< endl;
}