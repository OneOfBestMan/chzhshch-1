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
			
#ifdef BiLiKuanDu //按比例宽度输出
			int total = 0;
			while (xStart != xEnd + 1)
			{
				total += helperMap[xStart].total;
				xStart++;
			}
			
			stringstream strstream;
			strstream << item;

			int content = strstream.str().length();
			helperMap[&item] = dumpHelperMap::mapped_type(content, total);
#else
			stringstream strstream;
			strstream << item;

			int content = strstream.str().length();
			helperMap[&item] = dumpHelperMap::mapped_type(content, content);
#endif
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
	/* 大致思路是：
	（1）从高级线段、到低级线段，逐层打印； 在打印高层线段时，其包含的低层线段，也被打印出来；
	（2）在完成(1)之后，低层次线段队列可能依然有线段没有打印出来，这是由于这些低层线段，
	其对应的高层线段没有建立，即，低级别线段尚未完成，所以高级别线段也没有建立。 因此，需要把这些低层级线段，逐层打印出来。
	（3）实现的时候，按照自低层向高层，逐层求取 输出所占的 宽度
	*/
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
		while (p != end + 1)
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
