#pragma once

#ifndef CLASS_ZHONGSHU_H
#define CLASS_ZHONGSHU_H

#include "traits.h"
#include "Class_XianDuan.h"

#include <list>
using namespace std;


class ValueRange
{
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



class Class_ZhongShu
{

private:

	typedef Class_XianDuan<1> veryBaseXianDuanType;
	typedef Class_ZhongShu subZhongShuType;
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
	int grade;

	ValueRange coreRange;  // rangeHigh, rangeLow; ��������䣻
	ValueRange floatRange; // High��Low��ʾ���ನ���ĸߵ͵�

	veryBaseXianDuanType *Start;
	veryBaseXianDuanType *End;
	Direction d; // ����ķ����������߶η����෴�����������µ��߶��У����෽�������ϵġ�

	/* ���������㣺 �μ������ƻ�������������ص������򣩣��������������䣬��ôμ������Ƶ��յ��ǵ��������㣻 ����Ҫ�󣬸ôμ������㣬Ӧ�ø������ನ���ߵ㣨���򣩻�������ನ���͵㣨������ */
	Class_ZhongShu* thirdPoint; // �������༶��0���߶μ���1�������������Ч�ģ��������༶��0�����������㣬�������Լ������������߼������࣬thirdPoint��һ���ͼ������࣬���磺����3����ĵ��������㣬�Ǹ�����2���ࣻ

	/*

	�������༶�� һ��˼����

	����1�߶� == ���༶��0
	����2�߶� == ���༶��1
	����3�߶� == ���༶��2
	�Դ�����

	�ڴ�����3�߶ε�ʱ�������߶��Ǽ���2�߶Σ�������Щ����2�߶ε�TP/JP֮�󣬵õ��������б���Сֻ�������༶��1���߶�2���������б�����Ȼ��������0����ô��Ҫ�����Ǻ��ԡ�
	*/
	struct {
		union
		{
			struct{
				// ����juxtaposition������
				subZhongShuType *first;
				subZhongShuType *mid;
				subZhongShuType *last;
			} c1;
			struct{
				// ����turning point������
				subZhongShuType *first;
				Class_XianDuanBase *XianDuan;
			} c2;
			struct{
				// ����9��(��3^x, x>=2��)�μ����߶Σ����ɸ��߼������������
				Class_XianDuanBase *start;
				Class_XianDuanBase *last;
			} c3;
			struct
			{
				// �����߶εĵͼ����߶Σ�δ�ܹ��ɸü�����������Σ���ʱ�����߶ξͱ������ü������ࣻ
				Class_XianDuanBase *xianDuan;
			}c4;
			struct
			{
				// �����������࣬ ������չ��ͨ����ֻ�����ڽϵͼ�������ࣻ�߼���������չ������3��ͬ���������ཻ��������չ��
				subZhongShuType *former;
				subZhongShuType *latter;
			}c5;
		};
		int type;
	} content;


	void initializeCommon()
	{
		thirdPoint = NULL;
		Start = End = NULL;

		switch(content.type)
		{

		case 1:
			coreRange = content.c1.first->getCoreRange();
			coreRange = coreRange && content.c1.mid->getCoreRange() && content.c1.last->getCoreRange();
			floatRange = content.c1.first->getFloatRange();
			floatRange = floatRange && content.c1.mid->getFloatRange() && content.c1.last->getFloatRange();
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
				while (curr <= end)
				{
					coreRange && ValueRange(curr->getLow(), curr->getHigh());
					floatRange || ValueRange(curr->getLow(), curr->getHigh());
					curr++;
				}

			}
			break;
		case 4:
			coreRange = ValueRange(content.c4.xianDuan->getLow(), content.c4.xianDuan->getHigh());
			floatRange = coreRange;
			break;
		case 5:
			coreRange = content.c5.former->getCoreRange();
			coreRange = coreRange && content.c5.latter->getCoreRange();
			floatRange = content.c5.former->getFloatRange();
			floatRange = floatRange && content.c5.latter->getFloatRange();
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
			End = content.c1.first->getEnd();
			break;
		case 2:
			End = content.c2.first->getEnd();
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
		/* ǰ�� �� ���� ����������̬ */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(Class_ZhongShu &latter)
	{
		/* ǰ�� �� ���� �����µ���̬ */
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
		/* ǰ�� ���� ���ߣ��������ص� */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (Class_ZhongShu &latter)
	{
		/* ǰ�� ���� ���ߣ� �������ص� */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(Class_ZhongShu &latter)
	{
		/* ���� */
		return  (*this >> latter) || (*this << latter);
	}

	ValueRange getCoreRange() const {return coreRange;}
	ValueRange getFloatRange() const {return floatRange;}

	bool intersect(const Class_ZhongShu &latter)
	{
		ValueRange temp = latter.getFloatRange();
		temp && getFloatRange();
		return !temp.isNulValueRange();
	}

	/* ���������㣺 �μ������ƻ�������������ص������򣩣��������������䣬��ôμ������Ƶ��յ��ǵ��������㣻 ����Ҫ�󣬸ôμ������㣬Ӧ�ø������ನ���ߵ㣨���򣩻�������ನ���͵㣨������ */

	/*
	getStart() ����������ʼ����һ��
	getEnd() ��������������زȵ���һ�ʣ����������������һ�ʵ�ĩ�˵㣻
	*/

	friend Class_ZhongShu* createZhongShu(Class_XianDuanBase *xianDuan,int); // type = 4
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu*, Class_XianDuanBase*,int); // type = 2
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu*, Class_ZhongShu*, int); // type = 5
	friend Class_ZhongShu* createZhongShu(Class_ZhongShu *, Class_ZhongShu *, Class_ZhongShu * , int);  // type = 1 
	friend Class_ZhongShu* createZhongShu(Class_XianDuanBase *, Class_XianDuanBase *, int ); // type = 3

	static const int MAX_LEVEL = 7;
	static list<Class_ZhongShu>  zsList[MAX_LEVEL];
};




/* ����0����ͬ�ڼ���1�߶Σ� ������Ӧ��TurningPoint/JustaPosition�㷨��ʱ�򣬶��ڼ���2�߶Σ����԰�����ÿ�����߶Σ�������1������������0���ࡣ����������ϲ��ķ�������Ϳ��Լ򵥽���ģ�廯�����������������༶��1������3���߶�1���ɵģ������߼������࣬����3���ͼ������๹�ɡ� */



#endif