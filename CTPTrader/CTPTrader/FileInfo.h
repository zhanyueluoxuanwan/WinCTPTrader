//一些用户可更改的文件或者配置信息
#pragma once

//基本文件信息
#define UID					"E://uid.txt"					//用户名和密码
#define INSTRUMENT_FILE		"E://TraderInstrumentID.txt"	//合约列表
#define TRADE_PARAM_FILE	"E://TradeParam.txt"			//合约交易参数，与合约列表行数可以匹配

//模拟盘行情和交易信息
#define TRADE_FRONT_IP		"tcp://180.168.146.187:10000"	//交易前置
#define MARKET_FRONT_IP		"tcp://180.168.146.187:10010"	//行情前置
#define BROKER_ID			"9999"							//经纪商id


//最低保证金需求
const double base_percent = 0.3;			//最低保证金需求	