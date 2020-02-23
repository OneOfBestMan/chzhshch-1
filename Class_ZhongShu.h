#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"

#include <list>
using namespace std;



class ValueRange
{
friend class Class_ZhongShu;
private:
	float low, high;
public:
	ValueRange(float v1, float v2) {low = min(v1,v2); high = max(v1,v2);}
	ValueRange() {low = high = 0;}

	float getHigh() const {return high;}
	float getLow() const {return low;}

	ValueRange& operator &&(const ValueRange &item)
	{
		high = min(high, item.high);
		low = max(low, item.low);
		if (high < low)
			*this = NullValueRange;
		return *this;
	}

	ValueRange& operator ||(const ValueRange &item)
	{
		high = max(high, item.high);
		low = min(low, item.low);
		return *this;
	}

	bool isNulValueRange() const
	{
		return high == NullValueRange.high && low == NullValueRange.low;
	}

	static const ValueRange NullValueRange;
};


class DisplayZhongShu;

class Class_ZhongShu
{

private:

	typedef Class_XianDuan<1> veryBaseXianDuanType;
	typedef Class_ZhongShu subZhongShuType;
	
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
	int grade;

	ValueRange coreRange;  // rangeHigh, rangeLow; 中枢的区间；
	ValueRange floatRange; // High和Low表示中枢波动的高低点

	veryBaseXianDuanType *Start;
	veryBaseXianDuanType *End;
	Direction d; // 中枢的方向，与所在线段方向相反：即，在向下的线段中，中枢方向是向上的。

	/* 第三买、卖点： 次级别走势回拉（三卖）或回跌（三买），不触及中枢区间，则该次级别走势的终点是第三买卖点； 并且要求，该次级别的起点，应该高于中枢波动高点（三买）或低于中枢波动低点（三卖） */
	Class_ZhongShu* thirdPoint; // 对于中枢级别0（线段级别1），这个域是无效的，对于中枢级别0，第三买卖点，就是它自己；对于其它高级别中枢，thirdPoint是一个低级别中枢，比如：级别3中枢的第三买卖点，是个级别2中枢；

	/*

	关于中枢级别， 一点思考：

	级别1线段 == 中枢级别0
	级别2线段 == 中枢级别1
	级别3线段 == 中枢级别2
	以此类推

	在处理级别3线段的时候，其子线段是级别2线段，处理这些级别2线段的TP/JP之后，得到的中枢列表，最小只看到中枢级别1、线段2，如果这个列表中依然存在中枢0，那么需要将它们忽略。
	*/
	struct {
		union
		{
			struct{
				// 处理juxtaposition的情形
				subZhongShuType *first;
				subZhongShuType *mid;
				subZhongShuType *last;
			} c1;
			struct{
				// 处理turning point的情形
				subZhongShuType *first;
				Class_XianDuanBase *XianDuan;
			} c2;
			struct{
				// 处理，多于3个次级别中枢重叠，构成更高级别中枢的情形；
				Class_XianDuanBase *start;
				Class_XianDuanBase *last;
			} c3;
			struct
			{
				// 处理，线段的低级别线段，未能构成该级别中枢的情形；此时，此线段就被当做该级别中枢；
				Class_XianDuanBase *xianDuan;
			}c4;
			struct
			{
				// 两个相邻中枢， 发生扩展；通常，只发生在较低级别的中枢；高级别中枢拓展，都是3个同级别中枢相交，发生扩展；
				subZhongShuType *former;
				subZhongShuType *latter;
			}c5;
		};
		int type;
	} content;


