#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

// #define _HAS_ITERATOR_DEBUGGING 0 // 由于是对vector进行reserve space，如果不undef这个宏的话，使用[]访问元素，会报下表越界。 所以，必须在#include <vector>之前，将这个宏关闭. 注意，放置到stdafx.h中了.


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


	typedef ContainerType::value_type leiBi; // 类笔
	typedef ContainerType::iterator leiBiIter, BiIter;


	Class_LeiBi(void) {}
	~Class_LeiBi(void){}
	/* 类笔 的构造函数*/
	Class_LeiBi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct, int Cnt)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = Cnt;}

	static baseItemType_Container *base_Container;

	/* 经过思考，觉得，从类笔到笔，需要经过2个步骤：先是用分线段的方式，确定大的线段； 然后在线段内部，寻找合适的顶分型、底分型，将线段划分成若干笔；
	   目前的实现，打算先略去 从类笔 到 笔； 而是直接基于 类笔 划分线段。
	*/
	static ContainerType *container; 
	static ContainerType *intermediate; // 用来保存 "类-笔"，是计算 “笔”的中间结果；

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_LeiBi&);

	static const int MIN_BI_KXIAN = 5; // 一笔 至少包含 5根 无包含关系的K线；

	Class_XianDuan<1>* getBaseXianDuanStart() {return NULL;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return NULL;}

	int getKXianCnt() {return KXianCnt;}
private:

		/*

	解释下KXianCnt = 4

            |               |
        |   |             | |
        | | |     =       | |
      | | |             | |
    | |               | |
    |                 |
	*/
	int KXianCnt; // 包含几根 不包含关系的K线，这个域，仅在处理“类笔”的时候有效；对于“笔”，这个域没意义


	/* FenBi_Step1  实现的是 第65课中的 分笔定义 */
	static void FenBi_Step1();

	/* FenBi_Step2 实现的是 第69课中的 分笔思想 */
	/* 之前的实现，在调试的时候，发现是不对的。 目前看，分笔的实现，本质上还是划分线段，特征向量就是“类笔”中，那些非常小的类笔； */
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
	/* 笔 的构造函数*/
	Class_Bi(leiBi* biStart, leiBi* biEnd);

	Class_Bi(leiBi* biStart);

	static baseItemType_Container *base_Container;

	/* 经过思考，觉得，从类笔到笔，需要经过2个步骤：先是用分线段的方式，确定大的线段； 然后在线段内部，寻找合适的顶分型、底分型，将线段划分成若干笔；
	   目前的实现，打算先略去 从类笔 到 笔； 而是直接基于 类笔 划分线段。
	*/
	static ContainerType *container; 

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_Bi&);

	static const int MIN_BI_KXIAN = 5; // 一笔 至少包含 5根 无包含关系的K线；

	Class_XianDuan<1>* getBaseXianDuanStart() {return NULL;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return NULL;}

	int getKXianCnt() {return KXianCnt;}
private:

		/*

	解释下KXianCnt = 4

            |               |
        |   |             | |
        | | |     =       | |
      | | |             | |
    | |               | |
    |                 |
	*/
	int KXianCnt; // 包含几根 不包含关系的K线，这个域，仅在处理“类笔”的时候有效；对于“笔”，这个域没意义


	/* FenBi_Step1  实现的是 第65课中的 分笔定义 */
	//static void FenBi_Step1();

	/* FenBi_Step2 实现的是 第69课中的 分笔思想 */
	/* 之前的实现，在调试的时候，发现是不对的。 目前看，分笔的实现，本质上还是划分线段，特征向量就是“类笔”中，那些非常小的类笔； */
	static void FenBi_Step2();

	static bool bckwdSearch(ContainerType::reverse_iterator, Class_LeiBi *, Direction);
	//static bool bckwdSearch(int, Class_LeiBi *, Direction);

	static void checkValid();
};

#endif