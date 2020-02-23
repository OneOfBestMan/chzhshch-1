#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"

Class_Bi<vector<Class_KXian> >::baseItem_Container* Class_Bi<vector<Class_KXian> >::base_Container = (Class_Bi<vector<Class_KXian> >::baseItem_Container*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::container = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;


void Class_Bi<vector<Class_KXian>>::FenBi(bool release)
{
	if (release == false)
	{
		if (base_Container == NULL)
		{
			// 创建 K线 vector
			Class_KXian::initialize();
			base_Container = Class_KXian::container;
		}
		if (base_Container && !container)
		{
			container = new ContainerType();
			// step 1: 考虑K线包含关系，找出 类-顶分型、类-底分型；但是，并没有考虑，顶分型、底分型之间 有 5根K线的要求。
			FenBi_Step1();
			// step 2: 结合顶分型、底分型之间，至少5根k线的要求，继续处理各个笔；
			FenBi_Step2();
		}
	} else
	{
		delete container;
		container = NULL;

		Class_KXian::initialize(true);
		base_Container = NULL;
	}
}

void Class_Bi<vector<Class_KXian>>::FenBi_Step1()
{
	Direction d = ASCENDING; // 从最开始的第1、2根k线，我们假设之前的方向是ascending的，这样方便处理包含关系。

	ContainerType*  intermediate = new ContainerType();

	Class_env *env = Class_env::getInstance();
	int total = env->getTotal();


	Class_KXian temp = (*base_Container)[0];

	int i = 1;
	do 
	{
		while (i<total && temp == (*base_Container)[i])
		{
			temp.merge((*base_Container)[i], d);
			i++;
		}
	
		if (i == total)
		{
			// TODO: 建立最后的一个  类-笔
			break;
		}
		if (Class_KXian::getDirection(temp, (*base_Container)[i]) == d)
		{
			temp = (*base_Container)[i];
			i++;
		}
		else
		{
			// 方向不再一致
			d = -d;
		}
	} while (true);
}

void Class_Bi<vector<Class_KXian>>::FenBi_Step2()
{
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

