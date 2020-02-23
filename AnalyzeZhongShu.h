#ifndef ANALYZEZHONGSHU_H
#define ANALYZEZHONGSHU_H


#include "Class_ZhongShu.h"

#include "slist.h"

/*
  �������ϵ��߶�˵��ת�۵�TurningPoint(TP)��ָ �����ĵ㣺 TP1��TP2

               TP2       /
                /\      /
     TP1       /  \    end
      /\      /    \  /
     /  \    /      \/
TP1_1  TP1_2/
   /      \/
  /
 
 �������µ��߶Σ�TurningPoint��ָ �����ĵ㣺TP1��TP2
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
  �������ϵ��߶�˵�� ����Juxtaposition(JP)��ָ ���������ڣ������غ�����ģ��߶�: JP0/JP1/JP2/JP3 ��  JP2/JP3/JP4/JP5

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

  �������µ��߶�˵�� Juxtaposition��ָ ���������ڣ������غ�����ģ��߶�: JP0/JP1/JP2/JP3 ��  JP2/JP3/JP4/JP5

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

  �������е��߶�(��������߶η��������ϵ�)��Juxtaposition��ָ�� JP0/JP1/JP2/JP3/JP4/JP5
   
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

 �������е��߶�(��������߶η��������µ�)��Juxtaposition��ָ�� JP0/JP1/JP2/JP3
   
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


class MultiLevelZhongShuView // �༶������������ͼ������������չ�ķ���
{
template <typename XianDuanClass> friend void handleTurningPoint(MultiLevelZhongShuView &, typename XianDuanClass*, typename XianDuanClass*);
template <typename XianDuanClass> friend void handleLast(MultiLevelZhongShuView &, typename XianDuanClass*);

typedef Class_XianDuan<1> veryBaseXianDuanType;


	static const int MAX_LEVEL = Class_ZhongShu::MAX_LEVEL; // ���֧�ֵ����༶�� �����༶��0 �� ���༶��max_level
private:
	struct ElemOfView
	{
		Class_ZhongShu *elem;
		List_Entry level_link[MAX_LEVEL]; 
		List_Entry time_link;
		ElemOfView(Class_ZhongShu* cs) {elem = cs; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0;}
		ElemOfView() {elem = NULL; time_link.prev = time_link.next = NULL; for (int i = 0; i < MAX_LEVEL; i++) level_link[i].prev = level_link[i].next = NULL; flags = 0; }
		char flags; // ������merge����ɾ��������ʱ�򣬶���Ҫmerge����Ŀ���б��
	};

	List_Entry time_list;
	List_Entry level_list[MAX_LEVEL];

	volatile int total; // time_list����������Ԫ�ظ���
	volatile int levelTotal[MAX_LEVEL]; // ���������������Ԫ�ظ���

	volatile int max_level; // ֮���ԣ�Ҫ���������volatile������Ϊ����merge��ѭ���У���������������ŵ����Σ���ô�������������ܻᷢ���仯���������Ա����������ѭ�����������Ż�
	volatile int min_level; 
	// int forward; // ����ʽ������������࣬����merge�������Ǵ�������е�����֮������merge�� ���������ʽ����ôͬ���������б�ʹ��ջ������Ӧ��ʹ�ö��С�

	
	void Add_Elem(ElemOfView *eov)
	{
		int level = eov->elem->getGrade();
		assert(level < MAX_LEVEL);

		max_level = max(level, max_level);
		min_level = min(level, min_level);

		append(&time_list, &eov->time_link);
		total++;

		for (int i = min_level; i <= level; i++)
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

			// �� level list������ɾ��eov

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
			// level_list ȫ�����
			assert(total == 1);
			min_level = MAX_LEVEL + 1;
			max_level = -1;
		}else
		{
			if (max_level == level && isEmpty(&level_list[max_level]))
			{
				// ���eov�� max_level���е����һ��Ԫ�أ�����Ҫ����max_level�� �ͼ���ǿն���
				max_level = possibleMaxLevel;
			}

			if (min_level == level + 1)
			{
				// ���eov�� min_level���е����һ��Ԫ�أ���ɨ��߼���ǿն��У�����min_level
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
				delete item;
				total--;
			}
			assert(total == 0);
		}
	}

	// ��baseLevel�������У���searchFrom��ʼ��Ѱ����baseLevel����һ�µ����࣬�����أ�
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

	// ��baseLevel�������У���startFrom��ʼ��Ѱ�ұ�baseLevel����ߵ����࣬�����أ�
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

		if (baseXianDuanCount < 3) return 0;

		if (baseXianDuanCount < 9) return 1;

		if (baseXianDuanCount < 27) return 2;

		if (baseXianDuanCount < 81) return 3;

		if (baseXianDuanCount < 243) return 4;

		if (baseXianDuanCount < 729) return 5;

		if (baseXianDuanCount < 2187) return 6;

		assert(0); // ���༶�𳬹�֧�ַ�Χ
		return -1;
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


	void ExpandZS(ElemOfView *from, ElemOfView* to, int newLevel)
	{

		// ����һ��ElemOfView
		Class_ZhongShu *zs = createZhongShu(from->elem, to->elem, newLevel);
		ElemOfView* newEov = new ElemOfView(zs);
		
		List_Entry *curr = &from->time_link;
		List_Entry *end = to->time_link.next;
		// �� time link�����У� λ��from��to֮��ĸ����� �������
		int cnt = 0;
		while (curr != end)
		{
			ElemOfView *eov = list_entry(curr, ElemOfView, time_link);
			eov->flags = 1;
			curr = curr->next;
			cnt++;
		}
		total -= (cnt - 1);

		// ȷ���½��������࣬��newLevel��������� ��Ҫ�����λ��
		List_Entry *insertAt = from->time_link.prev;
		while (insertAt != &time_list)
		{
			ElemOfView *eov = list_entry(insertAt, ElemOfView, time_link);
			if (eov->elem->getGrade() == newLevel)
				break;
			insertAt = insertAt->prev;
		}

		// �� newEov ���� newLevel����
		if (insertAt == &time_list)
			insertAt = &level_list[newLevel];
		else
			insertAt = &list_entry(insertAt, ElemOfView, time_link)->level_link[newLevel];

		newEov->level_link[newLevel].next = insertAt->next;
		newEov->level_link[newLevel].prev = insertAt;
		insertAt->next->prev = &newEov->level_link[newLevel];
		insertAt->next = &newEov->level_link[newLevel];
		levelTotal[newLevel]++;


		// ���� level link�������ϱ�ǵ�һ�� ���������Ƴ�
		List_Entry *posToInsert;
		for (int i = 0; i < newLevel; i++)
		{
			int cnt = 0;

			curr = level_list[i].next;
			while (curr != &level_list[i] && list_entry(curr, ElemOfView, level_link[i])->flags == 0)
				curr = curr->next;

			if (curr == &level_list[i]) continue;

			posToInsert = curr;
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
		}


		// ��newEov���� time list���У����ң���time list�����д��ϱ�ǵ���Ŀ ɾ����
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
	}

	void merge(Class_XianDuanBase *TP1_2)
	{
		bool changed;
		do
		{
			changed = false;

			for (int level = min_level; level < max_level; level++)
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
					
					ElemOfView *eovl = list_entry(latter, ElemOfView, level_link[level]);
					ValueRange range = eovl->elem->getCoreRange();

					ElemOfView *eovf;
					List_Entry *former = latter->prev;
					while (former != startFrom->prev)
					{
						eovf = list_entry(former, ElemOfView, level_link[level]);
						range && eovf->elem->getCoreRange();
						if (range.isNulValueRange())
							break;
						former = former->prev;
					}
					former = former->next;

					if (former == latter)
					{
						latter = former;
						continue;
					} 
					else
						latter = former->prev;


					eovf = list_entry(former, ElemOfView, level_link[level]);

					if (canExpand(eovf->elem, eovl->elem))
					{
						int newLevel = getZSLevel(eovf->elem, eovl->elem);
						ExpandZS(eovf, eovl, newLevel);
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
		��������0������1������1������2����Ҫ����Ĵ��������ܲ��ø߼���������չ�ķ�ʽ��3�����ಢ�����ص���������ԭ�������ڣ������������С�����߶Σ������߶�1��
		�Ǻ�������Ĵ�С���������λ�����磬3������1�߶ι�������1,9������1�߶ι�������2,27������1�߶ι�������3���Դ����ơ����ǣ������༶����󣬾Ͱ���3��ͬ�������࣬����
		�����ص���ϵ��intersect�������ɸ߼��������������ˣ�������ȥ���ͼ����߶�������ԭ�����ڣ������ӣ���Щ�����ɸ߼�������ģ��μ�������֮����߶Σ���һ���Ǻ���Щ
		�μ�������ͬ������߶Σ�Ʃ�磬������ֱ��������ͣ��Ҳ����һ������1���߶Σ�����ˣ��Ͳ��ٿ�����Щ����֮����߶ε�����������෴�����ڵͼ���������˵�����磬���༶��0
		�����༶��1�����༶��1�����༶��2����Щ����֮��������ߣ����Ǽ���1���߶Σ���ˣ����ò���Ҫ�����ǵ�������ϵ�������ڡ�������˵��
		
	   ��1������intersect������0��3��1�����߶Σ����Ϳ��Թ���1������1��

                            /\
                           /  \
                  /\      /    3
                 /  2    /      \
          /\    2'   \  /        \
         /  1  /      \/
 		/    \/
       /
		
		1��2��3������0�� 1��2��������1��ֻ��Ҫ2������0����intersect��ϵ�Ϳ��Թ�������1�ˣ�������Ҫ��߼���������Ҫ3���ͼ�������Ĳ��Ų��ܹ��ɡ�����ԭ��
		����Ϊ����2'Ҳ������������0����һ���ˡ�

		���⣬����1������2��Ҳ��Ҫ���⴦������������

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

	   ���ͼ���Թ��Ϊ���µ�pattern: һ��TP����������2�߶ι��ɣ���༶��2�߶Σ�����1������1���ࣨ1+2'+2����1������0����(3)���Ҳ༶��2�߶Σ�����1������0���ࣨ5�����Ҳ༶��2�߶Σ�
	   ��������༶��1����ļ۸����䣻 ��ʱ�򣬿��԰Ѽ���1����+����0����+����0���࣬��������2���ࣻ
       */

		do
		{
			changed = false;

			// �������� ���ţ����ص�;
			if (levelTotal[level] >= 3)
			{
				// assert(level_stacks[level].top->elem == zs);
				IZhongShu *last = list_entry(level_list[level].prev, ElemOfView, level_link[level])->elem;
				IZhongShu *prev = list_entry(level_list[level].prev->prev, ElemOfView, level_link[level])->elem;
				IZhongShu *prev_prev = list_entry(level_list[level].prev->prev->prev, ElemOfView, level_link[level])->elem;

				if (zs->getGrade() == prev->getGrade() && prev->getGrade() == prev_prev->getGrade() &&
					zs->intersect(*prev_prev) && prev->intersect(*prev_prev) && zs->intersect(*prev))
				{
					// �������� �����ص� �γɸ��������
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
		
		// baseItem�����У����ܻ���һЩʣ�����Ŀ��û�й����һ������߶Σ���Ȼ��Ҫ�����������������ࡣ ���еķ�����Ҳ���ǣ�����һ����ʱ�ĸ����߶���������Щʣ������߶Σ�
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
		view.Add_Elem(elem);
		curr = curr->next;
	}
}


template <class XianDuanClass>
void handleTurningPoint(MultiLevelZhongShuView &view, XianDuanClass *TP1_1, XianDuanClass *TP1_2)
{
	typedef MultiLevelZhongShuView::ElemOfView ElemOfView;

	List_Entry *curr = TP1_1->zsList;
	while (curr != NULL)
	{
		ElemOfView *elem = list_entry(curr, ElemOfView, time_link);
		view.Add_Elem(elem);
		curr = curr->next;
	}

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
			view.Add_Elem(elem);
			curr = curr->next;
		}
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
			handleLast(view, TP1_1); // ���߶��е�����Ǹ��μ����߶�

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

		// ����һЩ�ͼ����߶Σ� ���ǻ�û�б����ɵ� �߼����߶Σ������Ҫ������Щ�߶�
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