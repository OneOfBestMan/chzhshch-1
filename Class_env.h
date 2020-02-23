#pragma once


#ifndef CLASS_ENV_H
#define CLASS_ENV_H

#include "StockFunc.h"
#include <map>


#include "Class_XianDuan.h"

using namespace std;

/*
 封装一些全局变量  读取K线的高、低点的方法

*/


class Class_env
{
public:

	
	typedef enum {
	/* 输出的线类型， 从笔 到 各个不同级别的线段 */	
		BI=0, XIANDUAN_1 = 1, XIANDUAN_2 = 2, XIANDUAN_3 = 3, XIANDUAN_4 = 4, XIANDUAN_5 = 5, XIANDUAN_6 = 6, XIANDUAN_7 = 7,
	/* 中枢的级别 */	
		ZS_0 = 0, ZS_1=1, ZS_2 = 2, ZS_3 = 3, ZS_4 = 4, ZS_5 = 5, ZS_6 = 6
	} gradeSelect;

	
	typedef enum {
		/* 对于不同的输出线类型，需要两个数组刻画，一个是各个线段从哪里开始到哪里结束，另一个是各个线段起始价格、结束价格 */
		OUTPUT_TIME = 0, OUTPUT_PRICE = 1, 
		/* 中枢的开始和结束标记、中枢持续多久、中枢最高价、中枢最低价 */
		OUTPUT_ZS_START_END_MARK = 0, OUTPUT_ZS_DURATION = 1, OUTPUT_ZS_LOWPRICE = 2, OUTPUT_ZS_HIGHPRICE = 3
	} funcSelect;

	typedef enum { XIANDUAN = 0, ZHONGSHU = 1} ZsXdSelect;

	~Class_env(void);
	static Class_env* getInstance(CALCINFO *p);
	static Class_env* getInstance();
	void outputResult();
	static Class_env* env;

	int   getTotal() const {return totalBar;}

    float *highParm;
    int firstHighParm;

	float *lowParm;
	int firstLowParm;

	float *startParm;
	int firstStartParm;

	float *endParm;
	int firstEndParm;

	int totalBar;
	char* stockName;
	DATA_TYPE barKind; // K线图的时间类型


	gradeSelect grade;
	funcSelect func;
	ZsXdSelect ZSorXD;

	float *resultBuf;


	bool operator==(const Class_env &secondEnv) const 
	{
		return (!strcmp(stockName, secondEnv.stockName) &&
			totalBar  == secondEnv.totalBar  && barKind == secondEnv.barKind);
	}

private:

	Class_env(CALCINFO* p);
	CALCINFO *handle;


};

#endif