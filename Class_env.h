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
	DATA_TYPE barKind; // K��ͼ��ʱ������


	gradeSelect grade;
	funcSelect func;
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