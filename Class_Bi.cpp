#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"

Class_Bi<vector<Class_KXian> >::baseItemType_Container* Class_Bi<vector<Class_KXian> >::base_Container = (Class_Bi<vector<Class_KXian> >::baseItemType_Container*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::container = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;


ostream& operator<<(ostream& out, Class_Bi<vector<Class_KXian>>& biObj)
{
	out << "Bi" << "(";

	out.setf(ios_base::fixed, ios_base::floatfield);
	out.precision(2);
	out.width(4);

	out<< biObj.bi.low << ", ";
	out.width(4);
	out<<  biObj.bi.high  << ")";
	return out;

}


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
			container->reserve(base_Container->size());
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

	// ContainerType*  intermediate = new ContainerType();
	ContainerType*  intermediate = container;

	Class_env *env = Class_env::getInstance();

	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();

	Class_KXian temp = *start;

	int cnt = 0;
	baseItemType_Container::iterator p = start;
	do
	{
		while (p != end && temp == *p)
		{
			temp.merge(*p, d);
			p++;
		}

		if (p == end)
		{
			// TODO: 建立最后的一个  类-笔
			float high = max (start->getHigh(), (p-1)->getHigh());
			float low = min (start->getLow(), (p-1)->getLow());
			(*intermediate)[cnt++] = ContainerType::value_type(&(*start), &(*(p-1)), high, low, d);
			start = p-1;
			break;
		}

		if (Class_KXian::getDirection(temp, *p) == d)
		{
			temp = *p;
			p++;
		}
		else
		{
			// 方向不再一致, 建立一个 类-笔
			float high = max(start->getHigh(), (p-1)->getHigh());
			float low = min(start->getLow(), (p-1)->getLow());
			intermediate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d));
			start = p-1;

			d = -d;
		}
	}while (p != end);
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

