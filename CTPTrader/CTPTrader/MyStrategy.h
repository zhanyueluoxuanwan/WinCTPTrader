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
typedef TThostFtdcOrderRefType MyOrderRef;

class MyStrategy {
public:
	//���캯��������uid
	MyStrategy(int id) :uid(id), pos(0), count(0), order_request(0), order_reference(0), front_id(0), session_id(0) {
		local_order_queue.clear();
	}
	//���»���������Ϣ
	void UpdateTradeBasis(int front_id, int session_id) {
		front_id = front_id;
		session_id = session_id;
	}
	//�����߼�����,��MdSpi��RtnMarketData�е���
	void TradeOnMarketData(map<string, vector<FT_DATA>> &market_data, string InstrumentID);
	//�����ر�
	void OnRtnOrder(MyOrder *order);
	//���׻ر�
	void OnRtnTrade(MyTrade *trade);
	//��ȡ����id
	int GetUid() { return uid; }
	//�ύ����
	void CommitOrder(ORDER &new_order);
	//���г���(�ĵ����ܵڶ��������)
	//ע���ĵ�����ֻ�������������Ч��1.����ʱ�Σ�2.����ǰ���뺬�б���
	void CancelOrder(ORDER &new_order);
private:
	int uid;	//���Ա�ţ����ں��ڶ����ע��ʹ��
	int pos;	//��Լ�ֲ�
	int count;	//�������������÷�ֹ���Թ���������ģ���ʽ�

	volatile int order_request;					//��������Ŀǰûɶ�ã���Ԥ��
	volatile int order_reference;				//�������ã������ֶ�
	MyOrderRef	ORDER_REF;						//�������ã���׼�ֶ�
	map<int, bool> local_order_queue;		//����ά���ı�������

	int front_id;			//����ǰ��id
	int session_id;			//��ǰ�ػ�id
};
