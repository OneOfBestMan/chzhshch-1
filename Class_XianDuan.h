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
	
	Class_XianDuan(void);
	~Class_XianDuan(void);

	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct);

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


	Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct);

    static baseItemType_Container *baseItems;
    static ContainerType *container;

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
	this->Start = &(*biStart);
	this->End = &(*biEnd);
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


template<int grade>
typename Class_XianDuan<grade>::baseItemType_Container* Class_XianDuan<grade>::baseItems = (typename Class_XianDuan<grade>::baseItemType_Container*)NULL;

template<int grade>
typename Class_XianDuan<grade>::ContainerType* Class_XianDuan<grade>::container = (typename Class_XianDuan<grade>::ContainerType*)NULL;


template<>
class FenXianDuanTemplate<typename Class_XianDuan<1> >
{
public:
	typedef  Class_XianDuan<1>::baseItemType_Container::iterator baseItemIterator;
	typedef  Class_XianDuan<1>::baseItemType baseItemType;
	typedef Class_XianDuan<1>::ContainerType ContainerType;
	typedef Class_XianDuan<1> XianDuanClass; // 这样定义，就可以使用FenXianDuan_Common.h中的函数了
	typedef Class_XianDuan<1>::baseItemType_Container baseItemType_Container;

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
					XianDuanClass::container = startFenXianDuan(begin, end);
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