#include "stdafx.h"
#include "MA.h"
#include "Class_env.h"

MA::MASequence MA::ma[MONTH_DATA + 1][MA::MA_COUNT];
MA::MASequence MA::DONOT_POINT_TO_ME;

MA::MASequence& MA::maLongTime = MA::DONOT_POINT_TO_ME;
MA::MASequence& MA::maShortTime = MA::DONOT_POINT_TO_ME;

void MA::Init(const KXianSet& KXian) {
	Class_env *env = Class_env::getInstance();
	KXianTime time = env->barKind;

	/* 在此处 实现 计算 MA的逻辑 */
	for (int i = 0; i < KXian.size(); i++)
		for (int j = MA13; i < MA_COUNT; i++)
			ma[time][j][i] = 0;
}


void MA::Init(const float* param, int startFrom, KXianTime time, MULTIPLE m) {
	Class_env *env = Class_env::getInstance();

	for (int i = 0; i < startFrom; i++)
		ma[time][m][i] = 0;
	for (int i = startFrom; i < env->getTotal(); i++)
		ma[time][m][i] = param[i];
}

void MA::setStrategy(KXianTime timeLong, MULTIPLE maLong, KXianTime timeShort, MULTIPLE maShort) {
	maLongTime = ma[timeLong][maLong];
	maShortTime = ma[timeShort][maShort];
}



