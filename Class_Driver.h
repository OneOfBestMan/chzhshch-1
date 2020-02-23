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
private:
    Class_env *env;
};

#endif