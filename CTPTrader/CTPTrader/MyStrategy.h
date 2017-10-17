//�����ļ�
#pragma once
#include "stdafx.h"
#include "TraderInfo.h"
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class MyStrategy {
public:
	//���캯��������uid
	MyStrategy(int id) :uid(id) {}
	//�����߼�����,��MdSpi��RtnMarketData�е���
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data);
	//�����ر�
	void OnRtnOrder(MyOrder *order);
	//���׻ر�
	void OnRtnTrade(MyTrade *trade);
	//��ȡ����id
	int GetUid() { return uid; }
private:
	int uid;
};
