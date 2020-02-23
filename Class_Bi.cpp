#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"



Class_Bi::baseItemType_Container* Class_Bi::base_Container = (Class_Bi::baseItemType_Container*)NULL;
Class_Bi::ContainerType* Class_Bi::container = (Class_Bi::ContainerType*)NULL;

Class_LeiBi::baseItemType_Container* Class_LeiBi::base_Container = (Class_LeiBi::baseItemType_Container*)NULL;
Class_LeiBi::ContainerType* Class_LeiBi::container = (Class_LeiBi::ContainerType*)NULL;


Class_Bi::Class_Bi(Class_Bi::leiBi *biStart, Class_Bi::leiBi *biEnd)
{
	Start = biStart;
	End = biEnd;
	
	d = Start->getDirection();

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

Class_Bi::Class_Bi(Class_Bi::leiBi *biStart)
{
	Start = End = biStart;
	d = Start->getDirection();
	this->High = (*biStart).getHigh();
	this->Low = (*biStart).getLow();
}



ostream& operator<<(ostream& out, Class_Bi& biObj)
{
	out << "Bi" << "(";

	out.setf(ios_base::fixed, ios_base::floatfield);
	out.precision(2);
	out.width(4);

	out<< biObj.getLow() << ", ";
	out.width(4);
	out<<  biObj.getHigh()  << ")";
	return out;

}

ostream& operator<<(ostream& out, Class_LeiBi& biObj)
{
	out << "Bi" << "(";

	out.setf(ios_base::fixed, ios_base::floatfield);
	out.precision(2);
	out.width(4);

	out<< biObj.getLow() << ", ";
	out.width(4);
	out<<  biObj.getHigh()  << ")";
	return out;

}


void Class_LeiBi::FenBi(bool release)
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
		}
	} else
	{
		delete container;
		// delete intermediate;
		// intermediate = container = NULL;
		container = NULL;

		Class_KXian::initialize(true);
		base_Container = NULL;
	}
}


