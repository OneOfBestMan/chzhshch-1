#ifndef ANALYZEZHONGSHU_H
#define ANALYZEZHONGSHU_H

// 如果，两个同级别floatRange有交集，就算扩展，而不考虑所包含的Class_XianDuan<1>的数量，这样，中枢扩展会更为频繁的发生；
// #define DONT_DEPEND_ON_BASIC_XIANDUAN_COUNT 

#include "Class_ZhongShu.h"

#include "slist.h"

#include <list>
using namespace std;

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
extern int debugCounter;

class MultiLevelZhongShuView // 多级别中枢联立视图，用于中枢扩展的分析
{
// template <typename XianDuanClass> friend void handleTurningPoint(MultiLevelZhongShuView &, typename XianDuanClass*, typename XianDuanClass*);
// template <typename XianDuanClass> friend void handleLast(MultiLevelZhongShuView &, typename XianDuanClass*);
template <typename XianDuanClass> friend void doWork(MultiLevelZhongShuView &, XianDuanClass &);
friend void doWork<>(MultiLevelZhongShuView &, Class_XianDuan<2>  &);
template <typename XianDuanClass> friend void AnalyzeZhongShu_Remaining(XianDuanClass *, MultiLevelZhongShuView &);
template <typename XianDuanClass> friend void AnalyzeZhongShu_PostOrder_V2();

typedef Class_XianDuan<1> veryBaseXianDuanType;


	static const int MAX_LEVEL = Class_ZhongShu::MAX_LEVEL; // 最大支持的中枢级别， 从中枢级别0 到 中枢级别max_level
private:
	typedef struct ElemOfView
	{
		Class_ZhongShu *elem;
		List_Entry level_link[MAX_LEVEL]; 
		List_Entry time_link;
		ElemOfView(Class_ZhongShu* cs) {elem = cs; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0;}
		ElemOfView() {elem = NULL; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0; }
		char flags; // 用来在merge链表，删除操作的时候，对需要merge的项目进行标记
	}ElemOfView;

	typedef struct mergeRecord {
		ElemOfView *from; // 中枢扩张 起始于
		ElemOfView *to;   // 中枢扩张 终止于
		int countOverlapZS; // 中枢扩张 包含的低级别中枢个数
		int newLevel;      // 扩张的中枢级别
		mergeRecord() { from = to = NULL; countOverlapZS = 0; newLevel = 0;}
		mergeRecord(ElemOfView *f, ElemOfView* t, int cnt, int level) { from = f; to = t; countOverlapZS = cnt; newLevel = level; }
	} mergeRecord;

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
		//for (int i = 0; i < min_level; i++)
		//	assert(isEmpty(&level_list[i]));

		for (int i = 0; i <= max_level; i++)
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

	// 在listHead的链表中，从searchFrom开始，寻找与baseLevel级别一致的中枢，并返回；
	List_Entry* locate_level_equal(PList_Head listHead, List_Entry *searchFrom, int baseLevel)
	{
		// 确定 listHead 的 级别
		int listLevel = 0;
		for ( ; listHead != &level_list[listLevel]; listLevel++) ; 

		assert(listLevel < MAX_LEVEL);

		while (searchFrom != listHead)
		{
			ElemOfView *item = list_entry(searchFrom, ElemOfView, level_link[listLevel]);
			int itemLevel = item->elem->getGrade();

			if (itemLevel == baseLevel)
				break;
			searchFrom = searchFrom->next;
		}

		return searchFrom;
	}

	// 在listHead的链表中，从startFrom开始，寻找比baseLevel级别高的中枢，并返回；
	List_Entry* locate_level_higher(PList_Head listHead, List_Entry *searchFrom, int baseLevel)
	{
		// 确定 listHead 的 级别
		int listLevel = 0;
		for ( ; listHead != &level_list[listLevel]; listLevel++) ; 

		assert(listLevel < MAX_LEVEL);

		while (searchFrom != listHead)
		{
			ElemOfView *item = list_entry(searchFrom, ElemOfView, level_link[listLevel]);
			int itemLevel = item->elem->getGrade();

			if (itemLevel > baseLevel)
				break;
			searchFrom = searchFrom->next;
		}

		return searchFrom;
	}


