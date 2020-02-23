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

	/* �������ӿڣ��Ǹ��������ģ�������С�Ĺ��ɵ�λ */
	Class_XianDuan<1>* getBaseXianDuanStart() {return getStart()->getBaseXianDuanStart();}
	Class_XianDuan<1>* getBaseXianDuanEnd() {return getEnd()->getBaseXianDuanEnd();}

	vector<IZhongShu*> *zsList; //���߶��������� �����б�
	void addZhongShu(IZhongShu* zs)
	{
		if (!zsList)
			zsList = new vector<IZhongShu*>;
		zsList->push_back(zs);
	}

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
		float high1 = getHigh();
		float high2 = latter.getHigh();

		float low1 = getLow();
		float low2 = latter.getLow();

		return (high1 >= high2 && high2 >= low1 || high1 <= high2 && high1 >= low2);
	}
};


/* ��������ӿڣ�����Ϊ�߶Σ����ܰ������ɸ���ͬ��������࣬��Щ������Ҫ���õ�һ���б��У���ˣ�����б��Ǹ����ʵ��б���ˣ���Ҫά�������ָ�룬�Ա㴦����Щ���ʵ����ࡣ */

class IZhongShu
{
public:

	typedef Class_XianDuan<1> veryLowXianDuanType;

	virtual int getGrade() = 0;

	/* ʱ���� */
	virtual bool preceed( IZhongShu &item)  = 0;
	virtual bool follow( IZhongShu &item)  = 0;
	/* �ռ��ϣ�����������̬ */
	virtual bool operator<( IZhongShu &item)  = 0;
	/* �ռ��ϣ������µ���̬ */
	virtual bool operator>( IZhongShu &item)  = 0;
	virtual bool operator==( IZhongShu &latter)  = 0;

	virtual bool operator>> ( IZhongShu &latter)  = 0;
	virtual bool operator<< ( IZhongShu &latter)  = 0;

	virtual float getHigh() const = 0;
	virtual float getLow()  const = 0;
	
	/* ������������ʱ��ʹ�� */
	virtual Class_KXian* getStartRec()  = 0;
	virtual Class_KXian* getEndRec() = 0;

	/* ���ڱȽ�����ʱ��ǰ�� */
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
	veryBaseXianDuanType *Start;
	veryBaseXianDuanType *End;
	Direction d; // ����ķ����������߶η����෴�����������µ��߶��У����෽�������ϵġ�


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
				XianDuanType *XianDuan;
			} c2;
			struct{
				// ����9��(��3^x, x>=2��)�μ����߶Σ����ɸ��߼������������
				subXianDuanType *start;
				subXianDuanType *last;
			} c3;
			struct
			{
				// �����߶εĵͼ����߶Σ�δ�ܹ��ɸü�����������Σ���ʱ�����߶ξͱ������ü������ࣻ
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
		/* ǰ�� �� ���� ����������̬ */
		return preceed(latter)  &&  getHigh() < latter.getLow();
	}

	bool operator>(IZhongShu &latter)
	{
		/* ǰ�� �� ���� �����µ���̬ */
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
		/* ǰ�� ���� ���ߣ��������ص� */
		return preceed(latter) && getHigh() > latter.getHigh()  && getLow() < latter.getHigh();
	}

	bool operator<< (IZhongShu &latter)
	{
		/* ǰ�� ���� ���ߣ� �������ص� */
		return preceed(latter) && getHigh() > latter.getLow()  && getHigh() < latter.getHigh();
	}
	
	bool operator==(IZhongShu &latter)
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