	void initializeCommon()
	{
		thirdPoint = (grade == 0) ? this : NULL;
		Start = End = NULL;

		switch(content.type)
		{

		case 1:
			coreRange = content.c1.first->getFloatRange();
			coreRange = coreRange && content.c1.mid->getFloatRange() && content.c1.last->getFloatRange();
			floatRange = content.c1.first->getFloatRange();
			floatRange = floatRange || content.c1.mid->getFloatRange() || content.c1.last->getFloatRange();
			break;
		case 2:
			{
				coreRange = content.c2.first->getCoreRange();
				floatRange = content.c2.first->getFloatRange();
				veryBaseXianDuanType* curr = content.c2.first->getEnd() + 1;
				veryBaseXianDuanType* last = content.c2.XianDuan->getBaseXianDuanEnd();

				ValueRange temp;
				while (curr <= last)
				{
					temp = temp || ValueRange(curr->getLow(), curr->getHigh());
					curr++;
				}
				coreRange = coreRange && temp;
				floatRange = floatRange || temp;
			}
			break;
		case 3:
			{
				veryBaseXianDuanType* curr = content.c3.start->getBaseXianDuanStart();
				veryBaseXianDuanType* end = content.c3.last->getBaseXianDuanStart();
				// coreRange和 floatRange需要单独设置，见 createZhongShu(Class_ZhongShu *zsArray[], int cntZS,  int grade)
			}
			break;
		case 4:
			coreRange = ValueRange(content.c4.xianDuan->getLow(), content.c4.xianDuan->getHigh());
			floatRange = coreRange;
			break;
		case 5:
			coreRange = content.c5.former->getFloatRange();
			coreRange = coreRange && content.c5.latter->getFloatRange();
			floatRange = content.c5.former->getFloatRange();
			floatRange = floatRange || content.c5.latter->getFloatRange();
			break;
		default:
			assert(0);
		}
	}

	
	Class_ZhongShu(subZhongShuType *f, subZhongShuType *m, subZhongShuType *l, int grad) 
	{
		grade = grad;
		content.type = 1; content.c1.first = f; content.c1.mid = m; content.c1.last = l;
		initializeCommon();
	}

	Class_ZhongShu(subZhongShuType *f, Class_XianDuanBase *x, int grad) 
	{
		grade = grad; 
		content.type = 2; content.c2.first = f; content.c2.XianDuan = x;
		initializeCommon();
	}

	Class_ZhongShu(Class_XianDuanBase *s, Class_XianDuanBase *l, int grad) 
	{ 
		//assert(l-s >= 8);
		grade = grad; content.type = 3; content.c3.start =s; content.c3.last = l;
		initializeCommon();
	}

	Class_ZhongShu(Class_XianDuanBase *x, int grad) 
	{ 
		assert(x->zsList == NULL); 
		grade = grad; content.type = 4; content.c4.xianDuan = x;
		initializeCommon();
	}

	Class_ZhongShu(subZhongShuType *f, subZhongShuType *l, int grad) 
	{
		grade = grad; 
		content.type = 5; content.c5.former = f; content.c5.latter = l;
		initializeCommon();
	}

public:	
	typedef DisplayZhongShu DisplayClass;

	Class_ZhongShu(void) {}
	~Class_ZhongShu(void) {}

	veryBaseXianDuanType*  getStart() 
	{
		if (Start) return Start;

		switch (content.type)
		{
		case 1:
			Start = content.c1.first->getStart();
			break;
		case 2:
			Start = content.c2.first->getStart();
			break;
		case 3:
			Start = content.c3.start->getBaseXianDuanStart();
			break;
		case 4:
			Start = content.c4.xianDuan->getBaseXianDuanStart();
			break;
		case 5:
			Start = content.c5.former->getStart();
			break;
		default:
			assert(0);
		}

		return Start;
	}
	
	veryBaseXianDuanType*  getEnd() 
	{
		if (End) return End;

		switch (content.type)
		{
		case 1:
			End = content.c1.last->getEnd();
			break;
		case 2:
			End = content.c2.XianDuan->getBaseXianDuanEnd();
			break;
		case 3:
			End = content.c3.last->getBaseXianDuanEnd();
			break;
		case 4:
			End = content.c4.xianDuan->getBaseXianDuanEnd();
			break;
		case 5:
			End = content.c5.latter->getEnd();
			break;
		default:
			assert(0);
		}

		return End;
	}


	int getGrade() {return grade;}

	float getHigh() const {return floatRange.getHigh();}
	float getLow() const {return floatRange.getLow();}