	int getZSLevel(Class_ZhongShu *startFrom, Class_ZhongShu *endBy)
	{
#ifdef DONT_DEPEND_ON_BASIC_XIANDUAN_COUNT
		// 如果，两个同级别floatRange有交集，就算扩展，而不考虑所包含的Class_XianDuan<1>的数量
		return startFrom->getGrade() + 1;
#else
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
		else if (baseXianDuanCount < 6561)
			newLevel = 7;
		else
			assert(0); // 中枢级别超过支持范围

		//if (newLevel > startFrom->getGrade() + 1)
		//	printf("break me here");

		return newLevel;
#endif
	}

	bool canExpand(Class_ZhongShu *former, Class_ZhongShu* latter)
	{
		assert(former->getGrade() == latter->getGrade());

		int childGrade = former->getGrade();

		if (former->intersect(*latter))
		{
#ifdef DONT_DEPEND_ON_BASIC_XIANDUAN_COUNT
		 // 如果，两个同级别floatRange有交集，就算扩展，而不考虑所包含的Class_XianDuan<1>的数量
			return true;
#else
			int newGrade = getZSLevel(former, latter);
			return newGrade > childGrade;			
#endif
		}
		else
			return false;
	}


	// 将 time list队列中，从from开始到to的项目，扩展成为一个新的中枢newZS
	ElemOfView* ExpandZS(ElemOfView *from, ElemOfView* to, Class_ZhongShu *newZS)
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

		// 确定新建立的中枢，在0..newLevel级别队列中 将要插入的位置
		for (int level = 0; level <= newLevel; level++)
		{
			List_Entry *insertAt = from->time_link.prev;
			while (insertAt != &time_list)
			{
				ElemOfView *eov = list_entry(insertAt, ElemOfView, time_link);
				if (eov->elem->getGrade() >= level)
					break;
				insertAt = insertAt->prev;
			}

		// 将 newEov 插入 newLevel队列
			if (insertAt == &time_list)
			{
				insertAt = &level_list[level];
			}
			else
				insertAt = &list_entry(insertAt, ElemOfView, time_link)->level_link[level];

			newEov->level_link[level].next = insertAt->next;
			newEov->level_link[level].prev = insertAt;
			insertAt->next->prev = &newEov->level_link[level];
			insertAt->next = &newEov->level_link[level];
			levelTotal[level]++;
		}


		// 遍历 level link，将打上标记的一段 从链表中移除
		List_Entry *posToRemove;
		/*for (int i = 0; i < min_level; i++)
			assert(isEmpty(&level_list[i]));*/


		for (int i = 0; i <= newLevel; i++)
		{
			int cnt = 0;

			curr = level_list[i].next;
			while (curr != &level_list[i] && list_entry(curr, ElemOfView, level_link[i])->flags == 0)
				curr = curr->next;

			if (curr == &level_list[i])
				continue;

			posToRemove = curr->prev;

			
			while (curr != &level_list[i] && list_entry(curr, ElemOfView, level_link[i])->flags == 1)
			{
				curr = curr->next;
				cnt++;
			}

			posToRemove->next = curr;
			curr->prev = posToRemove;

			levelTotal[i] -= cnt;
		}


		// 将newEov插入 time list队列，并且，将time list队列中打上标记的项目 删除掉
		List_Entry *insertAt = from->time_link.prev;
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

		assertValid();

