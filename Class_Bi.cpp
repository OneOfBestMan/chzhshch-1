#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"

Class_Bi<vector<Class_KXian> >::baseItem_Container* Class_Bi<vector<Class_KXian> >::base_Container = (Class_Bi<vector<Class_KXian> >::baseItem_Container*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::container = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;


void Class_Bi<vector<Class_KXian>>::FenBi()
{
	if (base_Container == NULL)
	{
	// ���� K�� vector
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

		// step 1: ����K�߰�����ϵ���ҳ� ���� �����͡��׷��ͣ����ǣ���û�п��ǣ������͡��׷���֮�� �� 5��K�ߵ�Ҫ��

		// step 2: ��϶����͡��׷���֮�䣬����5��k�ߵ�Ҫ�󣬼�����������ʣ�

	}
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

