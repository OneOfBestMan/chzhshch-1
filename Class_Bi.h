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

	typedef struct
	{
		int barIndex, int cont; // 这个结构用于处理具有包含关系的k线； cont表示后面有几根k线，和barIndex开始的k线存在包含关系
	} singleBar;

	stack<singleBar*> contains;

	Class_FenXing start;
	Class_FenXing end;
	
};

#endif