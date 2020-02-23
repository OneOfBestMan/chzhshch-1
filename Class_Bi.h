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
		int cont; // ����ṹ���ڴ�����а�����ϵ��k�ߣ� cont��ʾ�����м���k�ߣ���barIndex��ʼ��k�ߴ��ڰ�����ϵ
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