#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"

template <int grade>
class Class_ZhongShu: public traits<typename Class_XianDuan<grade-1>, typename Class_ZhongShu<grade>>
{
public:
	Class_ZhongShu(void);
	~Class_ZhongShu(void);

	float rangeHigh, rangeLow; // ��������䣻 High��Low��ʾ���ನ���ĸߵ͵�

/*
                            �˴��ߵ�һ��Ҫ����High
                                   /\
                                  /  \
                                 /    \
                                /      \
                               /        \    /
                  High        /          \  /
                   /\        /            \/
   rangeHigh      /  \      /     ��������[rangeLow, rangeHigh]����Ϊ����
       /\        /    \    /
      /  \      /      \  / 
     /    \    /        \/
    /      \  /     rangeLow
   /        \/
  /        Low
 /


\
 \           High
  \          /\
   \        /  \         rangeHigh
    \      /    \            /\
     \    /      \          /  \
      \  /        \        /    \
       \/          \      /      \
   rangeLow         \    /        \    ��������[rangeLow, rangeHigh]����Ϊ����
                     \  /          \          /\
                      \/            \        /  \
                     Low             \      /    \
                                      \    /
                                       \  /
                                        \/
                                  �˴��͵�һ��Ҫ����Low
*/

	/* ���������㣺 �μ������ƻ��������������������򣩣��������������䣬��ôμ������Ƶ��յ��ǵ��������㣻 ����Ҫ�󣬸ôμ������㣬Ӧ�ø������ನ���ߵ㣨���򣩻�������ನ���͵㣨������ */
	baseItemType thirdPoint;
};



template<int grade>
Class_ZhongShu<grade>::Class_ZhongShu(void)
{
}

template<int grade>
Class_ZhongShu<grade>::~Class_ZhongShu(void)
{
}



#endif