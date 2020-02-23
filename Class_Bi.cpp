#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"



Class_Bi<vector<Class_KXian> >::baseItemType_Container* Class_Bi<vector<Class_KXian> >::base_Container = (Class_Bi<vector<Class_KXian> >::baseItemType_Container*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::container = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;
Class_Bi<vector<Class_KXian> >::ContainerType* Class_Bi<vector<Class_KXian> >::intermediate = (Class_Bi<vector<Class_KXian> >::ContainerType*)NULL;


ostream& operator<<(ostream& out, Class_Bi<vector<Class_KXian>>& biObj)
{
	out << "Bi" << "(";

	out.setf(ios_base::fixed, ios_base::floatfield);
	out.precision(2);
	out.width(4);

	out<< biObj.bi.low << ", ";
	out.width(4);
	out<<  biObj.bi.high  << ")";
	return out;

}


void Class_Bi<vector<Class_KXian>>::FenBi(bool release)
{
	if (release == false)
	{
		if (base_Container == NULL)
		{
			// 创建 K线 vector
			Class_KXian::initialize();
			base_Container = Class_KXian::container;
		}
		if (base_Container && !container)
		{
			// step 1: 考虑K线包含关系，找出 类-顶分型、类-底分型；但是，并没有考虑，顶分型、底分型之间 有 5根K线的要求。
			FenBi_Step1();
			// step 2: 结合顶分型、底分型之间，至少5根k线的要求，生成各个笔；
			FenBi_Step2();
		}
	} else
	{
		delete container;
		container = NULL;

		Class_KXian::initialize(true);
		base_Container = NULL;
	}
}

void Class_Bi<vector<Class_KXian>>::FenBi_Step1()
{
	Direction d = ASCENDING; // 从最开始的第1、2根k线，我们假设之前的方向是ascending的，这样方便处理包含关系。

	intermediate = new ContainerType();

	Class_env *env = Class_env::getInstance();

	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();

	Class_KXian temp = *start;

	int KXianCnt = 1; // 一笔内 无“包含关系”的K线的数量； 顶分型、底分型的K线，算在内； 一笔的KXianCnt，应该大于等于5
	baseItemType_Container::iterator p = start;
	do
	{
		while (p != end && temp == *p) // == 表示“包含：
		{
			temp.merge(*p, d);
			p++;
		}

		if (p == end)
		{
			// TODO: 建立最后的一个  类-笔
			float high = max (start->getHigh(), (p-1)->getHigh());
			float low = min (start->getLow(), (p-1)->getLow());
			intermediate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));

			start = p-1;
			break;
		}

		if (Class_KXian::getDirection(temp, *p) == d)
		{
			temp = *p;
			p++;
			KXianCnt++;
		}
		else
		{
			// 方向不再一致, 建立一个 类-笔
			float high = max(start->getHigh(), (p-1)->getHigh());
			float low = min(start->getLow(), (p-1)->getLow());
			intermediate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));

			start = p-1;
			d = -d;
			KXianCnt = 1;
		}
	}while (p != end);
}

