#include "stdafx.h"
#include "Class_ZhongShu.h"



const ValueRange ValueRange::NullValueRange = ValueRange(0,0);


list<Class_ZhongShu>  Class_ZhongShu::zsList[MAX_LEVEL];

// ���ഴ���� 4�ַ�ʽ


Class_ZhongShu* createZhongShu(Class_XianDuanBase* xianDuan, int grade)
{
/*
  ����
  �߶μ���1 == ���༶��0

  3�����༶��0 == ���༶��1
  �߶μ���2 == ���༶��1

  3�����༶��1 == ���༶��2
  �߶μ���3 == ���༶��2
  ......
  ��������
*/
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade >= 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(xianDuan, grade));

	return &Class_ZhongShu::zsList[grade].back();
}

Class_ZhongShu* createZhongShu(Class_ZhongShu *zs, Class_XianDuanBase *xianDuan, int grade)
{
/*
  ����
  ���༶��0 + �߶μ���2 == ���༶��1
  ���༶��1 + �߶μ���3 == ���༶��2
   ......
  �Դ�����

  �������2���������߶μ���1��ͨ������assert(0)��ר�Ż�����������
*/
/*#define AS_ZhongShu(iZhongShu, grade) static_cast<Class_ZhongShu<grade>*>(iZhongShu)

	assert(zs->getGrade() == xianDuan->GRADE - 2);
	IZhongShu *result = new Class_ZhongShu<xianDuan->GRADE-1>(AS_ZhongShu(zs, xianDuan->GRADE - 2), xianDuan);
	return result;

#undef AS_ZhongShu*/

	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(zs, xianDuan, grade));

	return &Class_ZhongShu::zsList[grade].back();
}

Class_ZhongShu* createZhongShu(Class_ZhongShu *former, Class_ZhongShu *latter, int grade)
{
	assert(former->getGrade() == latter->getGrade());
	// assert(grade - 1 == former->getGrade()); //TODO: ���assertion���Ҳ�Ը��ע���ģ���Ŀǰ�����assert������ʱ���ã��Ժ�������ԭ��
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(former, latter, grade));

	return &Class_ZhongShu::zsList[grade].back();
}


Class_ZhongShu* createZhongShu(Class_ZhongShu *first, Class_ZhongShu *mid, Class_ZhongShu *last, int grade)
{
	assert(first->getGrade() == mid->getGrade()  &&  first->getGrade() == last->getGrade()  );
	assert(grade - 1 == first->getGrade());
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(first, mid, last, grade));

	return &Class_ZhongShu::zsList[grade].back();
}

Class_ZhongShu* createZhongShu(Class_XianDuanBase *start, Class_XianDuanBase *end, int grade)
{
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(start,end, grade));

	return &Class_ZhongShu::zsList[grade].back();
}


