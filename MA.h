#pragma once

/*

MA:  实现 均线 相关的逻辑

*/
#include "stdafx.h"
#include "StockFunc.h"
#include "Class_KXian.h"



class MA {
	typedef DATA_TYPE KXianTime;
public:
	typedef vector<float> MASequence;
	typedef vector<Class_KXian> KXianSet;
	typedef enum { MA13, MA21, MA34, MA55, MA_COUNT } MULTIPLE;

	MA() {};

	static void Init(const KXianSet& KXian);

	static void Init(const float* InitByParam, int firstKXian, KXianTime time, MULTIPLE m);

	static void setStrategy(KXianTime timeLong, MULTIPLE maLong, KXianTime timeShort, MULTIPLE maShort);

private:
	static MASequence ma[MONTH_DATA + 1][MA_COUNT];
	
	static MASequence& maLongTime;
	static MASequence& maShortTime;

	static MASequence DONOT_POINT_TO_ME;
};