void Class_Bi<vector<Class_KXian>>::FenBi_Step2()
{
	typedef enum {TOP = 1, BOTTOM = 2, STARTPOINT = TOP|BOTTOM} FLAGS;
	// 一个笔的两个端点，这里叫做point; 这个端点，是顶 还是 底，用flags标记； 这个顶、底分型的值，用val保存
	typedef struct stackItem {
		baseItemType* point;
		FLAGS flag; 
		float val;

		stackItem(baseItemType* p, FLAGS f, float v) {point =p; flag = f; val = v;}
		stackItem() {point = NULL; flag = (FLAGS)0; val = 0; }
		bool is_top() {return flag & TOP;}
		bool is_bot() {return flag & BOTTOM;}
		bool is_start() {return is_top() && is_bot(); /*起点，可以当成顶或底*/}
	} stackItem;
	vector<stackItem> analyzeStack;

	if (intermediate == NULL) return;

	ContainerType::iterator current = intermediate->begin(); // 当前 “类笔”， “类笔” 不要求满足顶、底之间至少5根互不包含的K线；
	ContainerType::iterator end = intermediate->end();

	// 某个股票的上市第一天的开盘价格，作为第一个笔的 首端点的价格；
	Class_Bi leiBi = *current;
    stackItem lastTopItem, lastBotItem;
	lastTopItem = lastBotItem = stackItem(leiBi.getStart(), STARTPOINT, leiBi.getStart()->getStart());
	analyzeStack.push_back(lastTopItem);


	while (current != end) 
	{
		leiBi = *current;

		if (leiBi.bi.KXianCnt >= MIN_BI_KXIAN)
		{
			if (leiBi.bi.d == ASCENDING)
			{
				if (lastBotItem.point == leiBi.getStart())
				{ /* > 5, ASCEND, LBI=BISTART */
					lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
					analyzeStack.push_back(lastTopItem);
				}
				else
				{
					if (analyzeStack.size() == 1)
					{
					/*  >5, ASCEND, LBI!=BISTART */
						if (leiBi.getHigh() > lastTopItem.val)
						{
							/*
                                       T
                              start    / 
                                 .    /
                                  .  /
                                   ./
                                   B
							*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
							analyzeStack.push_back(lastTopItem);
						}else if (leiBi.getHigh() < lastTopItem.val)
						{
							/*
                              start
                                 .    T
                                  .  /
                                   ./
                                   B
							*/
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getHigh());
							analyzeStack.push_back(lastBotItem);
						}
					} else
					{
						assert(analyzeStack.size() >=2);
						assert(leiBi.getLow() < lastTopItem.val && leiBi.getLow() >= lastBotItem.val);
						if (analyzeStack.back().is_top())
						{
							if (leiBi.getHigh() > lastTopItem.val)
							{
						 /*
                                     T
                             T      /
                            /  .   /
                           /    . /
                          /      B
                         BT
						 */
								analyzeStack.pop_back(); // obsolete top
								lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
								analyzeStack.push_back(lastTopItem);
							}
						} else
						{
						/*
                        T/B
                          \       T      T
                           \     .  .   /
                            \   .    . /
                             \ .      B
                              B
						*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
							analyzeStack.push_back(lastTopItem);
						}
					}

				}
			}else
			{
				if (lastTopItem.point == leiBi.getStart())
				{
					lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
					analyzeStack.push_back(lastBotItem);
				}
				else
				{
					/* DESCEND, > 5 , LTI != LEBSTART*/
					if (analyzeStack.size() == 1)
					{
						if (leiBi.getLow() > lastBotItem.val)
						{
							/*
                                        T
                                       . \
                                      .   \
                                     .     B
                                    .
                                  start
							*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getLow());
							analyzeStack.push_back(lastTopItem);
						}
						else if (leiBi.getLow() < lastBotItem.val)
						{
							/*
                                        T
                                       . \
                                      .   \
                                     .     \
                                  start     \
                                             \
                                              B
							*/
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
							analyzeStack.push_back(lastBotItem);
						}
					}
					else
					{
						assert(analyzeStack.size() >=2);
						assert(leiBi.getHigh() < lastTopItem.val && leiBi.getHigh() >= lastBotItem.val);
						if (analyzeStack.back().is_top())
						{
						/*
                          T
                         / .
                        /   .
                       /     T
                     B/T      \
                               \
                                B
						*/
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
							analyzeStack.push_back(lastBotItem);
						}
						else
						{
						/*
                       T/B
                         \       T
                          \     . \
                           \   .   \
                            \ .     \
                             B       \
                                      \
                                       B
						*/
							if (leiBi.getLow() < lastBotItem.val)
							{
								analyzeStack.pop_back(); // obsolete bottom
								lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
								analyzeStack.push_back(lastBotItem);
							}
						}
					}
				}
			}
		} else
		{
			if (leiBi.bi.d == ASCENDING)
			{
				if (lastBotItem.point == leiBi.getStart()) 
				{
					if (lastBotItem.is_start())
					{
						// lastBot是start point
						assert(analyzeStack.size() == 1);
					}
					else if (leiBi.getHigh() > lastTopItem.val)
					{
						assert(analyzeStack.size() >= 2);

						analyzeStack.pop_back(); // obsolete bottom
						stackItem obsoleteTop = analyzeStack.back();

						assert(lastTopItem.point == obsoleteTop.point);
						if (obsoleteTop.is_start())
						{
							/* 
                                       T
                                      .
                            start    .
                               \    . 
                                \  .
                                 \.
                             obsoleteBot
							*/
							lastBotItem = obsoleteTop;
						}
						else
						{
						/*
                                   T
                            T     .
                           / \   .
                          /   \ .
                         /     B
                        B

						*/
							analyzeStack.pop_back(); // obsolete top
							lastBotItem = analyzeStack.back();
						}

						lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
						analyzeStack.push_back(lastTopItem);
					}
				}
				else
				{
					if (analyzeStack.size() == 1)
					{
						if (leiBi.getHigh() > lastTopItem.val)
						{
							/*
                                       T
                              start    . 
                                 .    .
                                  .  .
                                   ..
                                   B
							*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
							analyzeStack.push_back(lastTopItem);
						}else if (leiBi.getHigh() < lastTopItem.val)
						{
							/*
                              start
                                 .    T
                                  .  .
                                   ..
                                   B
							*/
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getHigh());
							analyzeStack.push_back(lastBotItem);
						}
					}
					else
					{
						assert(analyzeStack.size() >= 2);
						assert(leiBi.getLow() < lastTopItem.val && leiBi.getLow() >= lastBotItem.val);
						if (analyzeStack.back().is_top())
						{
						 /*
                                     T
                             T      .
                            /  .   .
                           /    . .
                          /      B
                         B

						 */
							if (leiBi.getHigh() > lastTopItem.val)
							{
								analyzeStack.pop_back(); // obsolete top
								lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
								analyzeStack.push_back(lastTopItem);
							}
						}
						else
						{
						/*
                        T
                         \       T
                          \     . .   T
                           \   .   . .
                            \ .     B
                             B
						*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getHigh());
							analyzeStack.push_back(lastTopItem);
						}
					}					
				}
			}
			else
			{
				if (lastTopItem.point == leiBi.getStart())
				{
					if (lastTopItem.is_start())
					{
						// DO NOTHING
						assert(analyzeStack.size() == 1);
					}
					else if (leiBi.getLow() < lastBotItem.val)
					{
						assert(analyzeStack.size() >= 2);
						analyzeStack.pop_back(); // obsolete top

						stackItem obsoleteBot = analyzeStack.back();

						assert(lastBotItem.point == obsoleteBot.point);
						if (obsoleteBot.is_start())
						{
                           /*
						   obsoleteTop
                              / .
                             /   .
                            /     .
                           /       .
                        start       .
                                     .
                                     B
                           */
							lastTopItem = obsoleteBot;
						}
						else
						{
							/*
                           T
                            \     T
                             \   / .
                              \ /   .
                               B     .
                                      B
							*/
							analyzeStack.pop_back(); // obsolete bottom
							lastTopItem = analyzeStack.back();
						}
						lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
						analyzeStack.push_back(lastBotItem);
					}
				}
				else
				{
					if (analyzeStack.size() == 1)
					{
						if (leiBi.getLow() < lastBotItem.val)
						{
							/*
							      T
                                 . .
                                .   .
                               .     .
                            start     .
                                      B
							*/
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
							analyzeStack.push_back(lastBotItem);

						}else if (leiBi.getLow() > lastTopItem.val)
						{
							/*
							      T
                                 .  .
                                .    .
                               .      B
                            start

							*/
							lastTopItem = stackItem(leiBi.getEnd(), TOP, leiBi.getLow());
							analyzeStack.push_back(lastTopItem);
						}
					}
					else
					{
						assert(leiBi.getHigh() >= lastBotItem.val && leiBi.getHigh() < lastTopItem.val);
						if (analyzeStack.back().is_top())
						{
						/*
                         T
                        / .
                       /   .
                      /     T
                     /       .
                    /         .
                   /          B
                 B/T
					 */
							lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
							analyzeStack.push_back(lastBotItem);
						}
						else
						{
						/*
                         T/B
                          \       T
                           \     . .
                            \   .   .
                             \ .     .
                              B       .
                                       B
						*/
							if (leiBi.getLow() < lastBotItem.val)
							{
								analyzeStack.pop_back(); // obsolete bottom
								lastBotItem = stackItem(leiBi.getEnd(), BOTTOM, leiBi.getLow());
								analyzeStack.push_back(lastBotItem);
							}
						}
					}		
				}
			}
		}
		current++;
	}

	// 根据 analyzeStack 中的各个 顶、底分型， 生成 笔列表
	container = new ContainerType();
	container->reserve(analyzeStack.size() - 1);

	for (int i = 0; i < analyzeStack.size() - 1; i++)
	{
		stackItem point_start = analyzeStack[i];
		stackItem point_end   = analyzeStack[i+1];

		Direction d = (point_start.is_bot() && point_end.is_top()) ? ASCENDING : DESCENDING;
		container->push_back(Class_Bi(point_start.point, point_end.point, point_start.val, point_end.val, d));
	}
	
	delete intermediate;
	intermediate = NULL;
}

/*
实现 笔
（1）笔的构成： 底分型、顶分型构成一笔
（2）笔的破坏：
（3）笔的属性：
      底分型
	  顶分型
	  
*/


template<class baseItem_Container>
Class_Bi<baseItem_Container>::~Class_Bi(void)
{
}

