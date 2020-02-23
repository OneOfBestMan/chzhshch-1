#pragma once
#ifndef CLASS_BI_H
#define CLASS_BI_H

#include "Class_KXian.h"
#include "Class_FenXing.h"

#include <stack>

using namespace std;

template <class baseItem_Container> 
class Class_Bi
{
public:

	typedef struct {
		typename baseItem_Container::value_type *start;
		typename baseItem_Container::value_type *end;
		float high;
		float low;
	} valueType;

	typedef stack<Class_Bi> ContainerType;

	Class_Bi(void) {}
	~Class_Bi(void);
    
	void accept(int bar);


	static baseItem_Container *base_Container;
	static ContainerType *container;

private:

	valueType bi;

	/*typedef struct
	{
		int barIndex;
		int cont; // 这个结构用于处理具有包含关系的k线； cont表示后面有几根k线，和barIndex开始的k线存在包含关系
	} singleBar;

	stack<singleBar*> contains;*/

	/*stack<int> contains;

	stack<int> contains;*/

	/*Class_FenXing start;
	Class_FenXing end;

	float mergedBot;
	float mergedTop;*/


	
};

#endif