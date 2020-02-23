#ifndef ANALYZEZHONGSHU_H
#define ANALYZEZHONGSHU_H


#include "Class_ZhongShu.h"

#include "slist.h"

/*
  对于向上的线段说，转折点TurningPoint(TP)是指 这样的点： TP1、TP2

               TP2       /
                /\      /
     TP1       /  \    end
      /\      /    \  /
     /  \    /      \/
TP1_1  TP1_2/
   /      \/
  /
 
 对于向下的线段，TurningPoint是指 这样的点：TP1、TP2
  \
   \        /\
 TP1_1  TP1_2 \
	 \    /    \        /\
	  \  /      \      /  \
	   \/        \    /    \
      TP1         \  /      end
                   \/        \
                  TP2         \
                               \
*/

/*
  对于向上的线段说， 并列Juxtaposition(JP)是指 这样的相邻（且有重合区域的）线段: JP0/JP1/JP2/JP3 、  JP2/JP3/JP4/JP5

                           /\
                          /  \      /
                         /   JP5   /
                /\     JP4     \  /
               /  \    /        \/
      /\     JP2  JP3 /
     /  \    /      \/
   JP0  JP1 /
   /      \/
  /

  对于向下的线段说， Juxtaposition是指 这样的相邻（且有重合区域的）线段: JP0/JP1/JP2/JP3 、  JP2/JP3/JP4/JP5

  \
   \      /\
   JP0  JP1 \          
	 \  /    \        /\
      \/    JP2     JP3 \
               \    /    \          /
                \  /      \        /
                 \/       JP4    JP5
                            \    /
                             \  /
                              \/

  对于下列的线段(如果父亲线段方向是向上的)，Juxtaposition是指： JP0/JP1/JP2/JP3/JP4/JP5
   
                                 /\
                                /  \
                               /    \
                /\            /      \
     /\        /  \          /        \
    /  \      /    \        /          \
  JP0  JP1  JP2    JP3    JP4          JP5
  /      \  /        \    /              \
          \/          \  /                \
                       \/

 对于下列的线段(如果父亲线段方向是向下的)，Juxtaposition是指： JP0/JP1/JP2/JP3
   
                                 /\
                                /  \
                               /    \
                /\            /      \
     /\        /  \          /        \
    /  \      /    \        /          \
   /  JP0   JP1    JP2    JP3           \
  /      \  /        \    /              \
          \/          \  /                \
                       \/

*/


class MultiLevelZhongShuView // 多级别中枢联立视图，用于中枢扩展的分析
{
template <typename XianDuanClass> friend void handleTurningPoint(MultiLevelZhongShuView &, typename XianDuanClass*, typename XianDuanClass*);
template <typename XianDuanClass> friend void handleLast(MultiLevelZhongShuView &, typename XianDuanClass*);

private:
	static const int MAX_LEVEL = 7; // 最大支持的中枢级别， 从中枢级别0 到 中枢级别max_level

	struct ElemOfView
	{
		IZhongShu *elem;
		List_Entry level_link[MAX_LEVEL]; 
		List_Entry time_link;
		ElemOfView(IZhongShu* cs) {elem = cs; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; }
		ElemOfView() {elem = NULL; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL;  }
	};

	List_Entry time_list;
	List_Entry level_list[MAX_LEVEL];

	int total; // time_list队列所含的元素个数
	int levelTotal[MAX_LEVEL]; // 个级别队列所含的元素个数

	int max_level;
	int min_level;
	// int forward; // 增量式（随着添加中枢，随着merge）、还是待添加所有的中枢之后，再作merge。 如果是增量式，那么同级别中枢列表使用栈，否则，应该使用对列。

	

	void Add_Elem(IZhongShu *zs)
	{
		int level = zs->getGrade();
		assert(level < MAX_LEVEL);

		max_level = max(level, max_level);
		min_level = min(level, min_level);

		ElemOfView *elem = new ElemOfView(zs);
		append(&time_list, &elem->time_link);
		total++;

		for (int i = min_level; i <= level; i++)
		{
			append(&level_list[i], &elem->level_link[i]);
			levelTotal[i]++;
		}
	}