		return newEov;
	}

	// 扫描level_list[level]队列，对 “level + 1”级别的中枢，如果它的第三买卖点未指定，则寻找在它之后出现“紧跟着的”level级别中枢，并判断是否满足第三买卖点条件。
	// 如果满足，那么更新这个中枢；
	void updateThirdPoint(int level)
	{
		if (level == Class_ZhongShu::MAX_LEVEL) return;

		List_Entry *curr = locate_level_equal(&level_list[level], level_list[level].next, level + 1);
		ElemOfView *eov;
		while (curr != &level_list[level])
		{
			eov = list_entry(curr, ElemOfView, level_link[level]);
			if (eov->elem->getThirdPoint())
			{
				curr = locate_level_equal(&level_list[level], curr->next, level + 1);
				continue;
			}
			List_Entry *nextEntry = curr->next;
			while (nextEntry !=  &level_list[level])
			{
				ElemOfView *nextEov = list_entry(nextEntry, ElemOfView, level_link[level]);

				nextEntry = nextEntry->next; // 需要提前获取nexEntry->next，因为nextEntry有可能会被合并 导致从view中删除

				if (nextEov->elem->getGrade() == level)
				{
					ValueRange temp = eov->elem->getCoreRange();
					temp && nextEov->elem->getFloatRange();
					if (temp.isNulValueRange())
					{
						eov->elem->setThirdPoint(nextEov->elem);
						break;
					}
					else
					{
						// 如果有重合，那么需要更新中枢，并且当中枢级别可以上调的时候，要更新View
						int newLevel = getZSLevel(eov->elem, nextEov->elem);
						if (newLevel > level + 1)
						{
							// 一个 leve+1 级别的中枢 和一个 level级别的中枢，构成更大级别中枢
							Class_ZhongShu *newZS = createZhongShu(eov->elem, nextEov->elem, newLevel);
							ElemOfView *newEov = ExpandZS(eov, nextEov, newZS);
							curr = &newEov->level_link[level];
							break; // 之所以break，是因为，当前的newEov的级别已经调高了1级，所以，外层循环的那个level+1的项，已经失效；
						}
						else
						{
							// 一个 leve+1级别的中枢 合并 后面的一个level级别的中枢，中枢级别没有变化。
							eov->elem->updateEnd(nextEov->elem);
							//// RemoveElem(nextEov); // 这句非常错误，因为，level+1和level中枢之间，在time list上可能存在很多低级别中枢，所以绝不仅仅是删除掉中枢level那么简单，而是要把这两个中枢之间的低级别中枢都从view中删除掉
							eov = ExpandZS(eov, nextEov, eov->elem);
							curr = &eov->level_link[level];
						}
					}
				} 
				else 
					break; // 如果一个level+1级别的中枢，之后中枢的级别大于等于该中枢，那么就别再找该中枢的第三买卖点了
			}
			//if (nextEntry == &level_list[level])
			//	break;
			
			curr = locate_level_equal(&level_list[level], curr->next, level + 1);
		}
	}

	/* 对 level_list，实施merge; merge操作的实施方案，在operation中*/
	void applyMergeOps(PList_Head listHead, list<mergeRecord> &operations)
	{
		// 确定 listHead 的 级别
		int listLevel = 0;
		for ( ; listHead != &level_list[listLevel]; listLevel++) ; 

		Class_ZhongShu *newZS = NULL;
		Class_ZhongShu *lastMergeZS = NULL;
		bool setLaterAs3rdPoint = false;
		while (!operations.empty())
		{
			mergeRecord op = operations.front();
			operations.pop_front();

			switch (op.countOverlapZS)
			{
			case 1:
				assert(0);
				break;
			case 2:
				newZS = createZhongShu(op.from->elem, op.to->elem, op.newLevel);
				break;
			case 3:
				{
					ElemOfView *eovm = list_entry(op.from->level_link[listLevel].next, ElemOfView, level_link[listLevel]);
					assert(eovm->elem->getGrade() == listLevel);
					newZS = createZhongShu(op.from->elem, eovm->elem, op.to->elem, op.newLevel);
				}
				break;
			default:
				Class_ZhongShu **zsArray = new Class_ZhongShu*[op.countOverlapZS];
				ElemOfView *curr = op.from;
				int cnt = 0;
				while (curr != op.to)
				{
					zsArray[cnt++] = curr->elem;
					curr = list_entry(curr->level_link[listLevel].next, ElemOfView, level_link[listLevel]);
					assert(curr->elem->getGrade() == listLevel);
				}
				assert(curr == op.to);
				zsArray[cnt++] = op.to->elem;
				assert(cnt == op.countOverlapZS);
				newZS = createZhongShu(zsArray, op.countOverlapZS, op.newLevel);
				delete[] zsArray;
				break;
			}

			if (setLaterAs3rdPoint)
			{
				setLaterAs3rdPoint = false;
				
				ValueRange range = newZS->getFloatRange();
				range && lastMergeZS->getCoreRange();
				if (range.isNulValueRange())
					lastMergeZS->setThirdPoint(newZS);
				
				lastMergeZS = NULL;
			}

			if (!operations.empty())
			{
				// 分析newZS的第三买卖点
				mergeRecord& next = operations.front();
				if (op.newLevel == listLevel + 1) // 新产生的扩张中枢， 级别比 低级别中枢，高出1个级别
				{
					
					if (next.newLevel >= op.newLevel)
					{
					
						ElemOfView *eovAfterNewZS = list_entry(op.to->level_link[listLevel].next, ElemOfView, level_link[listLevel]);

						//assert(eovAfterNewZS->elem->getGrade() == listLevel);
						ValueRange range = eovAfterNewZS->elem->getFloatRange();
						range && newZS->getCoreRange();
						assert(range.isNulValueRange());

						if (eovAfterNewZS != next.from)
							newZS->setThirdPoint(eovAfterNewZS->elem);// 这两个级别扩张的中枢之间，存在一个低级别的中枢，不属于这两个中枢。 那么，这个低级别中枢，就构成了第一个扩张中枢的第三买卖点。
						//else
						//	newZS->setThirdPoint(eovAfterNewZS->elem);//第三买卖点后，又延伸出一个同级或更大级别的中枢，此时，这两个中枢就存在扩张成为更大中枢的可能。
					}
				
				} else // 如果， 新产生的扩张中枢级别比当前队列的级别高出2个或更多
				{ 
					if (next.newLevel == op.newLevel - 1 )   // 下一个扩张中枢，级别 低于 当前扩张中枢 1 个级别					
					{
						lastMergeZS = newZS; 
						setLaterAs3rdPoint = true; // 再生成下一个中枢的时候，设置它成为这个中枢的第三买卖点；
					}
				}
			}

			ExpandZS(op.from, op.to, newZS);
		}
	}

	void merge(Class_XianDuanBase *TP1_2)
	{
		bool changed;
		do
		{
			changed = false;

			for (int level = 0 /*min_level*/; level <= max_level; level++)
			{
				
				debugCounter++;
				/*
				if (debugCounter == 14281)
				{
					printf("break me here");
					assertValid(true);
				}
				*/
				
				updateThirdPoint(level);		

				if (levelTotal[level] < 2)
					continue;

				List_Entry *startFrom = locate_level_equal(&level_list[level], level_list[level].next, level);
				List_Entry *endAt = locate_level_higher(&level_list[level], startFrom, level);
				List_Entry *former = startFrom;

				// 设立mergeOperations，原因在于：为了避免startFrom指针的失效，遍历pool的时候，避免merge； 而是记录下可以合并的中枢，当pool遍历完成后，再进行merge.
				list<mergeRecord> mergeOperations;

				while (startFrom != &level_list[level])
				{
					while (former == endAt && startFrom != &level_list[level])
					{
						if (!mergeOperations.empty())
							applyMergeOps(&level_list[level], mergeOperations);

						// 定位出一段区间，该区间是一段都是level级别的中枢，结束于一个级别高于level的中枢，成为一个pool
						startFrom = locate_level_equal(&level_list[level], endAt, level);
						endAt = locate_level_higher(&level_list[level], startFrom, level);
						former = startFrom;
					}
					if (startFrom == &level_list[level])
						break;

					if (endAt == &level_list[level] && startFrom->prev != &level_list[level])
					{
						ElemOfView *checkPrevEov = list_entry(startFrom->prev, ElemOfView, level_link[level]);
						// 在队尾，有一组连续的level级别中枢；如果之前的那个中枢，级别是level+1，并且updateThirdPoint并没有成功设置该中枢的第三买卖点，后面的次级别中枢合并需要延迟，直至该level+1级别中枢第三买卖点被指定
						if (checkPrevEov->elem->getGrade() == level + 1 && checkPrevEov->elem->getThirdPoint() == NULL)
							break;
					}
					
					ElemOfView *eovf = list_entry(former, ElemOfView, level_link[level]); // eov latter

					ValueRange range = eovf->elem->getFloatRange();

					ElemOfView *eovl; // eov former; eov middle
					List_Entry *latter = former->next;
					int countOverlapZS = 1; // 统计 重叠区间的中枢个数
					while (latter != endAt)
					{
						// 对区间中的各个中枢，从前向后，进行merge处理；这段区间结束于一个级别高于level的中枢；
						eovl = list_entry(latter, ElemOfView, level_link[level]);
						//range && eovf->elem->getCoreRange();
						range && eovl->elem->getFloatRange();
						if (range.isNulValueRange())
							break;
						countOverlapZS++;
						latter = latter->next;
					}
					latter = latter->prev;

					if (latter == former)
					{
						former = latter->next;
						continue;
					} 

					if (latter != endAt->prev)
						former = latter->next;
					else
						former = endAt;

					eovl = list_entry(latter, ElemOfView, level_link[level]);

					if (canExpand(eovf->elem, eovl->elem))
					{
						int newLevel = getZSLevel(eovf->elem, eovl->elem);

						mergeOperations.push_back(mergeRecord(eovf, eovl, countOverlapZS, newLevel));
						
						changed = true;
					}
				} 
			}

		} while(changed);
	}


	void assertValid(bool printIt=false)
	{
		List_Entry *curr = time_list.next;

		List_Entry *currLevel[MAX_LEVEL];
		for (int i = 0; i < MAX_LEVEL; i++)
			currLevel[i] = &level_list[i];

		int levelCnt[MAX_LEVEL] = {0};
		int totalCnt = 0;

		while (curr != &time_list)
		{
			ElemOfView *eov = list_entry(curr, ElemOfView, time_link);
			Class_ZhongShu *zs = eov->elem;

			int level = zs->getGrade();

			for (int i = 0; i <= level; i++)
			{
				ElemOfView *toCheck = list_entry(currLevel[i]->next, ElemOfView, level_link[i]);
				assert(eov == toCheck);

				currLevel[i] = currLevel[i]->next;
				levelCnt[i]++;
			}
		
			curr = curr->next;
			totalCnt++;			
		}
		assert(curr == &time_list);
		for (int i = 0; i < MAX_LEVEL; i++)
			assert(currLevel[i] == level_list[i].prev);

		for (int i = 0; i < MAX_LEVEL; i++)
			assert(levelTotal[i] == levelCnt[i]);
		assert(total == totalCnt);

		if (printIt)
		{
			stringstream filename;
			filename<<"c:\\" <<  "zs_debug" << ".txt";
			ofstream file(filename.str().c_str());

			file.setf(ios_base::hex, ios_base::basefield);
			file << "time_list(0x" << &time_list << "): \n";

			curr = time_list.next;
			while (curr != &time_list)
			{
				ElemOfView *eov = list_entry(curr, ElemOfView, time_link);
				file<< "0x" << curr << ", eov=0x" << eov << ", level=" << eov->elem->getGrade() << "\n";
				curr = curr->next;
			}
			file << "\n";

			for (int i = 0; i < MAX_LEVEL; i++)
			{
				file << "level_list[" << i << "] (0x" << &level_list[i] << "): \n";
				curr = level_list[i].next;
				while (curr != &level_list[i])
				{
					ElemOfView *eov = list_entry(curr, ElemOfView, level_link[i]);
					file<< "0x" << curr << ", eov=0x" << eov << ", level=" << eov->elem->getGrade() << "\n";
					curr = curr->next;
				}
				file<<"\n";
			}
		}
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



template <class XianDuanClass>
void handleTurningPoint(MultiLevelZhongShuView &view, XianDuanClass *TP1_1, XianDuanClass *TP1_2)
{
	typedef MultiLevelZhongShuView::ElemOfView ElemOfView;

	// debugCounter++;
	//if (debugCounter == 2734)
	//	printf("break me here");

	/*
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
	}*/

	//if (view.total >= 4)
	//	printf("break me here");
	view.merge(TP1_2);
}

template <class XianDuanClass>
void doWork(MultiLevelZhongShuView &view, XianDuanClass &item)
{
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::baseItemIterator baseItemIterator;


	// debugCounter++;
	// MultiLevelZhongShuView view;
	
	baseItemIterator TP1_1 = item.getStart();
	baseItemIterator TP1_2 = TP1_1 + 1;
	baseItemIterator end = item.getEnd();

	while (TP1_1 <= end)
	{
		doWork(view, *TP1_1);
		if (TP1_2 <= end)
		{
			doWork(view, *TP1_2);
			// handleTurningPoint(view, TP1_1, TP1_2);
			view.merge(&(*TP1_2));

		}
		//else
			//handleLast(view, TP1_1); // 该线段中的最后那个次级别线段

		TP1_1 = TP1_1 + 2;
		TP1_2 = TP1_2 + 2;
	}
	//item.zsList = view.exportResult();
}

template<>
void doWork(MultiLevelZhongShuView &view, Class_XianDuan<2>  &item)
{
	typedef Class_XianDuan<1> baseItemType;
	typedef Class_XianDuan<2>::baseItemIterator baseItemIterator;

	// MultiLevelZhongShuView view;
	
	baseItemIterator TP1_1 = item.getStart();
	baseItemIterator TP1_2 = TP1_1 + 1;
	baseItemIterator end = item.getEnd();

	while (TP1_1 <= end)
	{
		if (TP1_2 <= end)
		{
			Class_ZhongShu *zs = createZhongShu(&(*TP1_2), 0);
			view.Add_Elem(zs);
		}
		TP1_1 = TP1_1 + 2;
		TP1_2 = TP1_2 + 2;
	}
}

template <class XianDuanClass>
void AnalyzeZhongShu_Remaining(XianDuanClass *startFrom, MultiLevelZhongShuView &view)
{
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType::iterator ItemIterator;
	typedef typename XianDuanClass::baseItemIterator baseItemIterator;


	if (startFrom)
	{
		ItemIterator end =  XianDuanClass::container->end();
		XianDuanClass *remainingEnd = &(*(end - 1));
		while (startFrom <= remainingEnd)
		{
			XianDuanClass& TP1_1 = *startFrom;
			doWork(view, TP1_1);

			if (startFrom + 1 <= remainingEnd)
			{
				XianDuanClass &TP1_2 = *(startFrom + 1);
				doWork(view, TP1_2);

				//handleTurningPoint(view, &TP1_1, &TP1_2);
				view.merge(&TP1_2);
				startFrom += 2;
			}
			else
			{
				//handleLast(view, &TP1_1);
				startFrom++;
			}
		}
		baseItemIterator remaining = (*(end - 1)).getEnd() + 1;
		AnalyzeZhongShu_Remaining<baseItemType>(&(*remaining), view);
		
		//printf("break me here");
	}
}

template <>
void AnalyzeZhongShu_Remaining<Class_XianDuan<1>>(Class_XianDuan<1> *startFrom, MultiLevelZhongShuView &view)
{
}


template <class XianDuanClass>
void AnalyzeZhongShu_PostOrder_V2()
{
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType::iterator ItemIterator;
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::baseItemIterator baseItemIterator;

	if (XianDuanClass::container)
	{
		MultiLevelZhongShuView view;

		ItemIterator curr = XianDuanClass::container->begin();
		ItemIterator end =  XianDuanClass::container->end();


		while (curr < end)
		{
			XianDuanClass& TP1_1 = *curr;
			doWork(view, TP1_1);
			
			if (curr + 1 < end)
			{
				XianDuanClass &TP1_2 = *(curr + 1);
				doWork(view, TP1_2);

				// handleTurningPoint(view, &TP1_1, &TP1_2);
				view.merge(&TP1_2);
				curr += 2;
			}
			else
			{
				//handleLast(view, &TP1_1);
				curr++;
			}
		}

		// 会有一些低级别线段， 它们还没有被归纳到 高级别线段，因此需要处理这些线段
		baseItemIterator remaining = (*(end - 1)).getEnd() + 1;

		// 通常，未能归纳成为高级别线段，都意味着很大的转折产生了；比如，从1900涨到6100，然后从6100跌倒4000点，后一段走势，可能还没有被归纳成为高级别线段；
		// 我更希望能够单独考察后一段下跌段，而不是把它们中的中枢，与之前的上涨段中的中枢去扩展。因此，建立一个单独的view，来分析这个转折之后的线段。
		// 如果，上涨、下跌已经都走出来了，那么，就要分开考察后面的走势； 如果，还看不清楚是否是走势的转折，那么就要统一考察；
		// MultiLevelZhongShuView afterBigTurningPoint;
		// AnalyzeZhongShu_Remaining<baseItemType>(remaining, afterBigTurningPoint);
		AnalyzeZhongShu_Remaining<baseItemType>(&(*remaining), view);
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