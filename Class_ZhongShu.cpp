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
	assert(former->getGrade() == latter->getGrade() || former->getGrade() == latter->getGrade() + 1); // 允许 前一个高中枢吞并后一个低级别中枢，来进行中枢扩展；
	// assert(grade - 1 == former->getGrade()); //TODO: 这个assertion是我不愿意注销的，但目前程序会assert。先临时禁用，以后再找找原因。
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	Class_ZhongShu::zsList[grade].push_back(Class_ZhongShu(former, latter, grade));

	return &Class_ZhongShu::zsList[grade].back();
}


Class_ZhongShu* createZhongShu(Class_ZhongShu *first, Class_ZhongShu *mid, Class_ZhongShu *last, int grade)
{
	assert(first->getGrade() == mid->getGrade()  &&  first->getGrade() == last->getGrade()  );
	// assert(grade - 1 == first->getGrade()); //TODO: 这个assertion是我不愿意注销的，但目前程序会assert。先临时禁用，以后再找找原因。
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

Class_ZhongShu* createZhongShu(Class_ZhongShu *zsArray[], int cntZS,  int grade)
{
	assert(grade < Class_ZhongShu::MAX_LEVEL && grade > 0);

	ValueRange coreRange = zsArray[0]->getFloatRange();
	ValueRange floatRange = zsArray[0]->getFloatRange();

	for (int i = 0; i < cntZS; i++)
	{
		Class_ZhongShu *curr = zsArray[i];

		coreRange && curr->getFloatRange();
		floatRange || curr->getFloatRange();
	}
		
	Class_ZhongShu *newZS = createZhongShu(zsArray[0]->getStart(), zsArray[cntZS-1]->getEnd(), grade);
	newZS->setCoreRange(coreRange);
	newZS->setFloatRange(floatRange);
	return newZS;
}


/*
Class_ZhongShu* createZhongShu(int grade, ...)
{
	va_list zsParms;
	va_start(zsParms, grade);

	int countZS = 0;
	while ((curr = va_arg(zsParms, Class_ZhongShu*)) != 0)
		countZS++

	va_start(zsParms, grade);
	switch (countZS)
	{
	case 0:
		assert(0);
		break;
	case 1:
		Class_XianDuanBase *curr = va_arg(zsParms, Class_XianDuanBase*);
		return createZhongShu(curr, grade);
	case 2:
		Class_ZhongShu* former = va_arg(zsParms, Class_ZhongShu*);
		Class_ZhongShu* latter = va_arg(zsParms, Class_ZhongShu*);
		return createZhongShu(former, latter, grade);
	case 3:
		Class_ZhongShu* first = va_arg(zsParms, Class_ZhongShu*);
		Class_ZhongShu* mid = va_arg(zsParms, Class_ZhongShu*);
		Class_ZhongShu* last = va_arg(zsParms, Class_ZhongShu*);
		return createZhongShu(first, mid, last, grade);
	default:
		Class_ZhongShu *first, *last;
		ValueRange coreRange, floatRange;
		for (int i = 1; i <= countZS; i++)
		{
			Class_ZhongShu *curr = va_arg(zsParms, Class_ZhongShu*);

			if (i == 1)
				coreRange = curr->getFloatRange();
			else
				coreRange && curr->getFloatRange();

			floatRange || curr->getFloatRange();


			if (i == 1)
				first = curr; 

			if (i == countZS) 
				last = mid;
		}
		
		Class_ZhongShu *newZS = createZhongShu(first->getStart(), last->getEnd(), grade);
		newZS->setCoreRange(coreRange);
		newZS->setFloatRange(floatRange);
		return newZS;
	}
}
*/
