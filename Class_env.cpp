#include "stdafx.h"
#include "Class_env.h"
#include "FenXianDuan.h"

#include <stdexcept>
using namespace std;

#include "Class_XianDuan.h"


#if 0
template <>
void preDump<typename Class_KXian>(dumpHelperMap &helperMap)
{

	char tempSpace[20]; // ���������Ĵ���(####.##, ####.##)

	if (Class_KXian::container == NULL) return;

	Class_KXian::ContainerType::iterator p = Class_KXian::container->begin();
	Class_KXian::ContainerType::iterator end   = Class_KXian::container->end();

	while (p != end)
	{
		Class_KXian::ContainerType::value_type &item = *p;

		sprintf(tempSpace, "(%4.2f, %4.2f)", item.getLow(), item.getHigh());
		int content = strlen(tempSpace);

		helperMap[&item] = dumpHelperMap::mapped_type(content, content + 1);
		p++;
	}
}


template <>
void preDump<typename Class_Bi<vector<Class_KXian> >>(dumpHelperMap &helperMap)
{
	typedef Class_Bi<vector<Class_KXian> >  BiClass;

	char tempSpace[20]; // ���������Ĵ���(####.##, ####.##)

	preDump<BiClass::baseItemType>(helperMap);

	if (BiClass::container == NULL) return;

	BiClass::ContainerType::iterator p = BiClass::container->begin();
	BiClass::ContainerType::iterator end   = BiClass::container->end();

	while (p != end)
	{
		BiClass::ContainerType::value_type &item = *p;
		BiClass::baseItemType *xStart = item.bi.start;
		BiClass::baseItemType *xEnd = item.bi.end;

		int total = 0;
		while (xStart != xEnd) 
		{
			total += helperMap[xStart].total;
			xStart++;
		}

		sprintf(tempSpace, "(%4.2f, %4.2f)", item.bi.low, item.bi.high);
		int content = strlen(tempSpace);

		helperMap[&item] = dumpHelperMap::mapped_type(content, total);
		p++;
	}
}

#endif

#if 0
template<>
void HuaFenXianDuan<typename Class_XianDuan<1> >(bool release)
{
	typedef Class_XianDuan<1> XianDuanClass;

	if (release == false)
	{
		if (XianDuanClass::baseItems == NULL) 
		{
			//XianDuanClass::FenBi();
			XianDuanClass::baseItemType::FenBi();
			XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
		}
		
		if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
		{
			XianDuanClass::container = new XianDuanClass::ContainerType();
			//������ǻ����߶εľ����߼�
			XianDuanClass::HuaFenXianDuan();
		}
	} else
	{
		delete XianDuanClass::container;
		XianDuanClass::container = NULL;

		XianDuanClass::baseItemType::FenBi(true);
		XianDuanClass::baseItems = NULL;
	}
}
#endif


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

/*
float Class_env::getKHigh(int pos)
{
	if (pos >= firstHighParm && pos < totalBar)
		return highParm[pos];
	else
		throw invalid_argument("�±�Խ��");
	return 0;
}


float Class_env::getKLow(int pos)
{
	if (pos >= firstLowParm && pos < totalBar)
		return lowParm[pos];
	else
		throw invalid_argument("�±�Խ��");
	return 0;
}


float Class_env::getKStart(int pos)
{
	if (pos >= firstStartParm && pos < totalBar)
		return startParm[pos];
	else
		throw invalid_argument("�±�Խ��");
	return 0;
}

float Class_env::getKEnd(int pos)
{
	if (pos >= firstEndParm && pos < totalBar)
		return endParm[pos];
	else
		throw invalid_argument("�±�Խ��");
	return 0;
}
*/

#if 0
bool Class_env::rangeContain(float k1B, float k1T, float k2B, float k2T)
{
    /*
	���һ���������εı�����ϵ
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
		// ������K���У����������ϵ
		mergedBot = max(getKLow(bar), mergedBot);
        mergedTop = max(getKHigh(bar), mergedTop);
	} else {
		// �½���k���У����������ϵ
		mergedBot = min(getKLow(bar), mergedBot);
        mergedTop = min(getKHigh(bar), mergedTop);

	}
}
#endif