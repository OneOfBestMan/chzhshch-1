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
			// ���� K�� vector
			Class_KXian::initialize();
			base_Container = Class_KXian::container;
		}
		if (base_Container && !container)
		{
			container = new ContainerType();
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
			// TODO: ��������һ��  ��-��
			break;
		}
		if (Class_KXian::getDirection(temp, (*base_Container)[i]) == d)
		{
			temp = (*base_Container)[i];
			i++;
		}
		else
		{
			// ������һ��
			d = -d;
		}
	} while (true);
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

