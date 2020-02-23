#pragma once

#include "StockFunc.h"


/*
 ��װһЩȫ�ֱ���  ��ȡK�ߵĸߡ��͵�ķ���

*/

class Class_env
{
public:
	~Class_env(void);
	static Class_env* getInstance(CALCINFO *p);
	static Class_env* getInstance();
	static Class_env* env;
    float getKHigh(int pos);
	float getKLow(int pos);
	float getKStart(int pos);
	float getKEnd(int pos);
private:


	Class_env(CALCINFO* p);
	CALCINFO *handle;

    float *highParm;
    int firstHighParm;

	float *lowParm;
	int firstLowParm;

	float *startParm;
	int firstStartParm;

	float *endParm;
	int firstEndParm;

	int totalBar;
};
