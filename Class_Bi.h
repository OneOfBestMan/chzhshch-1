#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

// #define _HAS_ITERATOR_DEBUGGING 0 // �����Ƕ�vector����reserve space�������undef�����Ļ���ʹ��[]����Ԫ�أ��ᱨ�±�Խ�硣 ���ԣ�������#include <vector>֮ǰ���������ر�. ע�⣬���õ�stdafx.h����.


#include "Class_KXian.h"

#include <ostream>
#include <vector>

#include "traits.h"

#include "Display.h"

using namespace std;

template <class baseItem_Container> 
class Class_Bi
{
public:
	typedef vector<Class_Bi> ContainerType;
	typedef typename baseItem_Container::value_type baseItemType;
	




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

	typename baseItem_Container::value_type *start;
	typename baseItem_Container::value_type *end;
	
};

template<>
class Class_Bi<vector<Class_KXian> >: public traits<Class_KXian, Class_Bi<vector<Class_KXian>> >
{
public:

	typedef preDumpTemplate<Class_Bi>  preDumpClass;
	typedef DumpTemplate<Class_Bi> DumpClass;
	typedef DumpTemplateV2<Class_Bi> DumpClassV2;
	typedef DisplayTemplate<Class_Bi> DisplayClass;




	Class_Bi(void) {}
	~Class_Bi(void){}
	/* ��� �Ĺ��캯��*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct, int Cnt)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = Cnt;}

	/* �� �Ĺ��캯��*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = -1;}

	static baseItemType_Container *base_Container;

	/* ����˼�������ã�����ʵ��ʣ���Ҫ����2�����裺�����÷��߶εķ�ʽ��ȷ������߶Σ� Ȼ�����߶��ڲ���Ѱ�Һ��ʵĶ����͡��׷��ͣ����߶λ��ֳ����ɱʣ�
	   Ŀǰ��ʵ�֣���������ȥ ����� �� �ʣ� ����ֱ�ӻ��� ��� �����߶Ρ�
	*/
	static ContainerType *container; 
	static ContainerType *intermediate; // �������� "��-��"���Ǽ��� ���ʡ����м�����

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_Bi&);

	static const int MIN_BI_KXIAN = 5; // һ�� ���ٰ��� 5�� �ް�����ϵ��K�ߣ�

private:

		/*

	������KXianCnt = 4

            |               |
        |   |             | |
        | | |     =       | |
      | | |             | |
    | |               | |
    |                 |
	*/
	int KXianCnt; // �������� ��������ϵ��K�ߣ�����򣬽��ڴ�����ʡ���ʱ����Ч�����ڡ��ʡ��������û����


	/* FenBi_Step1  ʵ�ֵ��� ��65���е� �ֱʶ��� */
	static void FenBi_Step1();

	/* FenBi_Step2 ʵ�ֵ��� ��69���е� �ֱ�˼�� */
	/* ֮ǰ��ʵ�֣��ڵ��Ե�ʱ�򣬷����ǲ��Եġ� Ŀǰ�����ֱʵ�ʵ�֣������ϻ��ǻ����߶Σ������������ǡ���ʡ��У���Щ�ǳ�С����ʣ� */
	static void FenBi_Step2();

};


#endif