#include "stdafx.h"
#include "Class_Driver.h"

Class_Driver::Class_Driver(Class_env *in): env(in)
{
}

Class_Driver::~Class_Driver(void)
{
}

void Class_Driver::process()
{
	int bar = 0;
	int total = Class_env::getInstance()->getTotal();

	while (bar++ < total)
	{
		if (pending[0].empty()) 
			pending[0].push(new Class_Bi());
		
		pending[0].top()->accept(bar);
	}
}