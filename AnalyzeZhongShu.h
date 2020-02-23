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

typedef Class_XianDuan<1> veryBaseXianDuanType;


	static const int MAX_LEVEL = Class_ZhongShu::MAX_LEVEL; // 最大支持的中枢级别， 从中枢级别0 到 中枢级别max_level
private:
	struct ElemOfView
	{
		Class_ZhongShu *elem;
		List_Entry level_link[MAX_LEVEL]; 
		List_Entry time_link;
		ElemOfView(Class_ZhongShu* cs) {elem = cs; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0;}
		ElemOfView() {elem = NULL; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0; }
		char flags; // 用来在merge链表，删除操作的时候，对需要merge的项目进行标记
	};

	List_Entry time_list;
	List_Entry level_list[MAX_LEVEL];

	volatile int total; // time_list队列所含的元素个数
	volatile int levelTotal[MAX_LEVEL]; // 个级别队列所含的元素个数

	volatile int max_level; // 之所以，要将它们设成volatile，是因为，在merge的循环中，如果出现中枢扩张的情形，那么这两个变量可能会发生变化；这样可以避免编译器对循环变量进行优化
	volatile int min_level; 
	// int forward; // 增量式（随着添加中枢，随着merge）、还是待添加所有的中枢之后，再作merge。 如果是增量式，那么同级别中枢列表使用栈，否则，应该使用对列。

	
	void Add_Elem(ElemOfView *eov)
	{
		int level = eov->elem->getGrade();
		assert(level < MAX_LEVEL);

		max_level = max(level, max_level);
		min_level = min(level, min_level);

		append(&time_list, &eov->time_link);
		total++;

		for (int i = 0; i <= level; i++)
		{
			append(&level_list[i], &eov->level_link[i]);
			levelTotal[i]++;
		}
	}

	void Add_Elem(Class_ZhongShu *zs)
	{
		int level = zs->getGrade();
		assert(level < MAX_LEVEL);

		max_level = max(level, max_level);
		min_level = min(level, min_level);

		ElemOfView *elem = new ElemOfView(zs);
		append(&time_list, &elem->time_link);
		total++;

		for (int i = 0; i <= level; i++)
		{
			append(&level_list[i], &elem->level_link[i]);
			levelTotal[i]++;
		}


	}

	void RemoveElem(ElemOfView *eov)
	{
		int level = eov->elem->getGrade();

		int possibleMaxLevel = -1;

		for (int i = 0; i <= level; i++)
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

	List_Entry* exportResult() 
	{
		if (isEmpty(&time_list))
			return NULL;

		List_Entry *result = time_list.next;

		time_list.prev->next = NULL;
		time_list.next->prev = NULL;


		total = 0;
		initializeList(&time_list);
		for (int i = 0; i < min_level; i++)
			assert(isEmpty(&level_list[i]));

		for (int i = min_level; i <= max_level; i++)
		{
			initializeList(&level_list[i]);
			levelTotal[i] = 0;
		}
		
		return result;
	}

	MultiLevelZhongShuView(/*bool mergeForward = true*/)
	{
		max_level = -1; 
		min_level = MAX_LEVEL + 1; 
		total = 0;

		initializeList(&time_list);

		for (int i = 0; i < MAX_LEVEL; i++)
		{
			initializeList(&level_list[i]);
			levelTotal[i] = 0;
		}
		
		/*forward = mergeForward;*/ 
	}

	~MultiLevelZhongShuView() 
	{
		if (total > 0)
		{
			while (!isEmpty(&time_list))
			{
				List_Entry *item = remove(time_list.next);
				ElemOfView *elem = list_entry(item, ElemOfView, time_link);
				delete elem;
				total--;
			}
			assert(total == 0);
		}
	}

	// 在baseLevel的链表中，从searchFrom开始，寻找与baseLevel级别一致的中枢，并返回；
	List_Entry* locate_level_equal(List_Entry *searchFrom, int baseLevel)
	{
		List_Entry *endOfList = &level_list[baseLevel];
		while (searchFrom != endOfList)
		{
			ElemOfView *item = list_entry(searchFrom, ElemOfView, level_link[baseLevel]);
			int itemLevel = item->elem->getGrade();

			if (itemLevel == baseLevel)
				break;
			else
			{
				assert(itemLevel > baseLevel);
				searchFrom = searchFrom->next;
			}
		}

		return searchFrom;
	}

	// 在baseLevel的链表中，从startFrom开始，寻找比baseLevel级别高的中枢，并返回；
	List_Entry* locate_level_higher(List_Entry *searchFrom, int baseLevel)
	{
		List_Entry *endOfList = &level_list[baseLevel];
		while (searchFrom != endOfList)
		{
			ElemOfView *item = list_entry(searchFrom, ElemOfView, level_link[baseLevel]);
			int itemLevel = item->elem->getGrade();

			if (itemLevel == baseLevel)
				searchFrom = searchFrom->next;
			else
			{
				assert(itemLevel > baseLevel);
				break;
			}
		}

		return searchFrom;
	}


	int getZSLevel(Class_ZhongShu *startFrom, Class_ZhongShu *endBy)
	{

		int baseXianDuanCount = endBy->getEnd() - startFrom->getStart() + 1;

		int newLevel;

		if (baseXianDuanCount < 3)
			newLevel = 0;
		else if (baseXianDuanCount < 9)
			newLevel = 1;
		else if (baseXianDuanCount < 27) 
			newLevel = 2;
		else if (baseXianDuanCount < 81) 
			newLevel = 3;
		else if (baseXianDuanCount < 243) 
			newLevel = 4;
		else if (baseXianDuanCount < 729)
			newLevel = 5;
		else if (baseXianDuanCount < 2187)
			newLevel = 6;
		else
			assert(0); // 中枢级别超过支持范围


		//if (newLevel > startFrom->getGrade() + 1)
		//	printf("break me here");

		return newLevel;
	}

	bool canExpand(Class_ZhongShu *former, Class_ZhongShu* latter)
	{
		assert(former->getGrade() == latter->getGrade());

		int childGrade = former->getGrade();

		if (former->intersect(*latter))
		{
			int newGrade = getZSLevel(former, latter);
			return newGrade > childGrade;
		}
		else
			return false;
	}


	// 将 time list队列中，从from开始到to的项目，扩展成为一个新的中枢newZS
	void ExpandZS(ElemOfView *from, ElemOfView* to, Class_ZhongShu *newZS)
	{

		// 建立一个ElemOfView
		int newLevel = newZS->getGrade();
		ElemOfView* newEov = new ElemOfView(newZS);
		
		List_Entry *curr = &from->time_link;
		List_Entry *end = to->time_link.next;
		// 将 time link链表中， 位于from和to之间的各个项 都标记上
		int cnt = 0;
		while (curr != end)
		{
			ElemOfView *eov = list_entry(curr, ElemOfView, time_link);
			eov->flags = 1;
			curr = curr->next;
			cnt++;
		}
		total -= (cnt - 1);

		// 确定新建立的中枢，在newLevel级别队列中 将要插入的位置
		List_Entry *insertAt = from->time_link.prev;
		while (insertAt != &time_list)
		{
			ElemOfView *eov = list_entry(insertAt, ElemOfView, time_link);
			if (eov->elem->getGrade() >= newLevel)
				break;
			insertAt = insertAt->prev;
		}

		// 将 newEov 插入 newLevel队列
		if (insertAt == &time_list)
		{
			insertAt = &level_list[newLevel];
		}
		else
			insertAt = &list_entry(insertAt, ElemOfView, time_link)->level_link[newLevel];

		newEov->level_link[newLevel].next = insertAt->next;
		newEov->level_link[newLevel].prev = insertAt;
		insertAt->next->prev = &newEov->level_link[newLevel];
		insertAt->next = &newEov->level_link[newLevel];
		levelTotal[newLevel]++;


		// 遍历 level link，将打上标记的一段 从链表中移除
		List_Entry *posToInsert;
		for (int i = 0; i < min_level; i++)
			assert(isEmpty(&level_list[i]));

		for (int i = min_level; i < newLevel; i++)
		{
			int cnt = 0;

			curr = level_list[i].next;
			while (curr != &level_list[i] && list_entry(curr, ElemOfView, level_link[i])->flags == 0)
				curr = curr->next;

			posToInsert = curr->prev;
			
			while (curr != &level_list[i] && list_entry(curr, ElemOfView, level_link[i])->flags == 1)
			{
				curr = curr->next;
				cnt++;
			}

			posToInsert->next = &newEov->level_link[i];
			newEov->level_link[i].prev = posToInsert;
			newEov->level_link[i].next = curr;
			curr->prev = &newEov->level_link[i];
			levelTotal[i] -= (cnt - 1);

			if (min_level == i && isEmpty(&level_list[i]))
				min_level++;
		}


		// 将newEov插入 time list队列，并且，将time list队列中打上标记的项目 删除掉
		insertAt = from->time_link.prev;
		newEov->time_link.prev = insertAt;
		newEov->time_link.next = to->time_link.next;
		insertAt->next = &newEov->time_link;
		to->time_link.next->prev = &newEov->time_link;

		curr = &from->time_link;
		while (curr != end)
		{
			List_Entry* next = curr->next;
			ElemOfView *elem = list_entry(curr, ElemOfView, time_link);
			delete elem;
			curr = next;
		}

		max_level = max(max_level, newLevel);
	}

	void merge(Class_XianDuanBase *TP1_2)
	{
		bool changed;
		do
		{
			changed = false;

			for (int level = min_level; level <= max_level; level++)
			{
				if (levelTotal[level] < 2) continue;

				List_Entry *startFrom = locate_level_equal(level_list[level].next, level);
				List_Entry *endAt = locate_level_higher(startFrom, level);
				List_Entry *latter = endAt->prev;
				while (startFrom != &level_list[level])
				{			
					while (latter == startFrom && startFrom != &level_list[level])
					{
						startFrom = locate_level_equal(endAt, level);
						endAt = locate_level_higher(startFrom, level);
						latter = endAt->prev;
					}
					if (startFrom == &level_list[level])
						break;
					
					ElemOfView *eovl = list_entry(latter, ElemOfView, level_link[level]); // eov latter
					ValueRange range = eovl->elem->getCoreRange();

					ElemOfView *eovf, *eovm; // eov former; eov middle
					List_Entry *former = latter->prev;
					int countOverlapZS = 1; // 统计 重叠区间的中枢个数
					while (former != startFrom->prev)
					{
						eovf = list_entry(former, ElemOfView, level_link[level]);
						range && eovf->elem->getCoreRange();
						if (range.isNulValueRange())
							break;
						countOverlapZS++;
						former = former->prev;
					}
					former = former->next;

					if (former == latter)
					{
						if (former != startFrom)
							latter = former->prev;

						continue;
					} 

					if (former != startFrom)
						latter = former->prev;
					else
						latter = former;

					eovf = list_entry(former, ElemOfView, level_link[level]);

					if (countOverlapZS == 3)
					{
						eovm = list_entry(former->next, ElemOfView, level_link[level]);;
					}

					if (canExpand(eovf->elem, eovl->elem))
					{
						Class_ZhongShu *newZS;
						int newLevel = getZSLevel(eovf->elem, eovl->elem);
						switch (countOverlapZS)
						{
						case 2:
							newZS = createZhongShu(eovf->elem, eovl->elem, newLevel);
							break;
						case 3:
							newZS = createZhongShu(eovf->elem, eovm->elem, eovl->elem, newLevel);
							break;
						case 1:
							assert(0);
							break;
						default: //countOverlapZS > 3
							newZS = createZhongShu(eovf->elem->getStart(), eovl->elem->getEnd(), newLevel);
							break;
						}
						ExpandZS(eovf, eovl, newZS);
						changed = true;
					}
				} 
			}

		} while(changed);
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
	typedef MultiLevelZhongShuView::ElemOfView ElemOfView;

	List_Entry* curr = last->zsList;
	while (curr != NULL)
	{
		ElemOfView *elem = list_entry(curr, ElemOfView, time_link);
		curr = curr->next;// 必须在加入view之前，获得下一个元素指针，否则，time_link会被修改
		view.Add_Elem(elem);
	}
	last->zsList = NULL;
}

extern int debugCounter;

template <class XianDuanClass>
void handleTurningPoint(MultiLevelZhongShuView &view, XianDuanClass *TP1_1, XianDuanClass *TP1_2)
{
	typedef MultiLevelZhongShuView::ElemOfView ElemOfView;

	debugCounter++;
	//if (debugCounter == 2734)
	//	printf("break me here");

	List_Entry *curr = TP1_1->zsList;
	while (curr != NULL)
	{
		ElemOfView *elem = list_entry(curr, ElemOfView, time_link);
		curr = curr->next; // 必须在加入view之前，获得下一个元素指针，否则，time_link会被修改
		view.Add_Elem(elem);
	}
	TP1_1->zsList = NULL;

	curr = TP1_2->zsList;
	if (!curr)
	{
		assert(TP1_2->GRADE == 1);
		Class_ZhongShu *zs = createZhongShu(TP1_2, 0);
		view.Add_Elem(zs);

	} else
	{
		while (curr != NULL)
		{
			ElemOfView *elem = list_entry(curr, ElemOfView, time_link);
			curr = curr->next; // 必须在加入view之前，获得下一个元素指针，否则，time_link会被修改
			view.Add_Elem(elem);
		}
		TP1_2->zsList = NULL;
	}

	//if (view.total >= 4)
	//	printf("break me here");
	view.merge(TP1_2);
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
	item.zsList = view.exportResult();
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
		//printf("break me here");
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