	void RemoveElem(ElemOfView *eov)
	{
		int level = eov->elem->getGrade();

		int possibleMaxLevel = -1;

		for (int i = min_level; i <= level; i++)
		{
			assert(!isEmpty(&level_list[i]) && levelTotal[i] > 0);

			// 从 level list队列中删除eov

			remove_entry(eov, level_link[i]);
			levelTotal[i]--;

			if (min_level == i && isEmpty(&level_list[i]))
			{
				assert(levelTotal[i] == 0);
				min_level++;
			}

			if (levelTotal[i] > 0)
				possibleMaxLevel = i;
		}

		if (min_level > max_level)
		{
			// level_list 全变空了
			assert(total == 1);
			min_level = MAX_LEVEL + 1;
			max_level = -1;
		}else
		{
			if (max_level == level && isEmpty(&level_list[max_level]))
			{
				// 如果eov是 max_level队列的最后一个元素，则需要更新max_level到 低级别非空队列
				max_level = possibleMaxLevel;
			}

			if (min_level == level + 1)
			{
				// 如果eov是 min_level队列的最后一个元素，则扫描高级别非空队列，更新min_level
				while (isEmpty(&level_list[min_level]))
					min_level++;
			}
		}

		remove_entry(eov, time_link);
		delete eov;
		total--;

		if (!total) 
			assert(isEmpty(&time_list));
	}

public:

	vector<IZhongShu*>* getResult() {return NULL;}

	MultiLevelZhongShuView(/*bool mergeForward = true*/)
	{
		max_level = -1; 
		min_level = MAX_LEVEL + 1; 

		initializeList(&time_list);

		for (int i = 0; i < MAX_LEVEL; i++)
			initializeList(&level_list[i]);

		/*forward = mergeForward;*/ 
	}

	~MultiLevelZhongShuView() 
	{
		if (total > 0)
		{
			while (!isEmpty(&time_list))
			{
				List_Entry *item = remove(time_list.next);
				delete item;
				total--;
			}
			assert(total == 0);
		}
	}


	void merge(Itraits *TP1_2)
	{

	}

#if 0

	void merge(IZhongShu *zs)
	{
		bool changed = false;
		int level = zs->getGrade();

		/* 
		对于中枢0到中枢1，中枢1到中枢2，需要特殊的处理，而不能采用高级别中枢扩展的方式（3个中枢并排有重叠）。归其原因，是由于，构成中枢的最小级别线段，即：线段1，
		是衡量中枢的大小的最基本单位，比如，3个级别1线段构成中枢1,9个级别1线段构成中枢2,27个级别1线段构成中枢3，以此类推。但是，在中枢级别变大后，就按照3个同级别中枢，并排
		存在重叠关系（intersect），构成高级别中枢来处理了，而不再去数低级别线段数量。原因在于，（连接）这些（构成高级别中枢的）次级别中枢之间的线段，不一定是和这些
		次级别中枢同级别的线段（譬如，可以是直接跳空涨停，也就是一个级别1的线段），因此，就不再考虑这些中枢之间的线段的数量；与此相反，对于低级别中枢来说，比如，中枢级别0
		到中枢级别1、中枢级别1到中枢级别2，这些中枢之间的连接线，就是级别1的线段，因此，不得不需要将它们的数量关系考虑在内。举例来说：
		
	   （1）两个intersect的中枢0（3个1分钟线段），就可以构成1个中枢1，

                            /\
                           /  \
                  /\      /    3
                 /  2    /      \
          /\    2'   \  /        \
         /  1  /      \/
 		/    \/
       /
		
		1、2、3是中枢0， 1和2构成中枢1，只需要2个中枢0具有intersect关系就可以构成中枢1了，而不需要像高级别中枢需要3个低级别中枢的并排才能构成。究其原因，
		是因为，把2'也算作构成中枢0的那一笔了。

		此外，中枢1到中枢2，也需要特殊处理，比如下例：

                                         TP
                                         /\
                                        /  \      /\
                                       /    \    5  \
                            /\        4'     \  /    \
                           /  \      /        \/      \
                  /\      3'   3    /                  \
                 /  2    /      \  /                    \
          /\    2'   \  /        \/                      \
         /  1  /      \/                                  \
 		/    \/
       /

	   这个图可以归结为如下的pattern: 一个TP由两个级别2线段构成；左侧级别2线段，包含1个级别1中枢（1+2'+2）、1个级别0中枢(3)；右侧级别2线段，包含1个级别0中枢（5）；右侧级别2线段，
	   跌到了左侧级别1中枢的价格区间； 这时候，可以把级别1中枢+级别0中枢+级别0中枢，看做级别2中枢；
       */

		do
		{
			changed = false;

			// 三个中枢 并排，有重叠;
			if (levelTotal[level] >= 3)
			{
				// assert(level_stacks[level].top->elem == zs);
				IZhongShu *last = list_entry(level_list[level].prev, ElemOfView, level_link[level])->elem;
				IZhongShu *prev = list_entry(level_list[level].prev->prev, ElemOfView, level_link[level])->elem;
				IZhongShu *prev_prev = list_entry(level_list[level].prev->prev->prev, ElemOfView, level_link[level])->elem;

				if (zs->getGrade() == prev->getGrade() && prev->getGrade() == prev_prev->getGrade() &&
					zs->intersect(*prev_prev) && prev->intersect(*prev_prev) && zs->intersect(*prev))
				{
					// 三个中枢 并排重叠 形成更大的中枢
					changed = true;

				}	
			}
		}while (changed);
	}

#endif



};



