#pragma once

#ifndef ICOMPARABLE_H
#define ICOMPARABLE_H

typedef enum Direction
{
	UNKNOWN = -1, // 对于K线，是没有方向的；
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3,
	OVERLAPPING = ENCLOSING, // 这个只适用于中枢
}  Direction;

class IComparable
{
public:
	virtual float getHigh() const = 0;
	virtual float getLow() const = 0;
	virtual void setHigh(float) = 0;
	virtual void setLow(float) = 0;

	bool operator<(const IComparable &latter) const
	{
		/* 前者 与 后者 构成上升形态 */
		return (getHigh() < latter.getHigh()  && getLow() < latter.getLow());
	}

	bool operator>(const IComparable &latter) const
	{
		/* 前者 与 后者 构成下跌形态 */
		return (getHigh() > latter.getHigh()  && getLow() > latter.getLow());
	}

	static Direction getDirection(const IComparable &former, const IComparable &latter)
	{
		if (former < latter)
			return ASCENDING;
		else if (former > latter)
			return DESCENDING;
		else
			return ENCLOSING;
	}

	bool operator>> (const IComparable &latter) const
	{
		/* 前包后 */
		return getHigh() >= latter.getHigh()  && getLow() <= latter.getLow();
	}

	bool operator<< (const IComparable &latter) const
	{
		/* 后包前 */
		return getHigh() <= latter.getHigh()  && getLow() >= latter.getLow();
	}
	
	bool operator==(const IComparable &latter) const
	{
		/* 包含 */
		return  (*this >> latter) || (*this << latter);
	}

	IComparable&  merge(const IComparable &latter, Direction d) // 合并两条具有包含关系的K线
	{
		// 两者 是 包含关系的
		assert(*this == latter);

		switch (d)
		{
		case ASCENDING:
			this->setHigh( max(this->getHigh(), latter.getHigh()) );
			this->setLow( max(this->getLow(), latter.getLow()) );
			break;
		case DESCENDING:
			this->setHigh( min(this->getHigh(), latter.getHigh()) );
			this->setLow( min(this->getLow(), latter.getLow()) );
			break;
		default:
			assert(0);
		}
		return *this;
	}

};


#endif