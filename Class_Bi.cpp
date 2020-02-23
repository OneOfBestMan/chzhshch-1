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
			// ���� K�� vector
			Class_KXian::initialize();
			base_Container = Class_KXian::container;
		}
		if (base_Container && !container)
		{
			container = new ContainerType();
			container->reserve(base_Container->size());
			// step 1: ����K�߰�����ϵ���ҳ� ��-�����͡���-�׷��ͣ����ǣ���û�п��ǣ������͡��׷���֮�� �� 5��K�ߵ�Ҫ��
			FenBi_Step1();
			// step 2: ��϶����͡��׷���֮�䣬����5��k�ߵ�Ҫ�󣬼�����������ʣ�
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
	Direction d = ASCENDING; // ���ʼ�ĵ�1��2��k�ߣ����Ǽ���֮ǰ�ķ�����ascending�ģ��������㴦�������ϵ��

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
			// TODO: ��������һ��  ��-��
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
			// ������һ��, ����һ�� ��-��
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
ʵ�� ��
��1���ʵĹ��ɣ� �׷��͡������͹���һ��
��2���ʵ��ƻ���
��3���ʵ����ԣ�
      �׷���
	  ������
	  
*/


template<class baseItem_Container>
Class_Bi<baseItem_Container>::~Class_Bi(void)
{
}

