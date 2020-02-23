#include "stdafx.h"
#include "Class_KXian.h"

#include "StockFunc.h"

#include "Class_env.h"
/*

K线   用于刻画分型的顶点、线段的高低点、中枢的高低点、中枢的第三买卖点

*/

Class_KXian::ContainerType* Class_KXian::container = NULL;

void Class_KXian::initialize()
{
	// 创建 K线 vector
	Class_env *env = Class_env::getInstance();
	int total = env->getTotal();
	container = new ContainerType(total);
	for (int i = 0; i < total; i++)
	{
		(*container)[i] = Class_KXian(env->startParm[i], env->endParm[i], env->lowParm[i], env->highParm[i]);
	}
}

Class_KXian::Class_KXian(float kStart, float kEnd, float kBot, float kHigh)
{

	bar.kStart = kStart;
	bar.kEnd = kEnd;
	bar.kHigh = kHigh;
	bar.kBot = kBot;
}

Class_KXian::~Class_KXian(void)
{
}


float Class_KXian::getStart()
{
	return bar.kStart;
}

float Class_KXian::getEnd()
{
	return bar.kEnd;
}

float Class_KXian::getHigh()
{
	return bar.kHigh;
}

float Class_KXian::getLow()
{
	return bar.kBot;
}