#include "stdafx.h"
#include "Class_env.h"
#include "FenXianDuan.h"

#include <stdexcept>
#include <fstream>
using namespace std;

#include "Class_XianDuan.h"
#include "debug.h"

Class_env* Class_env::env = NULL;


Class_env::Class_env(CALCINFO *p)
{
	handle = p;

    if (p->m_nNumParam != 4) 
		throw invalid_argument("���� ������������ȷ");

	if (p->m_pCalcParam[0].m_nParamStart < 0 || 
		p->m_pCalcParam[1].m_nParamStart < 0 ||
		p->m_pCalcParam[2].m_nParamStart < 0 || 
		p->m_pCalcParam[3].m_nParamStart < 0)
		throw invalid_argument("���� �����������ȷ");

	highParm = p->m_pCalcParam[0].m_pfParam;
	firstHighParm = p->m_pCalcParam[0].m_nParamStart;

    lowParm = p->m_pCalcParam[1].m_pfParam;
	firstLowParm = p->m_pCalcParam[1].m_nParamStart;

	startParm = p->m_pCalcParam[2].m_pfParam;
	firstStartParm =  p->m_pCalcParam[2].m_nParamStart;

	endParm = p->m_pCalcParam[3].m_pfParam;
	firstEndParm = p->m_pCalcParam[3].m_nParamStart;

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
		// �ͷ�֮ǰ�ķ������
		if (env)
		{
			FenXianDuan<Class_XianDuan<7>>(true);
			delete env;
		}

		env = newEnv;
		FenXianDuan<Class_XianDuan<7>>(false);

		static bool doOnce = false;
		if (!doOnce)
		{
			doOnce = true;
			dumpHelperMap map;
			ofstream file("c:\\log.txt");
			preDump<Class_XianDuan<7>>(map);
			Dump<Class_XianDuan<7>>(map, file);
		}


	}

    return env;
}


Class_env* Class_env::getInstance()
{
	if (env) 
		return env;
	else
		throw invalid_argument("env��NULL�����ȵ���getInstance(CALCINFO *p)��env���й���");
}

Class_env::~Class_env(void)
{
	delete stockName;
}
