#pragma once

#ifndef ICOMPARABLE_H
#define ICOMPARABLE_H

typedef enum Direction
{
	UNKNOWN = -1, // ����K�ߣ���û�з���ģ�
	ASCENDING =1,
	DESCENDING = 2,
	ENCLOSING = 3,
	OVERLAPPING = ENCLOSING, // ���ֻ����������
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
		/* ǰ�� �� ���� ����������̬ */
		return (getHigh() < latter.getHigh()  && getLow() < latter.getLow());
	}

	bool operator>(const IComparable &latter) const
	{
		/* ǰ�� �� ���� �����µ���̬ */
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
		/* ǰ���� */
		return getHigh() >= latter.getHigh()  && getLow() <= latter.getLow();
	}

	bool operator<< (const IComparable &latter) const
	{
		/* ���ǰ */
		return getHigh() <= latter.getHigh()  && getLow() >= latter.getLow();
	}
	
	bool operator==(const IComparable &latter) const
	{
		/* ���� */
		return  (*this >> latter) || (*this << latter);
	}

	IComparable&  merge(const IComparable &latter, Direction d) // �ϲ��������а�����ϵ��K��
	{
		// ���� �� ������ϵ��
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