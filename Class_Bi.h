#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

#include "Class_KXian.h"

//#include <stack>
#include <vector>

using namespace std;

template <class baseItem_Container> 
class Class_Bi
{
public:
	typedef vector<Class_Bi> ContainerType;
	
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
	typedef vector<Class_Bi> ContainerType;
	typedef Class_KXian baseItemType;
	typedef vector<Class_KXian> baseItemType_Container;

	typedef struct {
		baseItemType* start;
		baseItemType* end;
		float high;
		float low;
		Direction d;
	} valueType;


	Class_Bi(void) {}
	~Class_Bi(void){}
	Class_Bi(baseItemType_Container::value_type* biStart, baseItemType_Container::value_type* biEnd, float high, float low, Direction direct)
	                                               { bi.start = biStart; bi.end = biEnd; bi.high = high;  bi.low = low; bi.d = direct;}

	static baseItemType_Container *base_Container;
	static ContainerType *container;

	static void FenBi(bool release = false);
	
	valueType bi;
private:

	static void FenBi_Step1();
	static void FenBi_Step2();

};


#endif