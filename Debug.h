#pragma once

#ifndef DEBUG_H
#define DEBUG_H


#include <map>
#include <ostream>
#include <sstream> // to use istringstream
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
		if (XianDuanClass::container == NULL) return;

		XianDuanClass::ContainerType::iterator p = XianDuanClass::container->begin();
		XianDuanClass::ContainerType::iterator end   = XianDuanClass::container->end();
		
		while (p != end)
		{
			XianDuanClass::ContainerType::value_type &item = *p;
			XianDuanClass::baseItemType *xStart = item.getStart();
			XianDuanClass::baseItemType *xEnd = item.getEnd();
			
			int total = 0;
			while (xStart != xEnd)
			{
				total += helperMap[xStart].total;
				xStart++;
			}
			
			stringstream strstream;
			strstream << item;

			int content = strstream.str().length();
			helperMap[&item] = dumpHelperMap::mapped_type(content, total);
			p++;
		}
	}
};



template<class XianDuan_or_Bi>
void preDump(dumpHelperMap &helperMap) // post Order
{
	preDump<XianDuan_or_Bi::baseItemType>(helperMap);

	XianDuan_or_Bi::preDumpClass::doWork(helperMap);
}


template<class XianDuanClass >
class DumpTemplate
{
public:

	static void doWork(dumpHelperMap &helperMap, ostream &stream)
	{
		if (XianDuanClass::container == NULL) return;

		XianDuanClass::ContainerType::iterator p = XianDuanClass::container->begin();
		XianDuanClass::ContainerType::iterator end   = XianDuanClass::container->end();
		
		while (p != end)
		{
			XianDuanClass::ContainerType::value_type &item = *p;

			int total = helperMap[&item].total;
			
			stringstream strstream;
			strstream << item;

			int left = (total - 1 - strstream.str().length()) / 2 + strstream.str().length();
			int right = total - 1 - left;

			stream << '|';
			stream.width(left);
			stream << strstream.str();
			for (int i = 0; i < right; i++)
				stream << ' ';

			p++;
		}
		stream << '$' << '\n';
	}
};



template<class XianDuan_or_Bi_or_KXian>
void Dump(dumpHelperMap &helperMap, ostream &file)  // pre Order
{
	XianDuan_or_Bi_or_KXian::DumpClass::doWork(helperMap, file);

	Dump<XianDuan_or_Bi_or_KXian::baseItemType>(helperMap, file);
}



template<class XianDuanClass >
class DumpTemplateV2
{
public:

	static void printChildren(typename XianDuanClass::baseItemType *start, typename XianDuanClass::baseItemType *end, ostream &stream, dumpHelperMap &helperMap)
	{
		XianDuanClass::baseItemType *p = start;
		while (p != end)
		{
			XianDuanClass::baseItemType item = *p;

			int total = helperMap[p].total;

			stringstream strstream;
			strstream << item;

			int left = (total - 1 - strstream.str().length()) / 2 + strstream.str().length();
			int right = total - 1 - left;

			stream << '|';
			stream.width(left);
			stream << strstream.str();
			for (int i = 0; i < right; i++)
				stream << ' ';

			p++;
		}
		stream << '$' << "\n\n";
	}


	static void doWork(dumpHelperMap &helperMap, ostream &stream)
	{
		if (XianDuanClass::container == NULL) return;

		XianDuanClass::ContainerType::iterator p = XianDuanClass::container->begin();
		XianDuanClass::ContainerType::iterator end   = XianDuanClass::container->end();
		
		while (p != end)
		{
			XianDuanClass::ContainerType::value_type &item = *p;

			int total = helperMap[&item].total;
			
			stringstream strstream;
			strstream << item;

			int left = (total - 1 - strstream.str().length()) / 2 + strstream.str().length();
			int right = total - 1 - left;

			stream << '|';
			stream.width(left);
			stream << strstream.str();
			for (int i = 0; i < right; i++)
				stream << ' ';

			stream << '$' << '\n';
			printChildren(item.getStart(), item.getEnd(), stream, helperMap);

			p++;
		}
		stream << "####" << '\n';
	}
};



template<class XianDuan_or_Bi_or_KXian>
void DumpV2(dumpHelperMap &helperMap, ostream &file)  // pre Order
{
	XianDuan_or_Bi_or_KXian::DumpClassV2::doWork(helperMap, file);

	DumpV2<XianDuan_or_Bi_or_KXian::baseItemType>(helperMap, file);
}



#endif
