// CTPTrader.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "win64api\ThostFtdcMdApi.h"
#include "win64api\ThostFtdcTraderApi.h"
#include "MyStrategy.h"
#include "MdSpi.h"
#include "TdSpi.h"


//行情信息 
map<string, vector<FT_DATA>> market_data;

//交易参数			
map<string, fee> trade_param;

//资金信息
volatile double money;

//全局报单请求id，必须自增，否则过不了CTP检验
volatile int order_request = 0;					//报单请求，目前没啥用，先预留

//报单队列，采用双向队列
//生产者消费者模式
deque<ORDER> order_queue;						//报单队列

//全局锁，用于同步
std::condition_variable empty_signal;			//报单队列是否为空
std::mutex mtx;									//全局锁

int main()
{
	//生成交易实例
	shared_ptr<MyStrategy> my_strategy = shared_ptr<MyStrategy>(new MyStrategy(1));

	//注册交易接口
	CThostFtdcTraderApi *td_api = CThostFtdcTraderApi::CreateFtdcTraderApi();
	TdSpi *my_tdspi = new TdSpi(td_api);
	my_tdspi->RegisterStrategy(my_strategy);		//注册策略实例接口
	//注册事件处理对象
	td_api->RegisterSpi(my_tdspi);
	//订阅共有流和私有流
	//第一版采用quick模式，防止不必要的幺蛾子
	td_api->SubscribePublicTopic(THOST_TERT_RESTART);
	td_api->SubscribePrivateTopic(THOST_TERT_QUICK);
	//注册前置机
	td_api->RegisterFront(TRADE_FRONT_IP);	//模拟
	std::cout << "Registration Trader Success!" << endl;
	//和前置机连接
	td_api->Init();
	std::cout << "Init Trader Success!" << endl;

	Sleep(5000);

	//注册行情接口
	CThostFtdcMdApi *md_api = CThostFtdcMdApi::CreateFtdcMdApi();
	MdSpi *my_mdspi = new MdSpi(md_api);
	my_mdspi->RegisterStrategy(my_strategy);		//注册策略实例接口
	md_api->RegisterSpi(my_mdspi);
	md_api->RegisterFront(MARKET_FRONT_IP);
	std::cout << "Registration Market Success!" << endl;
	md_api->Init();
	std::cout << "Init API Success!" << endl;

	//停止行情接口
	md_api->Join();
	std::cout << "Market Thread Stop!" << endl;
	md_api->Release();
	std::cout << "Market Thread Release!" << endl;


	//停止交易接口
	td_api->Join();
	std::cout << "Trader Thread Stop!" << endl;
	td_api->Release();
	std::cout << "Trader Thread Release!" << endl;


	//释放接口资源
	delete my_mdspi;
	delete my_tdspi;
	my_strategy.reset();
	
    return 0;
}

