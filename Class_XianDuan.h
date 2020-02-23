#pragma once


#ifndef CLASS_XIANDUAN_H
#define CLASS_XIANDUAN_H

#include "Class_Bi.h"

#include <vector>
#include <ostream>
using namespace std;

#include "FenXianDuan.h"
#include "traits.h"

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

	//static void printMe() {printf("This is %d\n", GRADE); Class_XianDuan<grade-1>::printMe();}

	static const int GRADE = grade;


	static baseItemType_Container *baseItems;
	static ContainerType *container;
	
	Class_XianDuan(void);
	~Class_XianDuan(void);

	static ContainerType* HuaFenXianDuan() {return NULL;}

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
class Class_XianDuan<1>: public traits<Class_Bi<vector<Class_KXian> >, Class_XianDuan<1>> 
{
public:

	//static void printMe() {printf("This is %d, specialized. \n", GRADE); }

	typedef preDumpTemplate<Class_XianDuan>  preDumpClass;
	typedef DumpTemplate<Class_XianDuan> DumpClass;
	typedef DumpTemplateV2<Class_XianDuan> DumpClassV2;
	typedef FenXianDuanTemplate<Class_XianDuan> FenXianDuanClass;

	static const int GRADE = 1;




    static baseItemType_Container *baseItems;
    static ContainerType *container;

	static ContainerType* HuaFenXianDuan() {return NULL;}

	friend ostream& operator<<(ostream&, Class_XianDuan&);

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

/*
template<int grade>
ostream& operator<<(ostream& ostream, typename Class_XianDuan<grade>& objXianDuan)
{
	typedef Class_XianDuan<grade> XianDuanClass;
	ostream << 'X' << XianDuanClass::GRADE << "(";

	ostream.precision(2);
	ostream.width(4);

	ostream<< objXianDuan.XianDuan.low << ", ";
	ostream.width(4);
	ostream<< objXianDuan.XianDuan.high << ")";

	return ostream;
}
*/

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
				//下面就是划分线段的具体逻辑
				XianDuanClass::container = XianDuanClass::HuaFenXianDuan();
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