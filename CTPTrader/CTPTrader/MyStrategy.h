//�����ļ�
//ʵ��һ���򵥵Ĳ��Խ��в���
//ÿ���������͹�����û�гֲ־���һ�����Ƹ֣��гֲ־���һ�����Ƹ�
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
	MyStrategy(int id) :uid(id), pos(0), count(0) {}
	//�����߼�����,��MdSpi��RtnMarketData�е���
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID);
	//�����ر�
	void OnRtnOrder(MyOrder *order);
	//���׻ر�
	void OnRtnTrade(MyTrade *trade);
	//��ȡ����id
	int GetUid() { return uid; }
private:
	int uid;	//���Ա�ţ����ں���ʹ��
	int pos;	//��Լ�ֲ�
	int count;	//����������ֹ���Թ���������ģ���ʽ�
};
