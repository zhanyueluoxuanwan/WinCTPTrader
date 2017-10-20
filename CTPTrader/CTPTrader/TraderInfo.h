//���׿����Ϣ����û�����������������
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

//��������������Ϣ
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//��Լ��Ϣ�������жϽ���һ�ֵĳɱ�
//�����ɱ�����price*multiplier*commission+price*multiplier*deposit_percent����
//���⣬�۵���ǰ���������Ѳ��ܵ�����ͱ�֤����
//Ŀǰ��һ�滹δ���
typedef struct {
	double multiplier;			//��Լ����
	double commission;			//��������
	double deposit_percent;		//��֤����
}fee;			
extern map<string, fee> trade_param;		//���ײ���

//�ʽ���Ϣ
extern volatile double money;

//�����ṹ
typedef struct {
	string id;							//��Լ��Ϣ
	char direction;						//0��1��
	int volume;							//����
	double price;						//�۸�
	char type;							//0����1ƽ��-1��ƽ��
	TThostFtdcOrderRefType	ORDER_REF;	//�������ã����ڶ�λ����
	int strategy_uid;					//���ڶ�λ����
	int order_type;						//0������1����
	int front_id;						//����ǰ��
	int session_id;						//��ǰ�Ự
}ORDER;
//���Ӳ�λ����
//��������TCLOSEƽ���
#define TYPE_OPEN	'0'
#define TYPE_CLOSE	'1'		
#define TYPE_FCLOSE '2'		//ǿƽ���в�λ
#define TYPE_TCLOSE '3'		//ƽ��֣���ʱ���ô˱�Ƕ�
#define TYPE_YCLOSE '4'		//ƽ���
//���׷���
#define BID '0'				//��
#define ASK '1'				//��
//��������
#define ORDER_COMMIT 0		//����
#define ORDER_CANCEL 1		//����
//���ڱ���״̬��ѯ
#define ACTIVE true			//�ȴ��ɽ�״̬
#define TRADED false		//�Ѿ��ɽ����޷�����

//��������Ŀǰûɶ�ã���Ԥ��
extern volatile int order_request;				

//��Լ��Ϣ�ṹ��ͬ���ݿ�����ͽṹһ��
//����ֻ�������̿���Ϣ���Ժ������嵵�̿���Ϣ
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
extern map<string, vector<FT_DATA>> market_data;	//���ڴ洢ʵʱ��������Ϣ

//�������У�����˫�����
//��CTPTrader.cpp����ʵ��
//������������ģʽ
extern deque<ORDER> order_queue;

//ȫ����������ͬ��
extern std::condition_variable empty_signal;			//���������Ƿ�Ϊ��
extern std::mutex mtx;									//ȫ����


