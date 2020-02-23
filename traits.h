#ifndef TRAITS_H
#define TRAITS_H

#include <vector>
#include <assert.h>
using namespace std;


typedef enum Direction
{
	UNKNOWN = -1, // ����K�ߣ���û�з���ģ�
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3,
	OVERLAPPING = ENCLOSING, // ���ֻ����������
}  Direction;

Direction operator-(const Direction& d);


class Class_KXian;

template<class baseItemType, class Item>
class traits
{ 
public:
	typedef vector<Item> ContainerType;
	typedef baseItemType baseItemType;
	typedef vector<baseItemType> baseItemType_Container;

	float High, Low;
	Direction d;
	baseItemType *Start, *End;


	traits(baseItemType* start, baseItemType* end, float h, float l, Direction dir = UNKNOWN) {Start = start; End = end; High =h; Low = l; d = dir;}
	traits() {Start = End = (baseItemType*)NULL; High = Low = 0; d = UNKNOWN;}

	float getHigh() const {return High;}
	float getLow() const {return Low;}
	baseItemType*  getStart() const{return Start;}
	baseItemType*  getEnd() const {return End;}
	Direction getDirection() const {return d;}

	Class_KXian* getStartRec() { return Start->getStartRec(); }
	Class_KXian* getEndRec() { return End->getEndRec(); }

	bool operator<(const Item &latter) const
	{
		/* ǰ�� �� ���� ����������̬ */
		return (getHigh() < latter.getHigh()  && getLow() < latter.getLow());
	}

	bool operator>(const Item &latter) const
	{
		/* ǰ�� �� ���� �����µ���̬ */
		return (getHigh() > latter.getHigh()  && getLow() > latter.getLow());
	}

	static Direction getDirection(const Item &former, const Item &latter)
	{
		if (former < latter)
			return ASCENDING;
		else if (former > latter)
			return DESCENDING;
		else
			return ENCLOSING;
	}

	bool operator>> (const Item &latter) const
	{
		/* ǰ���� */
		return getHigh() >= latter.getHigh()  && getLow() <= latter.getLow();
	}

	bool operator<< (const Item &latter) const
	{
		/* ���ǰ */
		return getHigh() <= latter.getHigh()  && getLow() >= latter.getLow();
	}
	
	bool operator==(const Item &latter) const
	{
		/* ���� */
		return  (*this >> latter) || (*this << latter);
	}

	traits&  merge(const Item &latter, Direction d) // �ϲ��������а�����ϵ��K��
	{
		assert(*this == latter);

		switch (d)
		{
		case ASCENDING:
			this->High = max(this->getHigh(), latter.getHigh());
			this->Low = max(this->getLow(), latter.getLow());
			break;
		case DESCENDING:
			this->High = min(this->getHigh(), latter.getHigh());
			this->Low = min(this->getLow(), latter.getLow());
			break;
		default:
			assert(0);
		}
		return *this;
	}

	bool intersect(const Item &latter)
	{
		return (this->getHigh() >= latter.getLow()) || (this->getLow() < = latter.getHigh());
	}
};



/* ��������ӿڣ�����Ϊ�߶Σ����ܰ������ɸ���ͬ��������࣬��Щ������Ҫ���õ�һ���б��У���ˣ�����б��Ǹ����ʵ��б���ˣ���Ҫά�������ָ�룬�Ա㴦����Щ���ʵ����ࡣ */
template<class veryLowXianDuanType>
class IZhongShu
{
public:
	virtual int getGrade() = 0;

	/* ʱ���� */
	virtual bool preceed(const IZhongShu &item) const = 0;
	virtual bool follow(const IZhongShu &item) const = 0;
	/* �ռ��ϣ�����������̬ */
	virtual bool operator<(const IZhongShu &item) const = 0;
	/* �ռ��ϣ������µ���̬ */
	virtual bool operator>(const IZhongShu &item) const = 0;
	virtual bool operator==(const IZhongShu &latter) const = 0;

	virtual bool operator>> (const IZhongShu &latter) const = 0;
	virtual bool operator<< (const IZhongShu &latter) const = 0;

