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

//基本文件信息
#define UID "E://uid.txt"
#define INSTRUMENT_FILE "E://TraderInstrumentID.txt"

//基本数据类型信息
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//报单基本信息结构
typedef struct {
	string id;		//合约信息
	int direction;	//1买，0卖
	int volume;		//数量
	double price;	//价格
	int type;		//1开，0平，-1先平后开
}ORDER;

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

//行情信息结构，采用全局变量储存
static map<string, vector<FT_DATA>> market_data;		//用于存储实时的行情信息

//报单队列，采用双向队列
//生产者消费者模式
static deque<ORDER> order_queue;

//全局锁，用于同步
static std::condition_variable empty_signal;		//报单队列是否为空
static std::mutex mtx;								//全局锁

//基础信息类头文件
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//交易前置
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//行情前置
#define BROKER_ID "9999"