#if 0

template <class XianDuanClass>
class AnalyzeZhongShu_Template
{
public:
	typedef typename XianDuanClass::baseItemType_Container::iterator baseItemIterator;
	typedef typename XianDuanClass::ContainerType::iterator ItemIterator;
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType ContainerType;
	typedef typename XianDuanClass::baseItemType_Container baseItemType_Container;



	static void handleTurningPoint(ItemIterator &curr)
	{
		MultiLevelZhongShuView view;

		baseItemType* TP1_1 = (*curr).getStart();
		baseItemType* TP1_2 = TP1_1 + 1;
		baseItemType* end = (*curr).getEnd();

		while (TP1_1 < end)
		{
			if (!TP1_1->zsList && !TP1_2->zsList)
			{
				IZhongShu *zs = createZhongShu(TP1_2);
				view.Add_Elem(zs);

			} 

			TP1_1 = TP1_1 + 2;
			TP1_2 = TP1_2 + 2;
		}
	}

	static void handleTurningPoint(baseItemType *start, baseItemType *end)
	{/*
		baseItemType *TP1_1 = start;
		baseItemType *TP1_2 = start + 1;
		while (TP1_1 < end)
		{
			if (!TP1_1->zsList && !TP1_2->zsList)
			{

			}
		}*/
	}

	static void handleJuxtaposition(ItemIterator &curr)
	{
	}

	static void handleJuxtaposition(baseItemType *start, baseItemType *end)
	{
	}

	static void doWork() 
	{
		if (XianDuanClass::container == NULL)  return;

		ItemIterator curr = XianDuanClass::container->begin();
		ItemIterator end = XianDuanClass::container->end();
		baseItemType* veryEnd = &(*(XianDuanClass::baseItems->end() - 1));

		while (curr < end)
		{
			handleTurningPoint(curr);
			handleJuxtaposition(curr);

			curr++;
		}
		
		// baseItem队列中，可能会有一些剩余的项目，没有归入高一级别的线段，依然需要分析它们所包含中枢。 可行的方法，也许是，创建一个临时的父亲线段来包含这些剩余的子线段？
		baseItemType *remaining = (*(end-1)).getEnd() + 1;
		if (remaining < veryEnd)
		{
			// XianDuanClass temp = new XianDuanClass(baseItemIterator(remaining), baseItemIterator(veryEnd));
			handleTurningPoint(remaining, veryEnd);
			handleJuxtaposition(remaining, veryEnd);
		}
	}
};


template <class XianDuanClass>
void AnalyzeZhongShu_PostOrder()
{
	typedef typename XianDuanClass::baseItemType baseItemType;

	AnalyzeZhongShu_PostOrder<baseItemType>();
	AnalyzeZhongShu_Template<XianDuanClass>::doWork();
}


template <>
void AnalyzeZhongShu_PostOrder<Class_XianDuan<1>>()
{
}

