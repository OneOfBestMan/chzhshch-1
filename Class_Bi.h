#pragma once

#include <stack>

#ifndef CLASS_BI_H

#define CLASS_BI_H

class Class_Bi
{
public:
	Class_Bi(void);
	~Class_Bi(void);

	void accept(int bar);
private:
    stack<int> contains;

	typedef struct
	{
		int barHigh, int barLow;
	} barState

	Class_FenXing start;
	Class_FenXing end;
	
};

#endif