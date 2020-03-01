#pragma once


#ifndef CLASS_ENV_H
#define CLASS_ENV_H

#include "include_api.h"
#include <map>


using namespace std;

/*
 ��װһЩȫ�ֱ���  ��ȡK�ߵĸߡ��͵�ķ���

*/


class Class_env
{
public:

	
	typedef enum {
	/* ����������ͣ� �ӱ� �� ������ͬ������߶� */	
		LEI_BI = -1, BI=0, XIANDUAN_1 = 1, XIANDUAN_2 = 2, XIANDUAN_3 = 3, XIANDUAN_4 = 4, XIANDUAN_5 = 5, XIANDUAN_6 = 6, XIANDUAN_7 = 7,
	/* ����ļ��� */	
		ZS_0 = 0, ZS_1=1, ZS_2 = 2, ZS_3 = 3, ZS_4 = 4, ZS_5 = 5, ZS_6 = 6, ZS_7 = 7
	} gradeSelect;

	
	typedef enum {
		/* ���ڲ�ͬ����������ͣ���Ҫ��������̻���һ���Ǹ����߶δ����￪ʼ�������������һ���Ǹ����߶���ʼ�۸񡢽����۸� */
		OUTPUT_TIME = 0, OUTPUT_PRICE = 1, 
		/* ����Ŀ�ʼ�ͽ�����ǡ����������á�������߼ۡ�������ͼ� */
		OUTPUT_ZS_START_END_MARK = 0, OUTPUT_ZS_DURATION = 1, OUTPUT_ZS_LOWPRICE = 2, OUTPUT_ZS_HIGHPRICE = 3
	} funcSelect;

	typedef enum { XIANDUAN = 0, ZHONGSHU = 1} ZsXdSelect;

	~Class_env(void);
	static Class_env* getInstance(CALCINFO *p);
	static Class_env* getInstance();
	void outputResult();
	static Class_env* env;

	int   getTotal() const {return totalBar;}

	const STKDATA*	KXian;

	SPLITDATA*  XR_XD; // ��Ȩ����Ϣ
	int num_XR_XD; // ��Ȩ����Ϣ ����

	int totalBar;
	char* stockName;
	DATA_TYPE barKind; // K��ͼ��ʱ������


	// ouput ���
	gradeSelect grade;
	funcSelect func;
	ZsXdSelect ZSorXD;

	// ��С����ĵ�λ���Ǳʻ����߶Σ�����1�֡�5��k��ͼ����XianDuan<1>��Ϊ��С��������ࣻ ��֮�ϵ�K��ͼ���ѱ���Ϊ��С���������
	bool treatBiAsZS() {return barKind > MIN5_DATA;}

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