#pragma once

#ifndef CLASS_KXIAN_H
#define CLASS_KXIAN_H

#include <assert.h>
#include <vector>


using namespace std;


#include "Debug.h"


typedef enum Direction
{
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3
}  Direction;

Direction operator-(const Direction& d);

class Class_KXian
{
public:

	typedef vector<Class_KXian> ContainerType;

	typedef preDumpTemplate<Class_KXian>  preDumpClass;

	typedef struct
	{
		float kStart;
		float kEnd;
		float kBot;
		float kHigh;
	} valueType;

	static ContainerType *container;

	Class_KXian() {}
	Class_KXian(float kStart, float kEnd, float kBot, float kHigh);
	~Class_KXian(void);

	float getStart() const;
	float getEnd() const;
	float getHigh() const;
	float getLow() const;

	static Direction getDirection(const Class_KXian& firstKXian, const Class_KXian &secondKXian);
	static bool isAscending(const Class_KXian& firstKXian, const Class_KXian &secondKXian);
	static bool isDecending(const Class_KXian& firstKXian, const Class_KXian &secondKXian);
	static bool isEnclosing(const Class_KXian& firstKXian, const Class_KXian &secondKXian);

	bool operator>> (const Class_KXian &secondKXian) const
	{
		return isDecending(*this, secondKXian);
	}

	bool operator<< (const Class_KXian &secondKXian) const
	{
		return isAscending(*this, secondKXian);
	}

	bool operator==(const Class_KXian &secondKXian) const
	{
		return isEnclosing(*this, secondKXian);
	}

	Class_KXian& merge(const Class_KXian &secondKXian, Direction d) // 合并两条具有包含关系的K线
	{
		assert(*this == secondKXian);

		switch (d)
		{
		case ASCENDING:
			this->bar.kHigh = max(this->getHigh(), secondKXian.getHigh());
			this->bar.kBot = max(this->getLow(), secondKXian.getLow());
			break;
		case DESCENDING:
			this->bar.kHigh = min(this->getHigh(), secondKXian.getHigh());
			this->bar.kBot = min(this->getLow(), secondKXian.getLow());
			break;
		default:
			assert(0);
		}
		return *this;
	}

	static void initialize(bool release = false);
private:
	valueType bar;
};



template<>
class preDumpTemplate<typename Class_KXian>
{
public:
	static void doWork(dumpHelperMap &helperMap)
	{
		char tempSpace[20]; // 类似这样的串：(####.##, ####.##)
		if (Class_KXian::container == NULL) return;

		Class_KXian::ContainerType::iterator p = Class_KXian::container->begin();
		Class_KXian::ContainerType::iterator end   = Class_KXian::container->end();
		while (p != end)
		{
			Class_KXian::ContainerType::value_type &item = *p;
			sprintf(tempSpace, "(%4.2f, %4.2f)", item.getLow(), item.getHigh());
			int content = strlen(tempSpace);
			helperMap[&item] = mappedInfo(content, content + 1);
			p++;
		}
	}
};

template<>
void preDump<typename Class_KXian> (dumpHelperMap &helperMap);



#endif