	Class_ZhongShu* getThirdPoint() {return thirdPoint;}
	void setThirdPoint(Class_ZhongShu* thrdPoint)
	{
		assert(thirdPoint == NULL);
		thirdPoint = thrdPoint;
		
		// 在设置第三买卖点后，更新该中枢的floatRange
		veryBaseXianDuanType *curr = getEnd() + 1;
		veryBaseXianDuanType *end = thirdPoint->getStart();

		ValueRange tmpRange = ValueRange(curr->getLow(), curr->getHigh());
		while (curr != end)
		{
			tmpRange || ValueRange(curr->getLow(), curr->getHigh());
			curr++;
		}


		/* 这么更新floatRange的用意，在于，在两条线段转折的地方，增加中枢扩展的机会，比如，下图中，第2个“中枢1”，位于两个线段的转折处：
\
 \      /\
  \    /  \      
   \  0    \      /\
    \/      \    0  \ 
	         \  /    \                                /\<---
              \/      \   第三卖点                   /  \
       |<--中枢1-->|   \      /\                    /    \
                        \    0  \          /\     ATT     \
                         \  /    \        /  \    /        \
                          \/      \      /    0  /          \
                                   \    /      \/            \
                                    \  /                      \        
                                     \/                        \      /\
                          |<------ 中枢1 ----->|                \    0  \
                                                                 \  /    \
                                                                  \/      \
                                                              第三卖点     \

	   前后两个级别1的中枢，他们的floatRange本来没有交集，但是画“<---”的地方，是高过第一个中枢1的floagRange低点的。下面的处理，可以把"ATT"作为第2个中枢1的次级别波动，合并到第2个中枢的floatRange中；这样，两个中枢1的
	   floatRange就会产生交集，并且有可能会扩充成为更高级别的中枢；
		*/
		if (thrdPoint->getFloatRange().getLow() > getCoreRange().getHigh())
		{
			// 第三买卖点，在中枢上方，扩展中枢floatRange，从中枢end到第三买卖点，使其包含 低于 原来floatRange.low的部分
			if (tmpRange.getLow() < floatRange.getLow())
				floatRange.low = tmpRange.getLow();

		} else if (thrdPoint->getFloatRange().getHigh() < getCoreRange().getLow())
		{
			// 第三买卖点，在中枢下方，扩展中枢floatRange，从中枢end到第三买卖点，使其包含 高于 原来floatRange.high的部分
			if (tmpRange.getHigh() > floatRange.getHigh())
				floatRange.high = tmpRange.getHigh();
		}
		else
			assert(0);
	}
	
	Direction getDirection() const {return d;} 

	Class_KXian* getStartRec()
	{
		if (getStart())
			return Start->getStartRec();
		else
			return NULL;
	}

	Class_KXian* getEndRec()
	{
		if (getEnd())
			return End->getEndRec();
		else
			return NULL;
	}


	bool preceed(Class_ZhongShu &latter)
	{
		return getEndRec() > latter.getStartRec();
	}

	bool follow(Class_ZhongShu &former) 
	{
		return former.getEndRec() > getStartRec();
	}

