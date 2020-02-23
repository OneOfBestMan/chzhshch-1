#pragma once


#ifndef CLASS_XIANDUAN_H
#define CLASS_XIANDUAN_H

#include "Class_Bi.h"

#include <vector>
using namespace std;

#include "FenXianDuan.h"


template<int grade>
class Class_XianDuan
{
public:
	typedef Class_XianDuan<grade-1> baseItemType;
	typedef vector<baseItemType> baseItemType_Container;
	typedef vector<Class_XianDuan> ContainerType; 

	typedef preDumpTemplate<Class_XianDuan>  preDumpClass;
	typedef FenXianDuanTemplate<Class_XianDuan> FenXianDuanClass;

	//static void printMe() {printf("This is %d\n", GRADE); Class_XianDuan<grade-1>::printMe();}

	static const int GRADE = grade;

	typedef struct
	{
		typename baseItemType *start;
		typename baseItemType *end;
		float high;
		float low;
	} valueType;

	static baseItemType_Container *baseItems;
	static ContainerType *container;
	
	Class_XianDuan(void);
	~Class_XianDuan(void);

	static void HuaFenXianDuan() {}

	

private:
	valueType XianDuan;
};

template<>
class Class_XianDuan<1>
{
public:

	//static void printMe() {printf("This is %d, specialized. \n", GRADE); }

	typedef vector<Class_KXian> KXian_Container;
	typedef Class_Bi<KXian_Container> baseItemType;
    typedef vector<baseItemType> Bi_Container;
	typedef Bi_Container baseItemType_Container;
	typedef vector<Class_XianDuan> ContainerType;

	typedef preDumpTemplate<Class_XianDuan>  preDumpClass;
	typedef FenXianDuanTemplate<Class_XianDuan> FenXianDuanClass;

	static const int GRADE = 1;

	typedef struct {
		baseItemType *start;
		baseItemType *end;
		float high;
		float low;
	} valueType;

    static baseItemType_Container *baseItems;
    static ContainerType *container;

	static void HuaFenXianDuan() {}

private:
	valueType XianDuan;

};


template <int grade>
Class_XianDuan<grade>::Class_XianDuan(void)
{
}

template <int grade>
Class_XianDuan<grade>::~Class_XianDuan(void)
{
}


template<int grade>
typename Class_XianDuan<grade>::baseItemType_Container* Class_XianDuan<grade>::baseItems = (typename Class_XianDuan<grade>::baseItemType_Container*)NULL;

template<int grade>
typename Class_XianDuan<grade>::ContainerType* Class_XianDuan<grade>::container = (typename Class_XianDuan<grade>::ContainerType*)NULL;


template<>
class FenXianDuanTemplate<typename Class_XianDuan<1> >
{
public:
	static void doWork(bool release)
	{
		typedef Class_XianDuan<1> XianDuanClass;
		if (release == false)
		{
			if (XianDuanClass::baseItems == NULL) 
			{
				//XianDuanClass::FenBi();
				XianDuanClass::baseItemType::FenBi();
				XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
			}
			if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
			{
				XianDuanClass::container = new XianDuanClass::ContainerType(XianDuanClass::baseItems->size());
				//下面就是划分线段的具体逻辑
				XianDuanClass::HuaFenXianDuan();
			}
		} else
		{		
			delete XianDuanClass::container;
			XianDuanClass::container = NULL;

			XianDuanClass::baseItemType::FenBi(true);
			XianDuanClass::baseItems = NULL;
		}
	}
};



template<>
void FenXianDuan<typename Class_XianDuan<1>>(bool release);



#endif