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


class Class_LeiBi: public traits<Class_KXian, Class_LeiBi>
{
public:

	typedef preDumpTemplate<Class_LeiBi>  preDumpClass;
	typedef DumpTemplate<Class_LeiBi> DumpClass;
	typedef DumpTemplateV2<Class_LeiBi> DumpClassV2;
	typedef DisplayTemplate<Class_LeiBi> DisplayClass;


	typedef ContainerType::value_type leiBi; // ���
	typedef ContainerType::iterator leiBiIter, BiIter;


	Class_LeiBi(void) {}
	~Class_LeiBi(void){}
	/* ��� �Ĺ��캯��*/
	Class_LeiBi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct, int Cnt)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = Cnt;}

	static baseItemType_Container *base_Container;

	/* ����˼�������ã�����ʵ��ʣ���Ҫ����2�����裺�����÷��߶εķ�ʽ��ȷ������߶Σ� Ȼ�����߶��ڲ���Ѱ�Һ��ʵĶ����͡��׷��ͣ����߶λ��ֳ����ɱʣ�
	   Ŀǰ��ʵ�֣���������ȥ ����� �� �ʣ� ����ֱ�ӻ��� ��� �����߶Ρ�
	*/
	static ContainerType *container; 
	static ContainerType *intermediate; // �������� "��-��"���Ǽ��� ���ʡ����м�����

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_LeiBi&);

	static const int MIN_BI_KXIAN = 5; // һ�� ���ٰ��� 5�� �ް�����ϵ��K�ߣ�

	Class_XianDuan<1>* getBaseXianDuanStart() {return NULL;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return NULL;}

	int getKXianCnt() {return KXianCnt;}
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
	// static void FenBi_Step2();

};


class Class_Bi: public traits<Class_LeiBi, Class_Bi>
{
public:

	typedef preDumpTemplate<Class_Bi>  preDumpClass;
	typedef DumpTemplate<Class_Bi> DumpClass;
	typedef DumpTemplateV2<Class_Bi> DumpClassV2;
	typedef DisplayTemplate<Class_Bi> DisplayClass;

	typedef baseItemType_Container::value_type leiBi;


	Class_Bi(void) {}
	~Class_Bi(void){}
	/* �� �Ĺ��캯��*/
	Class_Bi(leiBi* biStart, leiBi* biEnd);

	Class_Bi(leiBi* biStart);

	static baseItemType_Container *base_Container;

	/* ����˼�������ã�����ʵ��ʣ���Ҫ����2�����裺�����÷��߶εķ�ʽ��ȷ������߶Σ� Ȼ�����߶��ڲ���Ѱ�Һ��ʵĶ����͡��׷��ͣ����߶λ��ֳ����ɱʣ�
	   Ŀǰ��ʵ�֣���������ȥ ����� �� �ʣ� ����ֱ�ӻ��� ��� �����߶Ρ�
	*/
	static ContainerType *container; 

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_Bi&);

	static const int MIN_BI_KXIAN = 5; // һ�� ���ٰ��� 5�� �ް�����ϵ��K�ߣ�

	Class_XianDuan<1>* getBaseXianDuanStart() {return NULL;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return NULL;}

	int getKXianCnt() {return KXianCnt;}
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
	//static void FenBi_Step1();

	/* FenBi_Step2 ʵ�ֵ��� ��69���е� �ֱ�˼�� */
	/* ֮ǰ��ʵ�֣��ڵ��Ե�ʱ�򣬷����ǲ��Եġ� Ŀǰ�����ֱʵ�ʵ�֣������ϻ��ǻ����߶Σ������������ǡ���ʡ��У���Щ�ǳ�С����ʣ� */
	static void FenBi_Step2();

	static bool bckwdSearch(ContainerType::reverse_iterator, Class_LeiBi *, Direction);
	//static bool bckwdSearch(int, Class_LeiBi *, Direction);

	static void checkValid();
};

#endif