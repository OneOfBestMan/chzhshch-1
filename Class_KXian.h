#pragma once

#ifndef CLASS_KXIAN_H
#define CLASS_KXIAN_H

#include <vector>
using namespace std;


typedef enum {ASCENDING =1, DESCENDING = 2, ENCLOSING = 3}  Direction;


class Class_KXian
{
public:

	typedef vector<Class_KXian> ContainerType;

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

	static void initialize();
private:
	valueType bar;
};


#endif