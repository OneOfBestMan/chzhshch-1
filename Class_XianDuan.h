#pragma once


#ifndef CLASS_XIANDUAN_H
#define CLASS_XIANDUAN_H

#include "Class_Bi.h"

#include <vector>
using namespace std;



template<int grade>
class Class_XianDuan
{
public:

	static const int GRADE = grade;

	typedef vector<Class_XianDuan<grade-1> > baseXianDuan_Container;

	static void printMe() {printf("This is %d\n", GRADE); Class_XianDuan<grade-1>::printMe();}

	typedef struct
	{
		typename baseXianDuan_Container::value_type *start;
		typename baseXianDuan_Container::value_type *end;
		float high;
		float low;
	} valueType;

	typedef vector<Class_XianDuan> ContainerType; 

	static baseXianDuan_Container *baseItems;

	static ContainerType *container;
	
	
	Class_XianDuan(void) {}
	~Class_XianDuan(void) {}


private:
	valueType XianDuan;
};

template<>
class Class_XianDuan<1>
{
public:

	static void printMe() {printf("This is %d, specialized. \n", GRADE); }

	typedef vector<Class_KXian> KXian_Container;
    typedef vector<Class_Bi<KXian_Container>> Bi_Container;

	static const int GRADE = 1;

	typedef struct {
		Class_Bi<KXian_Container> *start;
		Class_Bi<KXian_Container> *end;
		float high;
		float low;
	} valueType;

	typedef vector<Class_XianDuan> ContainerType;
	typedef Bi_Container baseXianDuan_Container;

    static baseXianDuan_Container *baseItems;
    static ContainerType *container;

private:
	valueType XianDuan;

};


#endif