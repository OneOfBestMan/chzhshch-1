#pragma once


#ifndef CLASS_ENV_H
#define CLASS_ENV_H

#include "StockFunc.h"


#include "Class_XianDuan.h"

/*
 封装一些全局变量  读取K线的高、低点的方法

*/

template<class XianDuanClass>
void HuaFenXianDuan()
{
	int grade = XianDuanClass::GRADE;

	if (XianDuanClass::baseItems == NULL)
	{
		HuaFenXianDuan<XianDuanClass::baseItemType>();
		XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
	}

	if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
	{
		XianDuanClass::container = new XianDuanClass::ContainerType(XianDuanClass::baseItems->size());
		//下面就是划分线段的具体逻辑
		XianDuanClass::HuaFenXianDuan();
	}
}


template<>
void HuaFenXianDuan<typename Class_XianDuan<1> >();



class Class_env
{
public:
	~Class_env(void);
	static Class_env* getInstance(CALCINFO *p);
	static Class_env* getInstance();
	static Class_env* env;
    static float getKHigh(int pos);
	static float getKLow(int pos);
	static float getKStart(int pos);
	static float getKEnd(int pos);
	int   getTotal() {return totalBar;}
	static bool rangeContain(float k1B, float k1T, float k2B, float k2T);
	static void updateMergedRange(int direction, int bar, float &mergedBot, float &mergedTop);

    float *highParm;
    int firstHighParm;

	float *lowParm;
	int firstLowParm;

	float *startParm;
	int firstStartParm;

	float *endParm;
	int firstEndParm;

	int totalBar;



private:


	Class_env(CALCINFO* p);
	CALCINFO *handle;


};

#endif