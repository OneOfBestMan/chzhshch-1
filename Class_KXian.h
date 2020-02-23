#pragma once

#ifndef CLASS_KXIAN_H
#define CLASS_KXIAN_H

#include <vector>
using namespace std;

class Class_KXian
{
public:

	typedef struct
	{
		float kStart;
		float kEnd;
		float kBot;
		float kHigh;
	} valueType;

	typedef vector<Class_KXian> ContainerType;

	static ContainerType *container;

	Class_KXian() {}
	Class_KXian(float kStart, float kEnd, float kBot, float kHigh);
	~Class_KXian(void);

	float getStart();
	float getEnd();
	float getHigh();
	float getLow();
private:
	valueType bar;
};


#endif