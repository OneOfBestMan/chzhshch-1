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
template<int grade> class Class_XianDuan;
class IZhongShu;

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


	traits(baseItemType* start, baseItemType* end, float h, float l, Direction dir = UNKNOWN) {Start = start; End = end; High =h; Low = l; d = dir; zsList = NULL;}
	traits() {Start = End = (baseItemType*)NULL; High = Low = 0; d = UNKNOWN; zsList = NULL;}
	~traits() {delete zsList;}

	float getHigh() const {return High;}
	float getLow() const {return Low;}
	baseItemType*  getStart() const{return Start;}
	baseItemType*  getEnd() const {return End;}
	Direction getDirection() const {return d;}

	Class_KXian* getStartRec() { return Start->getStartRec(); }
	Class_KXian* getEndRec() { return End->getEndRec(); }

	/* 这两个接口，是给中枢服务的，中枢最小的构成单位 */
	Class_XianDuan<1>* getBaseXianDuanStart() {return getStart()->getBaseXianDuanStart();}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return getEnd()->getBaseXianDuanEnd();}

	vector<IZhongShu*> *zsList; //该线段所包含的 中枢列表
	void addZhongShu(IZhongShu* zs)
	{
		if (!zsList)
			zsList = new vector<IZhongShu*>;
		zsList->push_back(zs);
	}

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
		float high1 = getHigh();
		float high2 = latter.getHigh();

		float low1 = getLow();
		float low2 = latter.getLow();

		return (high1 >= high2 && high2 >= low1 || high1 <= high2 && high1 >= low2);
	}
};


/* 设立这个接口，是因为线段，可能包含若干个不同级别的中枢，这些中枢需要放置到一个列表中，因此，这个列表是个异质的列表，因此，需要维护虚类的指针，以便处理这些异质的中枢。 */

class IZhongShu
{
public:

	typedef Class_XianDuan<1> veryLowXianDuanType;

	virtual int getGrade() = 0;

	/* 时间上 */
	virtual bool preceed( IZhongShu &item)  = 0;
	virtual bool follow( IZhongShu &item)  = 0;
	/* 空间上，构成上升形态 */
	virtual bool operator<( IZhongShu &item)  = 0;
	/* 空间上，构成下跌形态 */
	virtual bool operator>( IZhongShu &item)  = 0;
	virtual bool operator==( IZhongShu &latter)  = 0;

	virtual bool operator>> ( IZhongShu &latter)  = 0;
	virtual bool operator<< ( IZhongShu &latter)  = 0;

	virtual float getHigh() const = 0;
	virtual float getLow()  const = 0;
	
	/* 用于输出中枢的时候使用 */
	virtual Class_KXian* getStartRec()  = 0;
	virtual Class_KXian* getEndRec() = 0;

	/* 用于比较中枢时间前后 */
	virtual veryLowXianDuanType* getEnd() = 0;
	virtual veryLowXianDuanType* getStart() = 0;
	virtual bool intersect(const IZhongShu &latter) = 0;
};



template<class subZhongShuType, class XianDuanType, class ZhongShuType>
class traits_ZhongShu: public IZhongShu
{ 
public:
	typedef vector<ZhongShuType> ContainerType;
	typedef subZhongShuType subZhongShuType;

	typedef typename XianDuanType::baseItemType subXianDuanType;

	typedef Class_XianDuan<1> veryBaseXianDuanType;

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
	veryBaseXianDuanType *Start;
	veryBaseXianDuanType *End;
	Direction d; // 中枢的方向，与所在线段方向相反：即，在向下的线段中，中枢方向是向上的。


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
				XianDuanType *XianDuan;
			} c2;
			struct{
				// 处理，9个(或3^x, x>=2个)次级别线段，构成更高级别中枢的情形
				subXianDuanType *start;
				subXianDuanType *last;
			} c3;
			struct
			{
				// 处理，线段的低级别线段，未能构成该级别中枢的情形；此时，此线段就被当做该级别中枢；
				XianDuanType *xianDuan;
			}c4;
		};
		int type;
	} content;


	traits_ZhongShu(subZhongShuType *f, subZhongShuType *m, subZhongShuType *l)  {content.type = 1; content.c1.first = f; content.c1.mid = m; content.c1.last = l;}

	traits_ZhongShu(subZhongShuType *f, XianDuanType *x) {content.type = 2; content.c2.first = f; content.c2.XianDuan = x;}

	traits_ZhongShu(subXianDuanType *s, subXianDuanType *l) { assert(l-s >= 8); content.type = 3; content.c3.start =s; content.c3.last = l;}

	traits_ZhongShu(XianDuanType *x) { assert(x->zsList == NULL); content.type = 4; content.c4.xianDuan = x;}

	traits_ZhongShu() {}

	int getGrade() {return ZhongShuType::GRADE ;}

	float getHigh() const {return High;}
	float getLow() const {return Low;}


	
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


	bool preceed(IZhongShu &latter)
	{
		return getEndRec() > latter.getStartRec();
	}

	bool follow(IZhongShu &former) 
	{
		return former.getEndRec() > getStartRec();
	}

	bool operator<(IZhongShu &latter)
	{
		/* 前者 与 后者 构成上升形态 */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(IZhongShu &latter)
	{
		/* 前者 与 后者 构成下跌形态 */
		return preceed(latter)  && getLow() > latter.getHigh();
	}

	static Direction getDirection(IZhongShu &former, IZhongShu &latter)
	{
		assert(former.preceed(latter));

		if (former < latter)
			return ASCENDING;
		else if (former > latter)
			return DESCENDING;
		else
			return OVERLAPPING;
	}

	bool operator>> (IZhongShu &latter)
	{
		/* 前者 高于 后者，但是有重叠 */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (IZhongShu &latter)
	{
		/* 前者 低于 后者， 但是有重叠 */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(IZhongShu &latter)
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
	}*/

	bool intersect(const IZhongShu &latter)
	{
		float high1 = getHigh();
		float high2 = latter.getHigh();

		float low1 = getLow();
		float low2 = latter.getLow();

		return (high1 >= high2 && high2 >= low1 || high1 <= high2 && high1 >= low2);
	}
};


#endif