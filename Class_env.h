#pragma once


#ifndef CLASS_ENV_H
#define CLASS_ENV_H

#include "StockFunc.h"
#include <map>


#include "Class_XianDuan.h"

using namespace std;

/*
 ��װһЩȫ�ֱ���  ��ȡK�ߵĸߡ��͵�ķ���

*/

#if 0

typedef struct mappedInfo{
	int content; 
	int total; 
	mappedInfo(int c, int t) : content(c), total(t) {}
	mappedInfo() {}
} mappedInfo;

typedef map<void*, mappedInfo> dumpHelperMap;

template <class XianDuanClass>
void preDump(dumpHelperMap &helperMap)
{
	char tempSpace[20]; // ���������Ĵ���(####.##, ####.##)

	preDump<XianDuanClass::baseItemType>();


	if (XianDuanClass::container == NULL) return;

	XianDuanClass::ContainerType::iterator p = XianDuanClass::container.begin();
	XianDuanClass::ContainerType::iterator end   = XianDuanClass::container.end();

	while (p != end)
	{
		XianDuanClass::ContainerType::value_type &item = *p;
		XianDuanClass::baseItemType_Container::iterator xStart = item.XianDuan.start;
		XianDuanClass::baseItemType_Container::iterator xEnd = item.XianDuan.end;

		int total = 0;
		while (xStart != xEnd)
		{
			total += helperMap[&(*xStart)].total;
			xStart++;
		}

		sprintf(tempSpace, "(%4.2f, %4.2f)", item.XianDuan.low, item.XianDuan.high);
		int content = strlen(tempSpace);

		helperMap[&item] = dumpHelperMap::mapped_type(content, total);
		p++;
	}
}

template<class XianDuanClass>
void Dump()
{
	/* ����˼·�ǣ�
	��1���Ӹ߼��߶Ρ����ͼ��߶Σ�����ӡ�� �ڴ�ӡ�߲��߶�ʱ��������ĵͲ��߶Σ�Ҳ����ӡ������
	��2�������(1)֮�󣬵Ͳ���߶ζ��п�����Ȼ���߶�û�д�ӡ����������������Щ�Ͳ��߶Σ�
	���Ӧ�ĸ߲��߶�û�н����������ͼ����߶���δ��ɣ����Ը߼����߶�Ҳû�н����� ��ˣ���Ҫ����Щ�Ͳ㼶�߶Σ�����ӡ������
	��3��ʵ�ֵ�ʱ�򣬰����ԵͲ���߲㣬�����ȡ �����ռ�� ���
	*/


	dumpHelperMap helperMap;

	preDump<XianDuanClass>(helperMap);

	Dump<XianDuanClass::baseItemType>(); // �ȴ�ӡ �� �߶�

	XianDuanClass::ContainerType::iterator begin = XianDuanClass::container->begin();
	XianDuanClass::ContainerType::iterator end = XianDuanClass::container->end();

}
#endif

class Class_env
{
public:

	/* ����������ͣ� �ӱ� �� ������ͬ������߶� */
	typedef enum {BI=0, XIANDUAN_1 = 1, XIANDUAN_2 = 2, XIANDUAN_3 = 3, XIANDUAN_4 = 4, XIANDUAN_5 = 5, XIANDUAN_6 = 6, XIANDUAN_7 = 7} gradeSelect;

	/* ���ڲ�ͬ����������ͣ���Ҫ��������̻���һ���Ǹ����߶δ����￪ʼ�������������һ���Ǹ����߶���ʼ�۸񡢽����۸� */
	typedef enum {OUTPUT_TIME = 0, OUTPUT_PRICE = 1} funcSelect;


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


	gradeSelect grade;
	funcSelect func;
	float *resultBuf;


	bool operator==(const Class_env &secondEnv) const 
	{
		return (!strcmp(stockName, secondEnv.stockName) &&
			totalBar  == secondEnv.totalBar);
	}

private:

	Class_env(CALCINFO* p);
	CALCINFO *handle;


};

#endif