//策略文件
//实现一个简单的策略进行测试
//每次行情推送过来，没有持仓就买一手螺纹钢，有持仓就卖一手螺纹钢
#pragma once
#include "stdafx.h"
#include "TraderInfo.h"
#include <iostream>
#include <map>
#include <vector>
using namespace std;
typedef TThostFtdcOrderRefType MyOrderRef;

class MyStrategy {
public:
	//构造函数，加上uid
	MyStrategy(int id) :uid(id), pos(0), count(0), order_request(0), order_reference(0), front_id(0), session_id(0) {
		local_order_queue.clear();
	}
	//更新基本交易信息
	void UpdateTradeBasis(int front_id, int session_id) {
		front_id = front_id;
		session_id = session_id;
	}
	//交易逻辑核心,在MdSpi中RtnMarketData中调用
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID);
	//报单回报
	void OnRtnOrder(MyOrder *order);
	//交易回报
	void OnRtnTrade(MyTrade *trade);
	//获取策略id
	int GetUid() { return uid; }
	//提交报单
	void CommitOrder(ORDER &new_order);
	//进行撤单(改单功能第二版再添加)
	//注：改单功能只在下列情况下有效：1.交易时段，2.调用前必须含有报单
	void CancelOrder(ORDER &new_order);
private:
	int uid;	//策略编号，用于后期多策略注册使用
	int pos;	//合约持仓
	int count;	//计数器，测试用防止测试过快消耗完模拟资金

	volatile int order_request;					//报单请求，目前没啥用，先预留
	volatile int order_reference;				//报单引用，计数字段
	MyOrderRef	ORDER_REF;						//报单引用，标准字段
	map<int, bool> local_order_queue;		//本地维护的报单队列

	int front_id;			//交易前置id
	int session_id;			//当前回话id
};
