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
			(*base_Container)[i] = Class_KXian(env->startParm[i], env->endParm[i], env->lowParm[i], env->highParm[i]);
		}
	}

	if (base_Container && !container)
	{
		container = new ContainerType();

		// step 1: 考虑K线包含关系，找出 类似 顶分型、底分型；但是，并没有考虑，顶分型、底分型之间 有 5根K线的要求。

		// step 2: 结合顶分型、底分型之间，至少5根k线的要求，继续处理各个笔；

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

