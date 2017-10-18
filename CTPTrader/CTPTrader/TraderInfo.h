#pragma once
#include"stdafx.h"
#include "./win64api/ThostFtdcMdApi.h"
#include "./win64api/ThostFtdcTraderApi.h"
#include <map>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

//�����ļ���Ϣ
#define UID "E://uid.txt"
#define INSTRUMENT_FILE "E://TraderInstrumentID.txt"
#define TRADE_PARAM_FILE "E://TradeParam.txt"

//��������������Ϣ
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//��Լ��Ϣ�������жϽ���һ�ֵĳɱ�
//�����ɱ�����price*multiplier*commission+price*multiplier*deposit_percent����
//���⣬�۵���ǰ���������Ѳ��ܵ�����ͱ�֤����
typedef struct {
	double multiplier;			//��Լ����
	double commission;			//��������
	double deposit_percent;		//��֤����
}fee;
static const double base_percent = 0.3;		//��ͱ�֤������				
static map<string, fee> trade_param;		//���ײ���

//�ʽ���Ϣ
static volatile double money;

//�����ṹ
typedef struct {
	string id;		//��Լ��Ϣ
	int direction;	//0��1��
	int volume;		//����
	double price;	//�۸�
	int type;		//0����1ƽ��-1��ƽ��
}ORDER;

//�������ú����󣬱������������������CTP����
extern volatile int order_request;				//��������Ŀǰûɶ��
extern volatile int order_reference;			//�������ã������ֶ�
extern TThostFtdcOrderRefType	ORDER_REF;			//�������ã���׼�ֶ�

//��Լ��Ϣ�ṹ��ͬ���ݿ�����ͽṹһ��
typedef struct {
	string id;
	string time;
	double open;
	double high;
	double low;
	double close;
	double ask1;
	double bid1;
	double askvol1;
	double bidvol1;
	double vol;			//�ɽ���
	double interest;	//�ɽ���
	double holding;		//�ֲ�
	double upper_limit;	//��ͣ��
	double lower_limit;	//��ͣ��
}FT_DATA;

//�ֲֽṹ
typedef struct {
	int YdPosition;		//���վ��ֲ�
	int Position;		//���վ��ֲ�
	int LongPosition;	//���
	int ShortPosition;	//�ղ�
}POS_INFO;

//������Ϣ�ṹ������ȫ�ֱ�������
static map<string, vector<FT_DATA>> market_data;		//���ڴ洢ʵʱ��������Ϣ

//�������У�����˫�����
//��CTPTrader.cpp����ʵ��
//������������ģʽ
extern deque<ORDER> order_queue;

//ȫ����������ͬ��
extern std::condition_variable empty_signal;			//���������Ƿ�Ϊ��
extern std::mutex mtx;									//ȫ����

//������Ϣ��ͷ�ļ�
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//����ǰ��
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//����ǰ��
#define BROKER_ID "9999"

