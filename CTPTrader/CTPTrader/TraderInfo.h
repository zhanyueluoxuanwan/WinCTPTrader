//交易框架信息，如没有特殊需求请勿更改
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

//基本数据类型信息
typedef CThostFtdcOrderField MyOrder;
typedef CThostFtdcTradeField MyTrade;

//合约信息，用于判断交易一手的成本
//基本成本按照price*multiplier*commission+price*multiplier*deposit_percent计算
//此外，扣掉当前交易手续费不能低于最低保证金率
//目前第一版还未添加
typedef struct {
	double multiplier;			//合约乘数
	double commission;			//手续费率
	double deposit_percent;		//保证金率
}fee;			
extern map<string, fee> trade_param;		//交易参数

//资金信息
extern volatile double money;

//报单结构
typedef struct {
	string id;							//合约信息
	char direction;						//0买，1卖
	int volume;							//数量
	double price;						//价格
	char type;							//0开，1平，-1先平后开
	TThostFtdcOrderRefType	ORDER_REF;	//报单引用，用于定位单号
	int strategy_uid;					//用于定位策略
	int order_type;						//0报单，1撤单
	int front_id;						//交易前置
	int session_id;						//当前会话
}ORDER;
//单子仓位类型
//测试先用TCLOSE平今仓
#define TYPE_OPEN	'0'
#define TYPE_CLOSE	'1'		
#define TYPE_FCLOSE '2'		//强平所有仓位
#define TYPE_TCLOSE '3'		//平今仓，暂时先用此标记段
#define TYPE_YCLOSE '4'		//平昨仓
//交易方向
#define BID '0'				//买
#define ASK '1'				//卖
//单子类型
#define ORDER_COMMIT 0		//报单
#define ORDER_CANCEL 1		//撤单
//用于报单状态查询
#define ACTIVE true			//等待成交状态
#define TRADED false		//已经成交，无法撤单

//报单请求，目前没啥用，先预留
extern volatile int order_request;				

//合约信息结构，同数据库表类型结构一致
//现在只包含了盘口信息，以后会更新五档盘口信息
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
extern map<string, vector<FT_DATA>> market_data;	//用于存储实时的行情信息

//报单队列，采用双向队列
//在CTPTrader.cpp生成实例
//生产者消费者模式
extern deque<ORDER> order_queue;

//全局锁，用于同步
extern std::condition_variable empty_signal;			//报单队列是否为空
extern std::mutex mtx;									//全局锁


