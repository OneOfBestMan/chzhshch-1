#pragma once

#ifndef CLASS_KXIAN_H
#define CLASS_KXIAN_H

#include <assert.h>
#include <vector>

#include <ostream>
#include <sstream>

using namespace std;


#include "Debug.h"
#include "traits.h"


class Class_KXian: public traits<void, Class_KXian>
{
public:

	typedef preDumpTemplate<Class_KXian>  preDumpClass;
	typedef DumpTemplate<Class_KXian> DumpClass;

	static ContainerType *container;

	Class_KXian() {}
	Class_KXian(float kStart, float kEnd, float kBot, float kHigh);
	~Class_KXian(void);

	float getStart() const {return Start;}
	float getEnd() const {return End;}

	Class_KXian* getStartRec() {return this;}
	Class_KXian* getEndRec() {return this;}


	friend ostream& operator<< (ostream&, Class_KXian &);

	static void initialize(bool release = false);

	Class_XianDuan<1>* getBaseXianDuanStart() {return NULL;}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return NULL;}

private:
	float Start;
	float End;
};



template<>
class preDumpTemplate<typename Class_KXian>
{
public:
	static void doWork(dumpHelperMap &helperMap)
	{
		if (Class_KXian::container == NULL) return;

		Class_KXian::ContainerType::iterator p = Class_KXian::container->begin();
		Class_KXian::ContainerType::iterator end   = Class_KXian::container->end();
		while (p != end)
		{
			Class_KXian::ContainerType::value_type &item = *p;
			
			stringstream strstream;
			strstream<< item;
			int content = strstream.str().length();

			helperMap[&item] = mappedInfo(content, content + 1); // +1 ´ú±í '|'×Ö·û
			p++;
		}
	}
};



template<>
void preDump<typename Class_KXian> (dumpHelperMap &helperMap);

template<>
void Dump<typename Class_KXian>(dumpHelperMap &helperMap, ostream &file) ;

template<>
void DumpV2<typename Class_KXian>(dumpHelperMap &helperMap, ostream &file) ;

#endif