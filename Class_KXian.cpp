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

template<>
void Dump<typename Class_KXian>(dumpHelperMap &helperMap, ostream &file)
{
	Class_KXian::DumpClass::doWork(helperMap, file);
}

template<>
void DumpV2<typename Class_KXian>(dumpHelperMap &helperMap, ostream &file)
{
	return;
}



Class_KXian::ContainerType* Class_KXian::container = NULL;


ostream& operator<<(ostream &out, Class_KXian &kxian)
{
	out << 'K' << "(";

	out.setf(ios_base::fixed, ios_base::floatfield);
	out.precision(2);
	out.width(4);

	out<< kxian.getLow() << ", ";
	out.width(4);
	out<<  kxian.getHigh() << ")";
	return out;
}


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

Class_KXian::Class_KXian(float kStart, float kEnd, float kBot, float kHigh) : traits((void*)0xdeaddead, (void*)0xdeaddead , kHigh, kBot)
{
	Start = kStart;
	End = kEnd;

}

Class_KXian::~Class_KXian(void)
{
}

