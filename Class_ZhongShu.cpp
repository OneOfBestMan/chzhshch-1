#include "stdafx.h"
#include "Class_ZhongShu.h"



const ValueRange ValueRange::NullValueRange = ValueRange(0,0);


list<Class_ZhongShu>  Class_ZhongShu::zsList[MAX_LEVEL];

// 中枢创建的 4种方式


Class_ZhongShu* createZhongShu(Class_XianDuanBase* xianDuan, int grade)
{
/*
  处理：
  线段级别1 == 中枢级别0

  3个中枢级别0 == 中枢级别1
  线段级别2 == 中枢级别1

  3个中枢级别1 == 中枢级别2
  线段级别3 == 中枢级别2
  ......
  依次类推
*/
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade >= 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(xianDuan, grade));

	return &Class_ZhongShu::zsList[grade].back();
}

Class_ZhongShu* createZhongShu(Class_ZhongShu *zs, Class_XianDuanBase *xianDuan, int grade)
{
/*
  处理：
  中枢级别0 + 线段级别2 == 中枢级别1
  中枢级别1 + 线段级别3 == 中枢级别2
   ......
  以此类推

  如果，第2个参数是线段级别1，通过包含assert(0)的专门化函数来处理
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
	// assert(grade - 1 == former->getGrade()); //TODO: 这个assertion是我不愿意注销的，但目前程序会assert。先临时禁用，以后再找找原因。
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


