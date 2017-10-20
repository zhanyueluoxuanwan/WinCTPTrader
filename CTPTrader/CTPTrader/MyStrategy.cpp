#include "stdafx.h"
#include "MyStrategy.h"
#include <iostream>
#include <iomanip> 
using namespace std;

//根据实时行情进行交易
void MyStrategy::TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID) {
	/*
	cout << "Time: " << setw(25) << market_data[InstrumentID][market_data[InstrumentID].size() - 1].time
		<< " InstrumentID: " << InstrumentID
		<< " Last Price: " << market_data[InstrumentID][market_data[InstrumentID].size() - 1].close
		<< endl;
		*/
	if (InstrumentID != "rb1801")
		return;
	count++;
	if (count % 20 != 0)	//每20组行情交一次
		return;
	//测试报撤单
	/*
	if (pos == 0) {
		cout << "Buy rb1801 contract!" << endl;
		ORDER new_order;
		new_order.id = InstrumentID;
		new_order.price = market_data[InstrumentID][market_data[InstrumentID].size() - 1].lower_limit;
		cout << "Open price is: " << new_order.price << endl;
		new_order.direction = BID;				//买
		new_order.type = TYPE_OPEN;				//开仓
		new_order.order_type = ORDER_COMMIT;	//报单
		CommitOrder(new_order);
		new_order.volume = 1;
		pos = 1;
	}
	else if (pos = 1) {		//测试撤单
		cout << "Cancel current order!" << endl;
		ORDER new_order;
		new_order.id = InstrumentID;
		new_order.order_type = ORDER_CANCEL;
		CancelOrder(new_order);
		pos = 0;
	}
	*/
	//测试成交
	if (pos == 0) {
		cout << "Buy rb1801 contract!" << endl;
		ORDER new_order;
		new_order.id = InstrumentID;
		new_order.price = market_data[InstrumentID][market_data[InstrumentID].size() - 1].ask1;
		cout << "Open price is: " << new_order.price << endl;
		new_order.direction = BID;				//买
		new_order.type = TYPE_OPEN;				//开仓
		new_order.order_type = ORDER_COMMIT;	//报单
		new_order.volume = 1;
		CommitOrder(new_order);
		pos = 1;
	}
	else if (pos == 1) {	//测试报单
		cout << "Sell rb1801 contract!" << endl;
		ORDER new_order;
		new_order.id = InstrumentID;
		new_order.price = market_data[InstrumentID][market_data[InstrumentID].size() - 1].bid1;
		new_order.direction = ASK;				//卖
		//new_order.type = TYPE_OPEN;			//开仓
		new_order.type = TYPE_TCLOSE;			//平仓
		new_order.order_type = ORDER_COMMIT;	//报单
		new_order.volume = 1;
		CommitOrder(new_order);
		pos = 0;
	}
}

//处理报单回报
void MyStrategy::OnRtnOrder(MyOrder *order) {
	if (strcmp(order->OrderSysID, "") == 0) 
		cout << "=========报单成功=========" << endl;
	else {
		cout << "=========报单接收=========" << endl;
		cout << "Order submitted! Order ID: " << order->OrderSysID << endl;
		cout << "FrontID is: " << order->FrontID
			<< " SessionID is: " << order->SessionID
			<< " OrderRef is: " << order->OrderRef
			<< " Order direction: " << order->Direction
			<< " Order status: " << order->OrderStatus
			<< " Order status message: " << order->StatusMsg
			<< endl;
	}
}

//处理成交回报
void MyStrategy::OnRtnTrade(MyTrade *trade) {
	cout << "=========成交返回=========" << endl;
	cout << "InstrumentID is: " << trade->InstrumentID
		<< " Price is: " << trade->Price
		<< " Volume is: " << trade->Volume
		<< " Order direction: " << trade->Direction
		<< endl;
	
}

//提交报单
void MyStrategy::CommitOrder(ORDER &new_order) {
	order_reference++;
	sprintf_s(ORDER_REF, "%d", order_reference);
	strcpy_s(new_order.ORDER_REF, ORDER_REF);
	order_queue.push_back(new_order);
	empty_signal.notify_all();
}

//撤单操作
void MyStrategy::CancelOrder(ORDER &new_order) {
	if (local_order_queue.count(order_reference) == 0)
		cout << "Failed Cancel! No such order!" << endl;
	else if(!local_order_queue[order_reference])
		cout << "Failed Cancel! Order has been traded!" << endl;
	else {
		strcpy_s(new_order.ORDER_REF, ORDER_REF);
		new_order.front_id = front_id;
		new_order.session_id = session_id;
		order_queue.push_back(new_order);
		empty_signal.notify_all();
	}
}