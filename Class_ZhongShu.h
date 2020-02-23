#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"


template<typename XianDuanType>
IZhongShu* createZhongShu(XianDuanType* xianDuan);

template<class XianDuanType>
IZhongShu* createZhongShu(IZhongShu *zs, XianDuanType *xianDuan);

template<>
IZhongShu* createZhongShu<>(IZhongShu *zs, Class_XianDuan<1> *xianDuan);


template <int grade>
class Class_ZhongShu: protected traits_ZhongShu<typename Class_ZhongShu<grade-1>, typename Class_XianDuan<grade+1>, typename Class_ZhongShu<grade>>
{
public:

	static const int GRADE = grade;

	
	veryBaseXianDuanType*  getStart() 
	{
		if (Start) return Start;

		switch (content.type)
		{
		case 1:
			Start = content.c1.first->getStart();
			break;
		case 2:
			Start = content.c2.first->getStart();
			break;
		case 3:
			Start = content.c3.start->getBaseXianDuanStart();
			break;
		case 4:
			Start = content.c4.xianDuan->getBaseXianDuanStart();
			break;
		case 5:
			Start = content.c5.former->getStart();
			break;
		default:
			assert(0);
		}

		return Start;
	}
	
	veryBaseXianDuanType*  getEnd() 
	{
		if (End) return End;

		switch (content.type)
		{
		case 1:
			End = content.c1.first->getEnd();
			break;
		case 2:
			End = content.c2.first->getEnd();
			break;
		case 3:
			End = content.c3.last->getBaseXianDuanEnd();
			break;
		case 4:
			End = content.c4.xianDuan->getBaseXianDuanEnd();
			break;
		case 5:
			End = content.c5.latter->getEnd();
			break;
		default:
			assert(0);
		}

		return End;
	}


private:
	Class_ZhongShu(void) {}
	~Class_ZhongShu(void) {}

	Class_ZhongShu(typename Class_XianDuan<grade+1> *xianDuan): traits_ZhongShu(xianDuan) {}
	Class_ZhongShu(Class_ZhongShu<grade-1>* subZhongShu, Class_XianDuan<grade+1>* xianDuan): traits_ZhongShu(subZhongShu, xianDuan) {}
	Class_ZhongShu(IZhongShu *former, IZhongShu *latter): traits_ZhongShu(former, latter) {}

	/* 第三买、卖点： 次级别走势回拉（三卖）或回跌（三买），不触及中枢区间，则该次级别走势的终点是第三买卖点； 并且要求，该次级别的起点，应该高于中枢波动高点（三买）或低于中枢波动低点（三卖） */

	/*
	getStart() 返回中枢起始的那一笔
	getEnd() 返回中枢回拉、回踩的那一笔，第三买、卖点就是这一笔的末端点；
	*/

	friend IZhongShu* createZhongShu<>(Class_XianDuan<grade+1> *xianDuan);
	friend IZhongShu* createZhongShu<>(IZhongShu*, Class_XianDuan<grade+1>*);

	friend IZhongShu* createZhongShu<>(IZhongShu*, IZhongShu*); // type = 5
};




/* 中枢0，等同于级别1线段； 这样在应用TurningPoint/JustaPosition算法的时候，对于级别2线段，可以把它的每条子线段（级别是1），视作级别0中枢。这样，中枢合并的分析程序就可以简单进行模板化，而不必在区分中枢级别1，是由3条线段1构成的；而更高级别中枢，须由3个低级别中枢构成。 */
template<>
class Class_ZhongShu<0>: public traits_ZhongShu<void, typename Class_XianDuan<1>, typename Class_ZhongShu<0>>
{
public:
	static const int GRADE = 0;


	veryBaseXianDuanType*  getStart() 
	{
		if (Start) return Start;

		switch (content.type)
		{
		case 1:
		case 2:
		case 3:
		default:
			assert(0);
			break;
		case 4:
			Start = content.c4.xianDuan->getBaseXianDuanStart();
			break;
		}

		return Start;
	}
	
	veryBaseXianDuanType*  getEnd() 
	{
		if (End) return End;

		switch (content.type)
		{
		case 1:
		case 2:
		case 3:
		default:
			assert(0);
			break;
		case 4:
			End = content.c4.xianDuan->getBaseXianDuanEnd();
			break;
		}

		return End;
	}
private:
	Class_ZhongShu(Class_XianDuan<1> *xianDuan): traits_ZhongShu(xianDuan) {}

	friend IZhongShu* createZhongShu<>(Class_XianDuan<1>* xianDuan);
	friend IZhongShu* createZhongShu<>(IZhongShu *zs, Class_XianDuan<1>* xianDuan); // 这个只是用来通过编译检查的，什么都不做。
};

// 中枢创建的 4种方式

template<class XianDuanType>
IZhongShu* createZhongShu(XianDuanType* xianDuan)
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
	IZhongShu *result = new Class_ZhongShu<xianDuan->GRADE-1>(xianDuan);
	return result;
}

template<class XianDuanType>
IZhongShu* createZhongShu(IZhongShu *zs, XianDuanType *xianDuan)
{
/*
  处理：
  中枢级别0 + 线段级别2 == 中枢级别1
  中枢级别1 + 线段级别3 == 中枢级别2
   ......
  以此类推

  如果，第2个参数是线段级别1，通过包含assert(0)的专门化函数来处理
*/
#define AS_ZhongShu(iZhongShu, grade) static_cast<Class_ZhongShu<grade>*>(iZhongShu)

	assert(zs->getGrade() == xianDuan->GRADE - 2);
	IZhongShu *result = new Class_ZhongShu<xianDuan->GRADE-1>(AS_ZhongShu(zs, xianDuan->GRADE - 2), xianDuan);
	return result;

#undef AS_ZhongShu
}

IZhongShu* createZhongShu(IZhongShu *former, IZhongShu *latter, int newGrade)
{
	assert(former->getGrade() == latter->getGrade());
	IZhongShu *result = new Class_ZhongShu<newGrade>(former, latter);
	return result;
}


#endif