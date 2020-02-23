#pragma once
#ifndef CLASS_DRIVER_H
#define CLASS_DRIVER_H

#include "Base.h"

#include <stack>

using namespace std;


class Class_Driver
{
public:
	static int const totalGrade = 7;
	Class_Driver();
	Class_Driver(Class_env *in);
	~Class_Driver(void);


	void process();
private:
    Class_env *env;

	stack<Class_Bi*>  pending[totalGrade];
};

#endif