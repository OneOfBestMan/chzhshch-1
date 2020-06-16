#pragma once


#ifndef CLASS_XIANDUAN_H
#define CLASS_XIANDUAN_H

#include "Class_Bi.h"

#include <vector>
#include <ostream>
using namespace std;

#include "Class_env.h"

#include "FenXianDuan.h"
#include "traits.h"
#include "Display.h"
#include "Zig_Common.h"

/*
template<int grade> class Class_XianDuan;
template<int grade> ostream& operator<< (ostream& ostream, Class_XianDuan<grade> & objXianDuan);
ostream& operator<< (ostream& ostream, Class_XianDuan<1> & objXianDuan);
*/

template<int grade>
class Class_XianDuan: public traits<Class_XianDuan<grade-1>, Class_XianDuan<grade> >
{
public:
	typedef preDumpTemplate<Class_XianDuan>  preDumpClass;
	typedef DumpTemplate<Class_XianDuan> DumpClass;
	typedef DumpTemplateV2<Class_XianDuan> DumpClassV2;
	typedef FenXianDuanTemplate<Class_XianDuan> FenXianDuanClass;
	typedef DisplayTemplate<Class_XianDuan> DisplayClass;

	typedef typename baseItemType_Container::iterator baseItemIterator;

	//static void printMe() {printf("This is %d\n", GRADE); Class_XianDuan<grade-1>::printMe();}

	static const int GRADE = grade;

	static baseItemType_Container *baseItems;
	static ContainerType *container;

	static int *debugInfPnt; // InfPnt = Inflection Point 拐点
	
	Class_XianDuan(void);
	~Class_XianDuan(void);

	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd); // v2分线段算法
	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct); // v1分线段算法


	/* 这两个接口，是给中枢服务的，中枢最小的构成单位 */
	Class_XianDuan<1>* getBaseXianDuanStart() {return getStart()->getBaseXianDuanStart();}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return getEnd()->getBaseXianDuanEnd();}
	
	friend ostream& operator<<(ostream& file, Class_XianDuan& objXianDuan) 
	{
		typedef Class_XianDuan<grade> XianDuanClass;

		file << 'X' << XianDuanClass::GRADE << "(";

		file.setf(ios_base::fixed, ios_base::floatfield);
		file.precision(2);
		file.width(4);

		file<< objXianDuan.getLow() << ", ";
		file.width(4);
		file<< objXianDuan.getHigh() << ")";

		return file;
	}


private:

};

template<>
class Class_XianDuan<1>: public traits<Class_Bi, Class_XianDuan<1>> 
{
public:

	//static void printMe() {printf("This is %d, specialized. \n", GRADE); }

	typedef preDumpTemplate<Class_XianDuan>  preDumpClass;
	typedef DumpTemplate<Class_XianDuan> DumpClass;
	typedef DumpTemplateV2<Class_XianDuan> DumpClassV2;
	typedef FenXianDuanTemplate<Class_XianDuan> FenXianDuanClass;
	typedef DisplayTemplate<Class_XianDuan> DisplayClass;


	typedef baseItemType_Container::iterator baseItemIterator;


	static const int GRADE = 1;


	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd); // v2分线段算法
	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct); // v1分线段 算法

    static baseItemType_Container *baseItems;
    static ContainerType *container;

	static int *debugInfPnt; // InfPnt = Inflection Point 拐点

	friend ostream& operator<<(ostream&, Class_XianDuan&);

	Class_XianDuan<1>* getBaseXianDuanStart() {return this;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return this;}

	
private:

	

};


template <int grade>
Class_XianDuan<grade>::Class_XianDuan(void)
{
}

template <int grade>
Class_XianDuan<grade>::~Class_XianDuan(void)
{
}

template <int grade>
Class_XianDuan<grade>::Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct)
{
	this->Start = biStart;
	this->End = biEnd;
	this->d = direct;

	float high = (*biStart).getHigh();
	float low = (*biStart).getLow();

	while (biStart != biEnd + 1)
	{
		high = max(high, (*biStart).getHigh());
		low = min(low, (*biStart).getLow());
		biStart++;
	}

	this->High = high;
	this->Low = low;


}

