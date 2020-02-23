#include "stdafx.h"
#include "Class_env.h"
#include "FenXianDuan.h"

#include <stdexcept>
#include <fstream>
#include <sstream>
using namespace std;

#include "Class_XianDuan.h"
#include "debug.h"

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

	startParm = p->m_pCalcParam[0].m_pfParam;
	firstStartParm = p->m_pCalcParam[0].m_nParamStart;

    endParm = p->m_pCalcParam[1].m_pfParam;
	firstEndParm = p->m_pCalcParam[1].m_nParamStart;

	lowParm = p->m_pCalcParam[2].m_pfParam;
	firstLowParm =  p->m_pCalcParam[2].m_nParamStart;

	highParm = p->m_pCalcParam[3].m_pfParam;
	firstHighParm = p->m_pCalcParam[3].m_nParamStart;

	totalBar = p->m_nNumData;

	if (p->m_strStkLabel)
	{
		stockName = new char[strlen(p->m_strStkLabel) + 1];
		stockName = strcpy(stockName, p->m_strStkLabel);
	}
}

Class_env* Class_env::getInstance(CALCINFO *p)
{
	Class_env* newEnv = new Class_env(p);

	if (env && *env == *newEnv)
	{
		delete newEnv;
	} else
	{
		// 释放之前的分析结果
		if (env)
		{
			FenXianDuan_PostOrderTravel<Class_XianDuan<7>>(true);
			delete env;
		}

		env = newEnv;
		FenXianDuan_PostOrderTravel<Class_XianDuan<7>>(false);

		static bool doOnce = false;
		doOnce = true;
		dumpHelperMap map;

		stringstream filename;
		filename<<"c:\\" << env->stockName << ".txt";

		ofstream file(filename.str().c_str());
		preDump<Class_XianDuan<7>>(map);
		DumpV2<Class_XianDuan<7>>(map, file);

	}

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
	delete stockName;
}
