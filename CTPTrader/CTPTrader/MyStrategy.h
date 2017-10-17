//策略文件
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
	MyStrategy(int id) :uid(id) {}
	//交易逻辑核心,在MdSpi中RtnMarketData中调用
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data);
	//报单回报
	void OnRtnOrder(MyOrder *order);
	//交易回报
	void OnRtnTrade(MyTrade *trade);
	//获取请求id
	int GetUid() { return uid; }
private:
	int uid;
};