template <int grade>
Class_XianDuan<grade>::Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd)
{
	baseItemType& start = *biStart;
	baseItemType& end = *biEnd;
	
	// assert(start.getDirection() == end.getDirection() && IComparable::getDirection(start,end) == start.getDirection()); 此处会触发错误：当只有1笔30%涨幅的时候，线段则只包含1笔；IComparable的判断失效
	assert(start.getDirection() == end.getDirection());


	this->Start = biStart;
	this->End = biEnd;
	this->d = start.getDirection();

	float high = (*biStart).getHigh();
	float low = (*biStart).getLow();

	/* 本来一直觉得，ZIG算法之后，大区间内，找顶分型、底分型，划分线段，应该可以保证线段高低点在首尾底分型出现，但是实际上也为尽然
	问题出在哪里？  问题出在，在handle confilict的时候，会对 顶顶、底底、顶底不足3笔进行处理，这就导致一些顶、底分型被删除了，所以，剩下的
	顶底分形 所形成的线段，就不一定满足  高、低点，一定出现在首尾了。

	此处，可以写一个assert，来检查这样的情况是否严重。 原则上来说，应该尽量避免出现高低点不出现在首尾的情况。 
	可以把这样的情况，当成bug去修。
	*/
	while (biStart != biEnd + 1)
	{
		high = max(high, (*biStart).getHigh());
		low = min(low, (*biStart).getLow());
		biStart++;
	}

	this->High = high;
	this->Low = low;
}

template<int grade>
typename Class_XianDuan<grade>::baseItemType_Container* Class_XianDuan<grade>::baseItems = (typename Class_XianDuan<grade>::baseItemType_Container*)NULL;

template<int grade>
typename Class_XianDuan<grade>::ContainerType* Class_XianDuan<grade>::container = (typename Class_XianDuan<grade>::ContainerType*)NULL;

template<int grade>
int* Class_XianDuan<grade>::debugInfPnt = (int*)NULL;

template<>
class FenXianDuanTemplate<typename Class_XianDuan<1> >
{
public:
	typedef  Class_XianDuan<1>::baseItemType_Container::iterator baseItemIterator;
	typedef  Class_XianDuan<1>::baseItemType baseItemType;
	typedef Class_XianDuan<1>::ContainerType ContainerType;
	typedef Class_XianDuan<1> XianDuanClass; // 这样定义，就可以使用FenXianDuan_Common.h中的函数了
	typedef Class_XianDuan<1>::baseItemType_Container baseItemType_Container;
	typedef typename ContainerType::iterator itemIterator;
	typedef Class_XianDuan<1> itemType;

	static void doWork(bool release)
	{
		typedef Class_XianDuan<1> XianDuanClass;
		if (release == false)
		{
			if (XianDuanClass::baseItems == NULL) 
			{
				//XianDuanClass::FenBi();
				baseItemType::FenBi();
				XianDuanClass::baseItems = baseItemType::container;
			}
			if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
			{
				if ((Class_env::getInstance())->treatBiAsZS())
				{
					/* 中枢划分的算法，把XianDuan<1>作为最小级别的中枢； 因此，如果我们希望把“笔”作为最小级别的中枢，那么只需要
					把 笔 复制到XianDuan<1>中，即：让每一个级别1的线段，就是一笔，就可以让笔成为 最小级别的中枢。
					*/
					XianDuanClass::container = new ContainerType();
					XianDuanClass::baseItemIterator start = XianDuanClass::baseItems->begin();
					XianDuanClass::baseItemIterator end = XianDuanClass::baseItems->end();
					while (start != end)
					{
						XianDuanClass::container->push_back(XianDuanClass(start, start, (*start).getDirection()));
						start++;
					}
				}else
				{
					//下面就是划分线段的具体逻辑
					baseItemIterator begin = XianDuanClass::baseItems->begin();
					baseItemIterator end = XianDuanClass::baseItems->end();
					//XianDuanClass::container = startFenXianDuan(begin, end);
					XianDuanClass::container = startFenXianDuan_v2();
				}
			}
		} else
		{		
			delete XianDuanClass::container;
			XianDuanClass::container = NULL;

			XianDuanClass::baseItemType::FenBi(true);
			XianDuanClass::baseItems = NULL;
		}
	}
private:
#include "FenXianDuan_Common.h"
};



template<>
void FenXianDuan_PostOrderTravel<typename Class_XianDuan<1>>(bool release);



#endif