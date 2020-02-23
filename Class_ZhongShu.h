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

	/* ���������㣺 �μ������ƻ�������������ص������򣩣��������������䣬��ôμ������Ƶ��յ��ǵ��������㣻 ����Ҫ�󣬸ôμ������㣬Ӧ�ø������ನ���ߵ㣨���򣩻�������ನ���͵㣨������ */

	/*
	getStart() ����������ʼ����һ��
	getEnd() ��������������زȵ���һ�ʣ����������������һ�ʵ�ĩ�˵㣻
	*/

	friend IZhongShu* createZhongShu<>(Class_XianDuan<grade+1> *xianDuan);
	friend IZhongShu* createZhongShu<>(IZhongShu*, Class_XianDuan<grade+1>*);

	friend IZhongShu* createZhongShu<>(IZhongShu*, IZhongShu*); // type = 5
};




/* ����0����ͬ�ڼ���1�߶Σ� ������Ӧ��TurningPoint/JustaPosition�㷨��ʱ�򣬶��ڼ���2�߶Σ����԰�����ÿ�����߶Σ�������1������������0���ࡣ����������ϲ��ķ�������Ϳ��Լ򵥽���ģ�廯�����������������༶��1������3���߶�1���ɵģ������߼������࣬����3���ͼ������๹�ɡ� */
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
	friend IZhongShu* createZhongShu<>(IZhongShu *zs, Class_XianDuan<1>* xianDuan); // ���ֻ������ͨ��������ģ�ʲô��������
};

// ���ഴ���� 4�ַ�ʽ

template<class XianDuanType>
IZhongShu* createZhongShu(XianDuanType* xianDuan)
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
	IZhongShu *result = new Class_ZhongShu<xianDuan->GRADE-1>(xianDuan);
	return result;
}

template<class XianDuanType>
IZhongShu* createZhongShu(IZhongShu *zs, XianDuanType *xianDuan)
{
/*
  ����
  ���༶��0 + �߶μ���2 == ���༶��1
  ���༶��1 + �߶μ���3 == ���༶��2
   ......
  �Դ�����

  �������2���������߶μ���1��ͨ������assert(0)��ר�Ż�����������
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