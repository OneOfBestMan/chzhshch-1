#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"






template <int grade>
class Class_ZhongShu: protected traits_ZhongShu<typename Class_ZhongShu<grade-1>, typename Class_XianDuan<grade>, typename Class_ZhongShu<grade>>
{
public:

	static const int GRADE = grade;

	Class_ZhongShu(void);
	~Class_ZhongShu(void);



	/* ���������㣺 �μ������ƻ�������������ص������򣩣��������������䣬��ôμ������Ƶ��յ��ǵ��������㣻 ����Ҫ�󣬸ôμ������㣬Ӧ�ø������ನ���ߵ㣨���򣩻�������ನ���͵㣨������ */

	/*
	getStart() ����������ʼ����һ��
	getEnd() ��������������زȵ���һ�ʣ����������������һ�ʵ�ĩ�˵㣻
	*/
	


};



template<int grade>
Class_ZhongShu<grade>::Class_ZhongShu(void)
{
}

template<int grade>
Class_ZhongShu<grade>::~Class_ZhongShu(void)
{
}


template<>
class Class_ZhongShu<1>: public traits<typename Class_XianDuan<1>, typename Class_ZhongShu<1>>
{
};




#endif