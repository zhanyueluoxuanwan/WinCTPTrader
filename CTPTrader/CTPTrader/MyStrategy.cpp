#include "stdafx.h"
#include "MyStrategy.h"
#include <iostream>
using namespace std;

//����ʵʱ������н���
void MyStrategy::TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID) {
	if (InstrumentID != "rb1801")
		return;
	count++;
	if (count % 20 != 0)	//ÿ20�����齻һ��
		return;
	if (pos == 0) {
		cout << "Buy rb1801 contract!" << endl;
		ORDER new_order;
		new_order.id = "rb1801";
		new_order.price = market_data["rb1801"][market_data["rb1801"].size() - 1].upper_limit;
		new_order.direction = 0;	//��
		new_order.type = 0;			//����
		order_queue.push_back(new_order);
		cout << "Order queue size: " << order_queue.size() << endl;
		pos = 1;
	}
	else if (pos == 1) {
		cout << "Sell rb1801 contract!" << endl;
		ORDER new_order;
		new_order.id = "rb1801";
		new_order.price = market_data["rb1801"][market_data["rb1801"].size() - 1].lower_limit;
		new_order.direction = 1;	//��
		new_order.type = 1;			//ƽ��
		order_queue.push_back(new_order);
		cout << "Order queue size: " << order_queue.size() << endl;
		pos = 0;
	}
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