#ifndef TRAITS_H
#define TRAITS_H

#include <vector>
using namespace std;


typedef enum Direction
{
	UNKNOWN = -1,
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3
}  Direction;

Direction operator-(const Direction& d);


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

};

#endif