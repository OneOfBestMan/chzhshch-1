#pragma once

#ifndef DEBUG_H
#define DEBUG_H


#include <map>
using namespace std;

typedef struct mappedInfo{
	int content; 
	int total; 
	mappedInfo(int c, int t) : content(c), total(t) {}
	mappedInfo() {}
} mappedInfo;
typedef map<void*, mappedInfo> dumpHelperMap;

class Class_KXian;


template<class XianDuanClass >
class preDumpTemplate
{
public:

	static void doWork(dumpHelperMap &helperMap)
	{
		XianDuanClass::ContainerType::iterator p = XianDuanClass::container.begin();
		XianDuanClass::ContainerType::iterator end   = XianDuanClass::container.end();
		
		while (p != end)
		{
			XianDuanClass::ContainerType::value_type &item = *p;
			XianDuanClass::baseItemType_Container::iterator xStart = item.XianDuan.start;
			XianDuanClass::baseItemType_Container::iterator xEnd = item.XianDuan.end;
			
			int total = 0;
			while (xStart != xEnd)
			{
				total += helperMap[&(*xStart)].total;
				xStart++;
			}
			
			sprintf(tempSpace, "(%4.2f, %4.2f)", item.XianDuan.low, item.XianDuan.high);
			int content = strlen(tempSpace);
			helperMap[&item] = dumpHelperMap::mapped_type(content, total);
			p++;
		}
	}
};



template<class XianDuan_or_Bi>
void preDump(dumpHelperMap &helperMap)
{
	XianDuan_or_Bi::baseItemType::preDumpClass::doWork(helperMap);

	XianDuan_or_Bi::preDumpClass::doWork(helperMap);
}


#endif
