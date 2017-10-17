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

//��������������Ϣ
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//����������Ϣ�ṹ
typedef struct {
	string id;		//��Լ��Ϣ
	int direction;	//1��0��
	int volume;		//����
	double price;	//�۸�
	int type;		//1����0ƽ��-1��ƽ��
}ORDER;

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
//������������ģʽ
static deque<ORDER> order_queue;

//ȫ����������ͬ��
static std::condition_variable empty_signal;		//���������Ƿ�Ϊ��
static std::mutex mtx;								//ȫ����

//������Ϣ��ͷ�ļ�
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//����ǰ��
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//����ǰ��
#define BROKER_ID "9999"
