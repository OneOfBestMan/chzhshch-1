#include "stdafx.h"
#include "Class_XianDuan.h"


Class_XianDuan<1>::baseItemType_Container* Class_XianDuan<1>::baseItems = (Class_XianDuan<1>::baseItemType_Container*)NULL;
Class_XianDuan<1>::ContainerType* Class_XianDuan<1>::container = (Class_XianDuan<1>::ContainerType*)NULL;


template<>
void FenXianDuan_PostOrderTravel<typename Class_XianDuan<1>>(bool release)
{
	Class_XianDuan<1>::FenXianDuanClass::doWork(release);
}



ostream& operator<<(ostream& ostream, Class_XianDuan<1>& objXianDuan)
{
	typedef Class_XianDuan<1> XianDuanClass;
	ostream << 'X' << XianDuanClass::GRADE << "(";

	ostream.setf(ios_base::fixed, ios_base::floatfield);
	ostream.precision(2);
	ostream.width(4);

	ostream<< objXianDuan.getLow() << ", ";
	ostream.width(4);
	ostream<< objXianDuan.getHigh() << ")";
	return ostream;
}



Class_XianDuan<1>::Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd, Direction direct)
{
	this->Start =biStart;
	this->End = biEnd;
	this->d = direct;

	float high = (*biStart).getHigh();
	float low = (*biStart).getLow();

	while (biStart != biEnd + 1)
	{
		high = max(high, (*biStart).getHigh());
		low = min(low, (*biStart).getLow());
		biStart++;
	}

	this->High = high;
	this->Low = low;


}

Class_XianDuan<1>::Class_XianDuan(baseItemIterator biStart, baseItemIterator biEnd)
{
	baseItemType& start = *biStart;
	baseItemType& end = *biEnd;

	assert(start.getDirection() == end.getDirection() && IComparable::getDirection(start, end) == start.getDirection());

	Class_XianDuan(biStart, biEnd, start.getDirection());
}