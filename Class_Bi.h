#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

// #define _HAS_ITERATOR_DEBUGGING 0 // �����Ƕ�vector����reserve space�������undef�����Ļ���ʹ��[]����Ԫ�أ��ᱨ�±�Խ�硣 ���ԣ�������#include <vector>֮ǰ���������ر�. ע�⣬���õ�stdafx.h����.


#include "Class_KXian.h"

#include <ostream>
#include <vector>

using namespace std;

template <class baseItem_Container> 
class Class_Bi
{
public:
	typedef vector<Class_Bi> ContainerType;
	typedef typename baseItem_Container::value_type baseItemType;
	
	typedef struct {
		typename baseItem_Container::value_type *start;
		typename baseItem_Container::value_type *end;
		float high;
		float low;
	} valueType;

	float getHigh() const {return bi.high;}
	float getLow()const {return bi.low;}
	baseItemType*  getStart()const {return bi.start;}
	baseItemType*  getEnd()const {return bi.end;}


	Class_Bi(void) {}
	~Class_Bi(void);


	static baseItem_Container *base_Container;
	static ContainerType *container;

	static void FenBi() {}

private:

	valueType bi;
};

template<>
class Class_Bi<vector<Class_KXian> >
{
public:
	typedef vector<Class_Bi> ContainerType;
	typedef Class_KXian baseItemType;
	typedef vector<Class_KXian> baseItemType_Container;

	typedef preDumpTemplate<Class_Bi>  preDumpClass;
	typedef DumpTemplate<Class_Bi> DumpClass;
	typedef DumpTemplateV2<Class_Bi> DumpClassV2;



	typedef struct {
		baseItemType* start;
		baseItemType* end;
		float high;
		float low;
		Direction d;
		int KXianCnt; // �������� ��������ϵ��K�ߣ�����򣬽��ڴ�����ʡ���ʱ����Ч�����ڡ��ʡ��������û����
	/*

	������KXianCnt = 4

            |               |
        |   |             | |
        | | |     =       | |
      | | |             | |
    | |               | |
    |                 |
	*/
	} valueType;

	float getHigh()const {return bi.high;}
	float getLow()const {return bi.low;}
	baseItemType*  getStart() const{return bi.start;}
	baseItemType*  getEnd()const {return bi.end;}


	Class_Bi(void) {}
	~Class_Bi(void){}
	/* ��� �Ĺ��캯��*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct, int Cnt)
	                                               { bi.start = biStart; bi.end = biEnd; bi.high = high;  bi.low = low; bi.d = direct; bi.KXianCnt = Cnt;}
	/* �� �Ĺ��캯��*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct)
	                                               { bi.start = biStart; bi.end = biEnd; bi.high = high;  bi.low = low; bi.d = direct; bi.KXianCnt = -1;}

	static baseItemType_Container *base_Container;
	static ContainerType *container;
	static ContainerType *intermediate; // �������� "��-��"���Ǽ��� ���ʡ����м�����

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_Bi&);

	static const int MIN_BI_KXIAN = 5; // һ�� ���ٰ��� 5�� �ް�����ϵ��K�ߣ�

private:
	valueType bi;
	static void FenBi_Step1();
	static void FenBi_Step2();

};


#endif