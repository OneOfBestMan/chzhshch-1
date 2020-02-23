#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"


class IZhongShu
{
public:
	virtual int getGrade() = 0;
};


template <int grade>
class Class_ZhongShu: public traits<typename Class_ZhongShu<grade-1>, typename Class_ZhongShu<grade>>
{
public:

	static const int GRADE = grade;

	Class_ZhongShu(void);
	~Class_ZhongShu(void);

	float rangeHigh, rangeLow; // 中枢的区间； High和Low表示中枢波动的高低点

/*
                            此处高点一定要高于High
                                   /\
                                  /  \
                                 /    \
                                /      \
                               /        \    /
                  High        /          \  /
                   /\        /            \/
   rangeHigh      /  \      /     高于区间[rangeLow, rangeHigh]，成为三买
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
   rangeLow         \    /        \    低于区间[rangeLow, rangeHigh]，成为三卖
                     \  /          \          /\
                      \/            \        /  \
                     Low             \      /    \
                                      \    /
                                       \  /
                                        \/
                                  此处低点一定要低于Low
*/

	/* 第三买、卖点： 次级别走势回拉（三卖）或回跌（三买），不触及中枢区间，则该次级别走势的终点是第三买卖点； 并且要求，该次级别的起点，应该高于中枢波动高点（三买）或低于中枢波动低点（三卖） */

	/*
	getStart() 返回中枢起始的那一笔
	getEnd() 返回中枢回拉、回踩的那一笔，第三买、卖点就是这一笔的末端点；
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