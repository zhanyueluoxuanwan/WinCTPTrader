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

class MyStrategy {
public:
	//构造函数，加上uid
	MyStrategy(int id) :uid(id), pos(0), count(0) {}
	//交易逻辑核心,在MdSpi中RtnMarketData中调用
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID);
	//报单回报
	void OnRtnOrder(MyOrder *order);
	//交易回报
	void OnRtnTrade(MyTrade *trade);
	//获取请求id
	int GetUid() { return uid; }
private:
	int uid;	//策略编号，用于后期使用
	int pos;	//合约持仓
	int count;	//计数器，防止测试过快消耗完模拟资金
};
