#pragma once
#include"stdafx.h"

//基本文件信息
#define UID "E://uid.txt"
#define INSTRUMENT_FILE "E://TraderInstrumentID.txt"

//基本数据类型信息

//合约信息结构，同数据库表类型结构一致
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
	double vol;			//成交量
	double interest;	//成交额
	double holding;		//持仓
}FT_DATA;

//持仓结构
typedef struct {
	int YdPosition;		//昨日净持仓
	int Position;		//今日净持仓
	int LongPosition;	//多仓
	int ShortPosition;	//空仓
}POS_INFO;

//基础信息类头文件
#pragma once
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//交易前置
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//行情前置
#define BROKER_ID "9999"
