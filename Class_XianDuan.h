#pragma once


#ifndef CLASS_XIANDUAN_H
#define CLASS_XIANDUAN_H

#include "Class_Bi.h"

#include <vector>
#include <stack>
using namespace std;



template<int grade>
class Class_XianDuan
{
public:
	typedef Class_XianDuan<grade-1> baseItemType;
	typedef vector<baseItemType> baseItemType_Container;
	typedef vector<Class_XianDuan> ContainerType; 

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
    typedef stack<baseItemType> Bi_Container;
	typedef Bi_Container baseItemType_Container;
	typedef vector<Class_XianDuan> ContainerType;

	static const int GRADE = 1;

	typedef struct {
		baseItemType *start;
		baseItemType *end;
		float high;
		float low;
	} valueType;
/*
	Class_XianDuan<1>(void);
	~Class_XianDuan<1>(void);
*/
    static baseItemType_Container *baseItems;
    static ContainerType *container;

	static void HuaFenXianDuan() {}

	//static void FenBi();
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


/*
template<>
Class_XianDuan<1>::Class_XianDuan(void)
{
}

template <>
Class_XianDuan<1>::~Class_XianDuan(void)
{
}*/

/*
template<>
void Class_XianDuan<1>::FenBi()
{
	if (baseItems)
}*/

#endif