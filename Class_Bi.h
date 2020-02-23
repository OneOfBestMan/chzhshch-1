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
	/* 类笔 的构造函数*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct, int Cnt)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = Cnt;}

	/* 笔 的构造函数*/
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct)
		:traits(biStart, biEnd, high, low, direct) {KXianCnt = -1;}

	static baseItemType_Container *base_Container;

	/* 经过思考，觉得，从类笔到笔，需要经过2个步骤：先是用分线段的方式，确定大的线段； 然后在线段内部，寻找合适的顶分型、底分型，将线段划分成若干笔；
	   目前的实现，打算先略去 从类笔 到 笔； 而是直接基于 类笔 划分线段。
	*/
	static ContainerType *container; 
	static ContainerType *intermediate; // 用来保存 "类-笔"，是计算 “笔”的中间结果；

	static void FenBi(bool release = false);

	friend ostream& operator<<(ostream&, Class_Bi&);

	static const int MIN_BI_KXIAN = 5; // 一笔 至少包含 5根 无包含关系的K线；

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
	static void FenBi_Step2();

};


#endif