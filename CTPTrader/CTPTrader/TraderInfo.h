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
#define TRADE_PARAM_FILE "E://TradeParam.txt"

//基本数据类型信息
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//合约信息，用于判断交易一手的成本
//基本成本按照price*multiplier*commission+price*multiplier*deposit_percent计算
//此外，扣掉当前交易手续费不能低于最低保证金率
typedef struct {
	double multiplier;			//合约乘数
	double commission;			//手续费率
	double deposit_percent;		//保证金率
}fee;
static const double base_percent = 0.3;		//最低保证金需求				
static map<string, fee> trade_param;		//交易参数

//资金信息
static volatile double money;

//报单结构
typedef struct {
	string id;		//合约信息
	int direction;	//0买，1卖
	int volume;		//数量
	double price;	//价格
	int type;		//0开，1平，-1先平后开
}ORDER;

//报单引用和请求，必须自增，否则过不了CTP检验
extern volatile int order_request;				//报单请求，目前没啥用
extern volatile int order_reference;			//报单引用，计数字段
extern TThostFtdcOrderRefType	ORDER_REF;			//报单引用，标准字段

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
	double upper_limit;	//涨停价
	double lower_limit;	//跌停价
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
//在CTPTrader.cpp生成实例
//生产者消费者模式
extern deque<ORDER> order_queue;

//全局锁，用于同步
extern std::condition_variable empty_signal;			//报单队列是否为空
extern std::mutex mtx;									//全局锁

//基础信息类头文件
#define TRADE_FRONT_IP "tcp://180.168.146.187:10000"	//交易前置
#define MARKET_FRONT_IP "tcp://180.168.146.187:10010"	//行情前置
#define BROKER_ID "9999"