	virtual float getHigh() const = 0;
	virtual float getLow() const = 0;
	
	virtual Class_KXian* getStartRec() = 0;
	virtual Class_KXian* getEndRec() = 0;

	virtual veryLowXianDuanType* getEnd() = 0;
	virtual veryLowXianDuanType* getStart() = 0;
};



template<class subZhongShuType, class XianDuanType, class veryLowXianDuanType, class ZhongShuType>
class traits_ZhongShu: public IZhongShu<veryLowXianDuanType>
{ 
public:
	typedef vector<ZhongShuType> ContainerType;
	typedef subZhongShuType subZhongShuType;

	typedef typename XianDuanType::baseItemType subXianDuanType;

	typedef veryLowXianDuanType veryBaseXianDuanType;

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

	float rangeHigh, rangeLow; // ��������䣻 
	float High, Low;  // High��Low��ʾ���ನ���ĸߵ͵�
	veryBaseXianDuanType *start;
	veryBaseXianDuanType *end;

	//Direction d;


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
				XianDuanType *XianDuan;
			} c2;
			struct{
				// ����9��(��3^x, x>=2��)�μ����߶Σ����ɸ��߼������������
				subXianDuanType *start;
				subXianDuanType *last;
			} c3;
		};
		int type;
	} content;


	traits_ZhongShu(subZhongShuType *f, subZhongShuType *m, subZhongShuType *l)  {content.type = 1; content.c1.first = f; content.c1.mid = m; content.c1.last = l;}

	traits_ZhongShu(subZhongShuType *f, XianDuanType *x) {content.type = 2; content.c2.first = f; content.c2.XianDuan = x;}

	traits_ZhongShu(subXianDuanType *s, subXianDuanType *l) { assert(l-s >= 8); content.type = 3; content.c3.start =s; content.c3.last = l;}


	float getHigh() const {return High;}
	float getLow() const {return Low;}


	veryBaseXianDuanType*  getStart() const{return Start;}
	veryBaseXianDuanType*  getEnd() const {return End;}
	
	/*Direction getDirection() const {return d;} */

	Class_KXian* getStartRec() { return Start->getStartRec(); }
	Class_KXian* getEndRec() { return End->getEndRec(); }


	bool preceed(const IZhongShu &latter) const
	{
		return getEndRec() > latter.getStartRec();
	}

	bool follow(const IZhongShu &former) const 
	{
		return latter.getEndRec() > getStartRec();
	}

	bool operator<(const IZhongShu &latter) const
	{
		/* ǰ�� �� ���� ����������̬ */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(const IZhongShu &latter) const
	{
		/* ǰ�� �� ���� �����µ���̬ */
		return preceed(latter)  && getLow() > latter.getHigh();
	}

	static Direction getDirection(const IZhongShu &former, const IZhongShu &latter)
	{
		assert(former.preceed(latter));

		if (former < latter)
			return ASCENDING;
		else if (former > latter)
			return DESCENDING;
		else
			return OVERLAPPING;
	}

	bool operator>> (const IZhongShu &latter) const
	{
		/* ǰ�� ���� ���ߣ��������ص� */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (const IZhongShu &latter) const
	{
		/* ǰ�� ���� ���ߣ� �������ص� */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(const IZhongShu &latter) const
	{
		/* ���� */
		return  (*this >> latter) || (*this << latter);
	}

	/*traits&  merge(const Item &latter, Direction d) // �ϲ��������а�����ϵ��K��
	{
		assert(*this == latter);

		switch (d)
		{
		case ASCENDING:
			this->High = max(this->getHigh(), latter.getHigh());
			this->Low = max(this->getLow(), latter.getLow());
			break;
		case DESCENDING:
			this->High = min(this->getHigh(), latter.getHigh());
			this->Low = min(this->getLow(), latter.getLow());
			break;
		default:
			assert(0);
		}
		return *this;
	}

	bool intersect(const Item &latter)
	{
		return (this->getHigh() >= latter.getLow()) || (this->getLow() < = latter.getHigh());
	}*/

};


#endif