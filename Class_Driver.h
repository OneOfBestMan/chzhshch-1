#pragma once

#include "Base.h"

#include <stack>

using namespace std;

#ifndef CLASS_DRIVER_H
#define CLASS_DRIVER_H

class Class_Driver
{
public:
	static int const totalGrade = 7;
	Class_Driver();
	~Class_Driver(void);


	void process();
private:
	stack<Class_Bi*>  pending[totalGrade];
};

#endif