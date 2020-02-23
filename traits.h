#ifndef TRAITS_H
#define TRAITS_H

#include <vector>
#include <assert.h>
using namespace std;


typedef enum Direction
{
	UNKNOWN = -1, // 对于K线，是没有方向的；
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3,
	OVERLAPPING = ENCLOSING, // 这个只适用于中枢
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
		/* 前者 与 后者 构成上升形态 */
		return (getHigh() < latter.getHigh()  && getLow() < latter.getLow());
	}

	bool operator>(const Item &latter) const
	{
		/* 前者 与 后者 构成下跌形态 */
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
		/* 前包后 */
		return getHigh() >= latter.getHigh()  && getLow() <= latter.getLow();
	}

	bool operator<< (const Item &latter) const
	{
		/* 后包前 */
		return getHigh() <= latter.getHigh()  && getLow() >= latter.getLow();
	}
	
	bool operator==(const Item &latter) const
	{
		/* 包含 */
		return  (*this >> latter) || (*this << latter);
	}

	traits&  merge(const Item &latter, Direction d) // 合并两条具有包含关系的K线
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



/* 设立这个接口，是因为线段，可能包含若干个不同级别的中枢，这些中枢需要放置到一个列表中，因此，这个列表是个异质的列表，因此，需要维护虚类的指针，以便处理这些异质的中枢。 */
template<class veryLowXianDuanType>
class IZhongShu
{
public:
	virtual int getGrade() = 0;

	/* 时间上 */
	virtual bool preceed(const IZhongShu &item) const = 0;
	virtual bool follow(const IZhongShu &item) const = 0;
	/* 空间上，构成上升形态 */
	virtual bool operator<(const IZhongShu &item) const = 0;
	/* 空间上，构成下跌形态 */
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

	float rangeHigh, rangeLow; // 中枢的区间； 
	float High, Low;  // High和Low表示中枢波动的高低点
	veryBaseXianDuanType *start;
	veryBaseXianDuanType *end;

	//Direction d;


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
				XianDuanType *XianDuan;
			} c2;
			struct{
				// 处理，9个(或3^x, x>=2个)次级别线段，构成更高级别中枢的情形
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
		/* 前者 与 后者 构成上升形态 */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(const IZhongShu &latter) const
	{
		/* 前者 与 后者 构成下跌形态 */
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
		/* 前者 高于 后者，但是有重叠 */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (const IZhongShu &latter) const
	{
		/* 前者 低于 后者， 但是有重叠 */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(const IZhongShu &latter) const
	{
		/* 包含 */
		return  (*this >> latter) || (*this << latter);
	}

	/*traits&  merge(const Item &latter, Direction d) // 合并两条具有包含关系的K线
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