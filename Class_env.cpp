#include "stdafx.h"
#include "Class_env.h"

#include <stdexcept>
using namespace std;

#include "Class_XianDuan.h"




template<>
void HuaFenXianDuan<typename Class_XianDuan<1> >()
{
	typedef Class_XianDuan<1> XianDuanClass;

	if (XianDuanClass::baseItems == NULL) 
	{
		//XianDuanClass::FenBi();
		XianDuanClass::baseItemType::FenBi();
		XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
	}

	if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
	{
		XianDuanClass::container = new XianDuanClass::ContainerType();
		//下面就是划分线段的具体逻辑
		XianDuanClass::HuaFenXianDuan();
	}
}



Class_env* Class_env::env = NULL;


Class_env::Class_env(CALCINFO *p)
{
	handle = p;

    if (p->m_nNumParam != 4) 
		throw invalid_argument("输入 参数个数不正确");

	if (p->m_pCalcParam[0].m_nParamStart < 0 || 
		p->m_pCalcParam[1].m_nParamStart < 0 ||
		p->m_pCalcParam[2].m_nParamStart < 0 || 
		p->m_pCalcParam[3].m_nParamStart < 0)
		throw invalid_argument("输入 数组参数不正确");

	highParm = p->m_pCalcParam[0].m_pfParam;
	firstHighParm = p->m_pCalcParam[0].m_nParamStart;

    lowParm = p->m_pCalcParam[1].m_pfParam;
	firstLowParm = p->m_pCalcParam[1].m_nParamStart;

	startParm = p->m_pCalcParam[2].m_pfParam;
	firstStartParm =  p->m_pCalcParam[2].m_nParamStart;

	endParm = p->m_pCalcParam[3].m_pfParam;
	firstEndParm = p->m_pCalcParam[3].m_nParamStart;

	totalBar = p->m_nNumData;
	
}

Class_env* Class_env::getInstance(CALCINFO *p)
{
	if (env) return env;
	env = new Class_env(p);
    return env;
}


Class_env* Class_env::getInstance()
{
	if (env) 
		return env;
	else
		throw invalid_argument("env是NULL，请先调用getInstance(CALCINFO *p)对env进行构造");
}

Class_env::~Class_env(void)
{
}


float Class_env::getKHigh(int pos)
{
	/*if (pos >= firstHighParm && pos < totalBar)
		return highParm[pos];
	else
		throw invalid_argument("下标越界");*/
	return 0;
}


float Class_env::getKLow(int pos)
{
	/*if (pos >= firstLowParm && pos < totalBar)
		return lowParm[pos];
	else
		throw invalid_argument("下标越界");*/
	return 0;
}


float Class_env::getKStart(int pos)
{
	/*
	if (pos >= firstStartParm && pos < totalBar)
		return startParm[pos];
	else
		throw invalid_argument("下标越界");*/
	return 0;
}

float Class_env::getKEnd(int pos)
{
	/*
	if (pos >= firstEndParm && pos < totalBar)
		return endParm[pos];
	else
		throw invalid_argument("下标越界");*/
	return 0;
}



bool Class_env::rangeContain(float k1B, float k1T, float k2B, float k2T)
{
    /*
	检测一下两种情形的保函关系
                   k1T
	    k2T         |  k2T  
         |          |   |
	k1T  |          |   |
     |   |          |   |
     |   |          |   |
    k1B  |          |  k2B
         |          |
	    k2B        k1B
	*/

	return (k1T <= k2T  && k1B >= k2B) || (k1T >= k2T && k1B <= k2B);
}

void Class_env::updateMergedRange(int direction, int bar, float &mergedBot, float &mergedTop)
{
	if (direction == 1) 
	{
		// 上升的K线中，处理包含关系
		mergedBot = max(getKLow(bar), mergedBot);
        mergedTop = max(getKHigh(bar), mergedTop);
	} else {
		// 下降的k线中，处理包含关系
		mergedBot = min(getKLow(bar), mergedBot);
        mergedTop = min(getKHigh(bar), mergedTop);

	}
}