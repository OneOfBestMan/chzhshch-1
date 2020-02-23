#pragma once


#ifndef CLASS_ENV_H
#define CLASS_ENV_H

#include "StockFunc.h"


#include "Class_XianDuan.h"

/*
 封装一些全局变量  读取K线的高、低点的方法

*/

template<class XianDuanClass>
void DumpOut()
{
	/* 大致思路是：
	（1）从高级线段、到低级线段，逐层打印； 在打印高层线段时，其包含的低层线段，也被打印出来；
	（2）在完成(1)之后，低层次线段队列可能依然有线段没有打印出来，这是由于这些低层线段，
	其对应的高层线段没有建立，即，低级别线段尚未完成，所以高级别线段也没有建立。 因此，需要把这些低层级线段，逐层打印出来。
	*/
	
}


template<class XianDuanClass>
void HuaFenXianDuan(bool release = false)
{
	int grade = XianDuanClass::GRADE;

	if (release == false)
	{
		if (XianDuanClass::baseItems == NULL)
		{
			HuaFenXianDuan<XianDuanClass::baseItemType>(release);
			XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
		}
		
		if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
		{
			XianDuanClass::container = new XianDuanClass::ContainerType(XianDuanClass::baseItems->size());
			//下面就是划分线段的具体逻辑
			XianDuanClass::HuaFenXianDuan();
		}
	} else
	{
		delete XianDuanClass::container;
		XianDuanClass::container = NULL;

		HuaFenXianDuan<XianDuanClass::baseItemType>(release);
		XianDuanClass::baseItems = NULL;
	}
}


template<>
void HuaFenXianDuan<typename Class_XianDuan<1> >(bool release);



class Class_env
{
public:
	~Class_env(void);
	static Class_env* getInstance(CALCINFO *p);
	static Class_env* getInstance();
	static Class_env* env;

	/*
    static float getKHigh(int pos);
	static float getKLow(int pos);
	static float getKStart(int pos);
	static float getKEnd(int pos);
	static bool rangeContain(float k1B, float k1T, float k2B, float k2T);
	static void updateMergedRange(int direction, int bar, float &mergedBot, float &mergedTop);
	*/

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

	bool operator==(const Class_env &secondEnv) const 
	{
		return (!strcmp(stockName, secondEnv.stockName) &&
			totalBar  == secondEnv.totalBar);
	}

	static void releaseOldCalculation(Class_env *p)
	{

	}

private:


	Class_env(CALCINFO* p);
	CALCINFO *handle;


};

#endif