	bool operator<(Class_ZhongShu &latter)
	{
		/* 前者 与 后者 构成上升形态 */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(Class_ZhongShu &latter)
	{
		/* 前者 与 后者 构成下跌形态 */
		return preceed(latter)  && getLow() > latter.getHigh();
	}

	static Direction getDirection(Class_ZhongShu &former, Class_ZhongShu &latter)
	{
		assert(former.preceed(latter));

		if (former < latter)
			return ASCENDING;
		else if (former > latter)
			return DESCENDING;
		else
			return OVERLAPPING;
	}

	bool operator>> (Class_ZhongShu &latter)
	{
		/* 前者 高于 后者，但是有重叠 */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (Class_ZhongShu &latter)
	{
		/* 前者 低于 后者， 但是有重叠 */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(Class_ZhongShu &latter)
	{
		/* 包含 */
		return  (*this >> latter) || (*this << latter);
	}

	ValueRange getCoreRange() const {return coreRange;}
	ValueRange getFloatRange() const {return floatRange;}
	void setCoreRange(ValueRange &val) {coreRange = val;}
	void setFloatRange(ValueRange &val) {floatRange = val;}

	bool intersect(const Class_ZhongShu &latter)
	{
		ValueRange temp = latter.getFloatRange();
		temp && getFloatRange();
		return !temp.isNulValueRange();
	}

	/* 第三买、卖点： 次级别走势回拉（三卖）或回跌（三买），不触及中枢区间，则该次级别走势的终点是第三买卖点； 并且要求，该次级别的起点，应该高于中枢波动高点（三买）或低于中枢波动低点（三卖） */

	/*
	getStart() 返回中枢起始的那一笔
	getEnd() 返回中枢回拉、回踩的那一笔，第三买、卖点就是这一笔的末端点；
	*/

	friend Class_ZhongShu* createZhongShu(Class_XianDuanBase *xianDuan,int); // type = 4
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu*, Class_XianDuanBase*,int); // type = 2
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu*, Class_ZhongShu*, int); // type = 5
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu *, Class_ZhongShu *, Class_ZhongShu * , int);  // type = 1 
	friend Class_ZhongShu* createZhongShu(Class_XianDuanBase *, Class_XianDuanBase *, int ); // type = 3
	//friend Class_ZhongShu* createZhongShu(int grade, ...);
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu *zsArray[], int cntZS,  int grade);

	static const int MAX_LEVEL = 8;
	static list<Class_ZhongShu>  zsList[MAX_LEVEL];
};


class DisplayZhongShu
{
public:
	static void doWork(int grade, int timeOrLowOrHigh, float *resultBuf)
	{
		/*
		显示中枢成如下的梯形：

floatRange.high
        |\
        | \
        |  \
        |   \
        |    \coreRange.high
        |    |
        |  d |
        |    |
        |    /coreRange.low
        |   /
        |  /
        | /
        |/
floatRange.low

		*/


		assert(grade < Class_ZhongShu::MAX_LEVEL);

		Class_KXian *veryStart = &(*Class_KXian::container->begin());

		list<Class_ZhongShu>::iterator curr = Class_ZhongShu::zsList[grade].begin();
		list<Class_ZhongShu>::iterator end = Class_ZhongShu::zsList[grade].end();
		while (curr != end)
		{
			Class_ZhongShu &zs = *curr;

			switch (timeOrLowOrHigh)
			{
			case 0: // 中枢开始、结束的标记
				resultBuf[zs.getStartRec() - veryStart] = 1; // 开始位置 mark 成1
				resultBuf[zs.getStartRec() - veryStart + (zs.getEndRec() - zs.getStartRec())/2] = 1.5; //这个位置显示注释文字
				resultBuf[zs.getEndRec() - veryStart] = 2; // 结束位置 mark 成2
				break;
			case 1: // 中枢持续的K线单位的个数
				resultBuf[zs.getStartRec() - veryStart] = zs.getEndRec() - zs.getStartRec();
				break;
			case 2: // low price
				resultBuf[zs.getStartRec() - veryStart] = zs.getFloatRange().getLow();
				resultBuf[zs.getStartRec() - veryStart + (zs.getEndRec() - zs.getStartRec())/2] = zs.getFloatRange().getLow(); //这个位置显示注释文字
				resultBuf[zs.getEndRec() - veryStart] = zs.getCoreRange().getLow();
				break;
			case 3: // high price
				resultBuf[zs.getStartRec() - veryStart] = zs.getFloatRange().getHigh();
				resultBuf[zs.getStartRec() - veryStart + (zs.getEndRec() - zs.getStartRec())/2] = zs.getFloatRange().getHigh(); //这个位置显示注释文字
				resultBuf[zs.getEndRec() - veryStart] = zs.getCoreRange().getHigh();
				break;
			default:
				assert(0);
			}
			curr++;
		}
	}
};


/* 中枢0，等同于级别1线段； 这样在应用TurningPoint/JustaPosition算法的时候，对于级别2线段，可以把它的每条子线段（级别是1），视作级别0中枢。这样，中枢合并的分析程序就可以简单进行模板化，而不必在区分中枢级别1，是由3条线段1构成的；而更高级别中枢，须由3个低级别中枢构成。 */

#endif