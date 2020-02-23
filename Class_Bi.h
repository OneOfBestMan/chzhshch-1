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
		int barIndex, int cont; // ����ṹ���ڴ�����а�����ϵ��k�ߣ� cont��ʾ�����м���k�ߣ���barIndex��ʼ��k�ߴ��ڰ�����ϵ
	} singleBar;

	stack<singleBar*> contains;

	Class_FenXing start;
	Class_FenXing end;
	
};

#endif