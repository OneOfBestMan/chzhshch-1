#include "stdafx.h"
#include "Class_KXian.h"

#include "StockFunc.h"

#include "Class_env.h"
/*

K线   用于刻画分型的顶点、线段的高低点、中枢的高低点、中枢的第三买卖点

*/


template<>
void preDump<typename Class_KXian> (dumpHelperMap &helperMap)
{
	Class_KXian::preDumpClass::doWork(helperMap);
}




Direction operator-(const Direction& d)
{
	switch (d)
	{
	case ASCENDING:
		return DESCENDING;
	case DESCENDING:
		return ASCENDING;
	default:
		assert(0);
	}
	return d;
}


Class_KXian::ContainerType* Class_KXian::container = NULL;

void Class_KXian::initialize(bool release)
{
	if (release == false)
	{
		// 创建 K线 vector
		Class_env *env = Class_env::getInstance();
		int total = env->getTotal();
		
		if (total > 0)
			container = new ContainerType(total);
		
		for (int i = 0; i < total; i++)
		{
			(*container)[i] = Class_KXian(env->startParm[i], env->endParm[i], env->lowParm[i], env->highParm[i]);
		}
	} else 
	{
		delete container;
		container = NULL;
	}
}

bool Class_KXian::isAscending(const Class_KXian& firstKXian, const Class_KXian &secondKXian)
{
	return (firstKXian.getHigh() < secondKXian.getHigh()  && firstKXian.getLow() < secondKXian.getLow());
}

bool Class_KXian::isDecending(const Class_KXian& firstKXian, const Class_KXian &secondKXian)
{
	return (firstKXian.getHigh() > secondKXian.getHigh()  && firstKXian.getLow() > secondKXian.getLow());
}

bool Class_KXian::isEnclosing(const Class_KXian& firstKXian, const Class_KXian &secondKXian)
{
	return (firstKXian.getHigh() >= secondKXian.getHigh()  && firstKXian.getLow() <= secondKXian.getLow()) ||
		   (firstKXian.getHigh() <= secondKXian.getHigh()  && firstKXian.getLow() >= secondKXian.getLow());
}

Direction Class_KXian::getDirection(const Class_KXian& firstKXian, const Class_KXian &secondKXian)
{
	if (isAscending(firstKXian, secondKXian))
		return ASCENDING;
	else if (isDecending(firstKXian, secondKXian))
		return DESCENDING;
	else
		return ENCLOSING;
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


float Class_KXian::getStart() const
{
	return bar.kStart;
}

float Class_KXian::getEnd() const
{
	return bar.kEnd;
}

float Class_KXian::getHigh() const
{
	return bar.kHigh;
}

float Class_KXian::getLow() const
{
	return bar.kBot;
}