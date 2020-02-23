#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"

Class_Bi<vector<Class_KXian> >::baseItem_Container* Class_Bi<vector<Class_KXian> >::base_Container = (Class_Bi<vector<Class_KXian> >::baseItem_Container*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::container = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;


void Class_Bi<vector<Class_KXian>>::FenBi()
{
	if (base_Container == NULL)
	{
	// 创建 K线 vector
		Class_env *env = Class_env::getInstance();
		int total = env->getTotal();
		base_Container = new baseItem_Container(total);
		for (int i = 0; i < total; i++)
		{
			((Class_KXian)(*base_Container)[i]) = Class_KXian(env->startParm[i], env->endParm[i], env->lowParm[i], env->highParm[i]);
		}
	}

	if (base_Container && !container)
	{
		// 分笔
		container = new ContainerType();
	}
}



/*
实现 笔
（1）笔的构成： 底分型、顶分型构成一笔
（2）笔的破坏：
（3）笔的属性：
      底分型
	  顶分型
	  
*/


template<class baseItem_Container>
Class_Bi<baseItem_Container>::~Class_Bi(void)
{
}


template<class baseItem_Container>
void Class_Bi<baseItem_Container>::accept(int bar)
{
	/*
	if (contains.empty())
	{
		contains.push(bar);
		return;
	} else
	{
		int lastBar = contains.top();
        if (bar - lastBar > 1)
		{
			// handle mergedTop and mergedBot
			if (Class_env::rangeContain(mergedBot, mergedTop, Class_env::getKLow(bar), Class_env::getKHigh(bar)))
				Class_env::updateMergedRange(direction, bar, mergedBot, mergedTop);
			else 
			{

			}
		}

	}*/


}