void Class_LeiBi::FenBi_Step1()
{
	Direction d = ASCENDING; // 从最开始的第1、2根k线，我们假设之前的方向是ascending的，这样方便处理包含关系。

	ContainerType* intermidiate = new ContainerType();

	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();

	Class_KXian temp = *start;

	int KXianCnt = 1; // 一笔内 无“包含关系”的K线的数量； 顶分型、底分型的K线，算在内； 一笔的KXianCnt，应该大于等于5
	baseItemType_Container::iterator p = start;

	
	/*
	注意: 一个笔的高、低价位，并不是简单地统计该笔所包含K线的最高值、最低值，必须考虑包含关系，譬如：
          |
          |___high
      |   |
      | | |
        | |
          |___low

      这个笔的方向是向下的，里面包含3根k线， 笔的high、low，如图所示，不难发现，笔的high值并不是第3根k线的最高点，这是由于后2根k线，进行了包含关系处理
	  再譬如：下面的笔方向向上，但笔的low并不是最后那根k线的最低值，这是由于后2根k线，进行了包含关系处理.
           _____high
          |
        | |
      | | |
      |   |_____low
          |
	*/
	float high = start->getHigh(); // 统计这个笔 的 high
	float low = start->getLow();  // 统计 这个笔 的 low

	do
	{
		while (p != end && temp == *p) // == 表示“包含：
		{
			temp.merge(*p, d);
			p++;
		}
		high = max(high, temp.getHigh());
		low = min(low, temp.getLow());

		if (p == end)
		{
			// TODO: 建立最后的一个  类-笔
			intermidiate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));

			start = p-1;
			break;
		}

		if (Class_KXian::getDirection(temp, *p) == d)
		{
			temp = *p;
			p++;
			KXianCnt++;
		}
		else if (Class_KXian::getDirection(temp, *p) == -d)
		{
			
			if (KXianCnt == 1)
			{
				/* KXianCnt 是1，只可能发生在最开始 几根K线，存在包含关系。 例如：

                     |
                   | |   |
                   | | | |
                   | | | |
                     | |
                     |

				  由于，我假定，初始的方向是acscending，所以，在处理了第1、第2 k线包含关系后，发现第3根k线 与之 方向相反，此时，KXianCnt是1；所以把ascending变为descending，
				  再来一次，但是，在处理了第1、2、3k线的包含关系后，第4根k线又与包含关系k线相反，此时KXianCnt依然是1；所以，开始处的这几根包含关系k线，既不能说是向上、也不能说是向下；
				  所以，这种情况，打破了我对“类笔”至少由 2根 没有包含关系的K线构成的认识。 处理的方式是： 假设第1根k线，之前有一个虚无的k线，构成一个类笔。
			   */
				assert(start == base_Container->begin());
				KXianCnt = 2;
			}

			intermidiate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));

			start = p-1;
			d = -d;
			KXianCnt = 1;

			high = start->getHigh();
			low = start->getLow();
		}
	}while (p != end);
	// 将剩余的K线做成1个类笔
	if (start != end -  1)
		intermidiate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));


	/* 对intermidiate中的各个类笔，做处理，检查下列情形：即 这3个类笔，各自KXianCnt都是2， 并且 第1、第3 类笔 同向或包含， 将这3个类笔，合并成为1个类笔，总笔数是4

      。 
            。
          。   
                 。
       
    */
	container = new ContainerType();
	ContainerType::iterator former = intermidiate->begin();
	ContainerType::iterator endl = intermidiate->end();

	while (former < endl - 2)
	{
		ContainerType::iterator latter = former + 2;

		int fKXianCnt = (*former).getKXianCnt();
		int mKXianCnt = (*(former + 1)).getKXianCnt();
		int lKXianCnt = (*latter).getKXianCnt();

		if (lKXianCnt != 2)
		{
			container->push_back(*former);
			container->push_back(*(former + 1));
			container->push_back(*latter);
			former = latter + 1;
			continue;
		} else if (mKXianCnt != 2)
		{
			container->push_back(*former);
			container->push_back(*(former + 1));
			former = latter;
			continue;
		} else if (fKXianCnt != 2)
		{
			container->push_back(*former);
			former++;
			continue;
		}
		
		Direction df = (*former).getDirection();
		
		if (df == getDirection(*former, *latter))
		{
			float high = max((*former).getHigh(), (*latter).getHigh());
			float low = min((*former).getLow(), (*latter).getLow());

			container->push_back(ContainerType::value_type((*former).getStart(), (*latter).getEnd(), high, low, df, 4));
			former = latter + 1;
		}
		else if (*former >> *latter)
		{
			/*
			这两个线段，前包后，merge之后，应该是按照df取 former方向相反的方式来merge
                 /\     _ high
                /  \  / 
             former \/
              /         
                        - low
			*/
			ContainerType::value_type merg = (*former);
			merg.merge(*latter, -df);
			container->push_back(ContainerType::value_type((*former).getStart(), (*latter).getEnd(), merg.getHigh(), merg.getLow(), df, 4));

			former = latter + 1;
		}
		else
		{
			container->push_back(*former);
			++former;
		}
	}
	// 拷贝剩余的类笔
	while (former != endl)
	{
		container->push_back(*former);
		former++;
	}
	delete intermidiate;
}


void Class_Bi::FenBi(bool release)
{
	if (release == false)
	{
		if (base_Container == NULL)
		{
			baseItemType::FenBi();
			base_Container = baseItemType::container;
		}
		if (base_Container && !container)
		{
			// step 2: 结合顶分型、底分型之间，至少5根k线的要求，生成各个笔；
			FenBi_Step2();
		}
	} else
	{
		delete container;
		// delete intermediate;
		// intermediate = container = NULL;
		container = NULL;

		baseItemType::FenBi(true);
		base_Container = NULL;
	}
}

