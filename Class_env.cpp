#include "stdafx.h"
#include "Class_env.h"
#include <stdexcept>


using namespace std;

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
    return env;
}

Class_env::~Class_env(void)
{
}


float Class_env::getKHigh(int pos)
{
	if (pos >= firstHighParm && pos < totalBar)
		return highParm[pos];
	else
		throw invalid_argument("下标越界");
}


float Class_env::getKLow(int pos)
{
	if (pos >= firstLowParm && pos < totalBar)
		return lowParm[pos];
	else
		throw invalid_argument("下标越界");
}


float Class_env::getKStart(int pos)
{
	if (pos >= firstStartParm && pos < totalBar)
		return startParm[pos];
	else
		throw invalid_argument("下标越界");
}

float Class_env::getKEnd(int pos)
{
	if (pos >= firstEndParm && pos < totalBar)
		return endParm[pos];
	else
		throw invalid_argument("下标越界");
}