#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

#include "Class_KXian.h"

#include <stack>
#include <vector>

using namespace std;

template <class baseItem_Container> 
class Class_Bi
{
public:
	typedef stack<Class_Bi> ContainerType;
	
	typedef struct {
		typename baseItem_Container::value_type *start;
		typename baseItem_Container::value_type *end;
		float high;
		float low;
	} valueType;



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
	typedef stack<Class_Bi> ContainerType;
	typedef vector<Class_KXian> baseItem_Container;

	typedef struct {
		baseItem_Container::value_type *start;
		baseItem_Container::value_type *end;
		float high;
		float low;
		Direction d;
	} valueType;


	Class_Bi(void) {}
	~Class_Bi(void){}
	Class_Bi(baseItem_Container::value_type* biStart, baseItem_Container::value_type* biEnd, float high, float low, Direction direct)
	                                               { bi.start = biStart; bi.end = biEnd; bi.high = high;  bi.low = low; bi.d = direct;}

	static baseItem_Container *base_Container;
	static ContainerType *container;

	static void FenBi(bool release = false);

private:

	static void FenBi_Step1();
	static void FenBi_Step2();
	valueType bi;

};


#endif