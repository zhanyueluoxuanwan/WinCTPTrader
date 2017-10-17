#pragma once
#include"stdafx.h"

//�����ļ���Ϣ
#define UID "E://uid.txt"
#define INSTRUMENT_FILE "E://TraderInstrumentID.txt"

//��������������Ϣ

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

//������Ϣ��ͷ�ļ�
#pragma once
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//����ǰ��
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//����ǰ��
#define BROKER_ID "9999"