#endif



template <class XianDuanClass>
void handleLast(MultiLevelZhongShuView &view, XianDuanClass *last)
{
}


template <class XianDuanClass>
void handleTurningPoint(MultiLevelZhongShuView &view, XianDuanClass *TP1_1, XianDuanClass *TP1_2)
{
	if (!TP1_1->zsList && !TP1_2->zsList)
	{
		//assert(TP1_1->GRADE == 1);
		IZhongShu *zs = createZhongShu(TP1_2);
		view.Add_Elem(zs);
	}
	else
	{
		for (int i = 0; i < TP1_1->zsList->size(); i++)
			view.Add_Elem(TP1_1->zsList->at(i));
		for (int i = 0; i < TP1_2->zsList->size(); i++)
			view.Add_Elem(TP1_2->zsList->at(i));

		view.merge(TP1_2);
	}
}

template <class XianDuanClass>
void doWork(XianDuanClass &item)
{
	typedef typename XianDuanClass::baseItemType baseItemType;

	MultiLevelZhongShuView view;
	
	baseItemType* TP1_1 = item.getStart();
	baseItemType* TP1_2 = TP1_1 + 1;
	baseItemType* end = item.getEnd();

	while (TP1_1 <= end)
	{
		doWork(*TP1_1);
		if (TP1_2 <= end)
		{
			doWork(*TP1_2);
			handleTurningPoint(view, TP1_1, TP1_2);
		}
		else
			handleLast(view, TP1_1); // 该线段中的最后那个次级别线段

		TP1_1 = TP1_1 + 2;
		TP1_2 = TP1_2 + 2;
	}
	item.zsList = view.getResult();
}

template<>
void doWork(Class_XianDuan<1>  &item)
{
}

template <class XianDuanClass>
void AnalyzeZhongShu_Remaining(XianDuanClass *startFrom, MultiLevelZhongShuView *view)
{
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType::iterator ItemIterator;


	if (startFrom)
	{
		ItemIterator end =  XianDuanClass::container->end();
		XianDuanClass *remainingEnd = &(*(end - 1));
		while (startFrom <= remainingEnd)
		{
			XianDuanClass& TP1_1 = *startFrom;
			doWork(TP1_1);

			if (startFrom + 1 <= remainingEnd)
			{
				XianDuanClass &TP1_2 = *(startFrom + 1);
				doWork(TP1_2);

				handleTurningPoint(*view, &TP1_1, &TP1_2);
				startFrom += 2;
			}
			else
			{
				handleLast(*view, &TP1_1);
				startFrom++;
			}
		}
		baseItemType *remaining = (*(end - 1)).getEnd() + 1;
		AnalyzeZhongShu_Remaining<baseItemType>(remaining, view);
	}
}

template <>
void AnalyzeZhongShu_Remaining<Class_XianDuan<1>>(Class_XianDuan<1> *startFrom, MultiLevelZhongShuView *view)
{
}


template <class XianDuanClass>
void AnalyzeZhongShu_PostOrder_V2()
{
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType::iterator ItemIterator;


	if (XianDuanClass::container)
	{
		MultiLevelZhongShuView view;

		ItemIterator curr = XianDuanClass::container->begin();
		ItemIterator end =  XianDuanClass::container->end();


		while (curr < end)
		{
			XianDuanClass& TP1_1 = *curr;
			doWork(TP1_1);
			
			if (curr + 1 < end)
			{
				XianDuanClass &TP1_2 = *(curr + 1);
				doWork(TP1_2);

				handleTurningPoint(view, &TP1_1, &TP1_2);
				curr += 2;
			}
			else
			{
				handleLast(view, &TP1_1);
				curr++;
			}
		}

		// 会有一些低级别线段， 它们还没有被归纳到 高级别线段，因此需要处理这些线段
		baseItemType *remaining = (*(end - 1)).getEnd() + 1;
		AnalyzeZhongShu_Remaining<baseItemType>(remaining, &view);
	}
	else
	{
		AnalyzeZhongShu_PostOrder_V2<baseItemType>();
	}
}

template <>
void AnalyzeZhongShu_PostOrder_V2<Class_XianDuan<1>>()
{
}

#endif