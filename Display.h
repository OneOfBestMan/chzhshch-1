#ifndef DISPLAY_H
#define DISPLAY_H

class Class_KXian;

template<class Bi_Or_XianDuan>
class DisplayTemplate
{
public:
	typedef typename Bi_Or_XianDuan::baseItemType_Container::iterator baseItemIterator;
	typedef typename Bi_Or_XianDuan::baseItemType baseItemType;
	typedef typename Bi_Or_XianDuan::ContainerType ContainerType;
	typedef typename Bi_Or_XianDuan ItemType;
	typedef typename Bi_Or_XianDuan::ContainerType::iterator ItemIterator;

	static void doWork(int func, float *resultBuf)
	{
		if (!Bi_Or_XianDuan::container) return;

		ItemIterator curr = Bi_Or_XianDuan::container->begin();
		ItemIterator end = Bi_Or_XianDuan::container->end();


		ItemType item = *curr;

		Class_KXian *veryStart = item.getStartRec();
		Class_KXian *lastEnd = veryStart;

		while (curr != end)
		{
			item = *curr;

			Class_KXian *currStart = item.getStartRec();

			assert(currStart == lastEnd);

			if (func == 0)
				resultBuf[currStart - veryStart] = 1;
			else
				resultBuf[currStart - veryStart] = (item.getDirection() == ASCENDING) ? item.getLow() : item.getHigh();

			lastEnd = item.getEndRec();

			curr++;
		}

		if (func == 0)
			resultBuf[item.getEndRec() - veryStart] = 1;
		else
			resultBuf[item.getEndRec() - veryStart] = (item.getDirection() == ASCENDING) ? item.getHigh() : item.getLow();


	}

};


#endif