bool Class_Bi::bckwdSearch(ContainerType::reverse_iterator from, Class_LeiBi *cmpTo, Direction d)
//bool Class_Bi::bckwdSearch(int from, Class_LeiBi *cmpTo, Direction d)
{
	// backward search

	//Class_Bi temp(cmpTo);
	//container->push_back(temp);

	ContainerType::reverse_iterator former = from;
	//ContainerType::reverse_iterator former = container->rbegin() + from;
	ContainerType::reverse_iterator latter = container->rbegin();
	ContainerType::reverse_iterator end = container->rend();
	ContainerType::reverse_iterator result = container->rbegin();

	//while (former < end - 2)
	//while (end - former > 2)
	while (former < end)
	{
		Direction d = (*latter).getDirection();
		if (d == getDirection(*former, *latter))
		{
			result = former;
			break;
		}
		else if (*former <<  *latter)
		{
			latter = former;

			if (former < end - 2)
				former += 2;
			else
				break;
		} else
			break;
			
	}

	if (result != container->rbegin())
	{
		Class_LeiBi *s = (*result).getStart();
		Class_LeiBi *e = cmpTo;
		latter = container->rbegin();
		while (latter != result + 1)
		{
			latter++;
			container->pop_back();
		}
		container->push_back(Class_Bi(s, e));
		return true;
	} else
	{
		// 将 cmpTo 弹出
		container->pop_back();
		return false;
	}
}

void Class_Bi::checkValid()
{
	ContainerType::iterator former =  container->begin();
	ContainerType::iterator endl = container->end();
	int length = container->size();
	while (former != endl - 1)
	{
		assert((*former).getEndRec() == (*(former+1)).getStartRec());
		former++;
	}
}

void Class_Bi::FenBi_Step2()
{
	container = new ContainerType();
	
	baseItemType_Container::iterator current = base_Container->begin();
	while (current < base_Container->end() - 2)
	{
		Class_LeiBi obj = *current;
		if ((*current).getKXianCnt() >= 5)
		{
			container->push_back(ContainerType::value_type(&(*current)));
			current++;
		}
		else
		{
		
		/*
		case 1：第1类笔、第3类笔 同向， 则 “类笔1、类笔2、类笔3”构成  1笔
        。
          。      。
            。  。  。
              。      。
                        。

		case 2：第1类笔、第3类笔 后包前， 则考察（1）类笔1 是否和 前两类笔 构成1笔（2）类笔1、类笔2 是否可以和前一类笔 构成1笔。考察方法是：
		逆向 扫描， 如果反向包含 或 反向时， 停止； 如果同向，那么就算找到了 1笔；如果包含，向前继续扫描。
                       。
         。          。  。
           。      。      。
             。  。          。
               。              。
                                 。
		case 3：第1类笔、第3类笔 前包后：向前扫描， 如果 后包前 或 反向， 停止；  如果前包后 或者 同向， 向前扫描。
         。
           。          。
             。      。  。
               。  。      。
                 。
		case 4：第1类笔、第3类笔 反向，则考察： 第1、第2类笔 逆向扫描；  第1类笔 逆向扫描。
                      。
        。          。  。
          。      。      。
            。  。
              。

		*/
			Direction d = (*current).getDirection();
			Direction dBi = leiBi::getDirection(*current, *(current + 2));
			if (d == dBi)
			{
				container->push_back(ContainerType::value_type(&(*current), &(*(current+2))));
				current = current + 3;
			}
			else if (-d == dBi  ||  *current << *(current + 2) )
			{
				if (container->size())
				{
					Class_Bi temp(&(*(current + 1)));
					container->push_back(temp);
					if (bckwdSearch(container->rbegin() + 1, &(*(current + 1)), -d))
					{
						current += 2;
						continue;
					}
				}

				if (container->size() > 1)
				{
					Class_Bi temp(&(*current));
					container->push_back(temp);
					if (bckwdSearch(container->rbegin() + 2,  &(*current), d))
					{
						current++;
						continue;
					}

				}
				
				{
					container->push_back(ContainerType::value_type(&(*current)));
					container->push_back(ContainerType::value_type(&(*(current+1))));
					current = current + 2;
				}
			}else
			{
				if (container->size() > 1)
				{
					Class_Bi temp(&(*current));
					container->push_back(temp);
					if (bckwdSearch(container->rbegin() + 2,  &(*current), d))
					{
						current++;
						continue;
					}
				}
				
				{
					container->push_back(ContainerType::value_type(&(*current), &(*(current+2))));
					current = current + 3;
				}
			}
		}
	}


}



#if 0
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

#endif




/*
实现 笔
（1）笔的构成： 底分型、顶分型构成一笔
（2）笔的破坏：
（3）笔的属性：
      底分型
	  顶分型
	  
*/




