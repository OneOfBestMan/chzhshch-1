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

/*
旧实现与v2的对比

（旧实现）

向上的笔，里面包含3根k线，头两根k线需要处理包含关系（high、low都取低值），与第3根k线不具有包含关系，最终，笔的high、low如下：

|
|___high
|   |
| | |
| |
|___low

不难发现，笔的high值并不是这3根k线中的最高点，这是由于对前2根k线进行了包含关系处理。

向下的笔，里面包含3根k线， 头两根k线需要处理包含关系（high、low都取高值），与第3根k线不具有包含关系，最终，笔的high、low，如图所示
_____high
|
| |
| | |
|   |_____low
|

笔的low并不是这3根k线的最低值，这是由于对前2根k线进行了包含关系处理。

（新实现）

向上的笔，这3根k线中的后2根，都包含在第1根k线中。 所以，只记住第1根k线的low和high，后2根k线都可以忽略，方向是UNSURE。

|
|
|   |
| | |
| |
|

对于第4根k线分类：

（1） 如果，依然被第1根k线包含，则方向UNSURE；继续看下一根k线

|
|     |
|   | |
| | | |
| |   |
|     |
|
（2） 如果，包含第1根k线， 则更新low、high为第4根k线的low、high，方向是UNSURE；继续看下一根k线

      |
|     |
|   | |
| | | |
| |   |
|     |
      |

（3） 如果，第4根k线的low、high都小于第1根k线的low、high，则方向是SEARCHING_BOTTOM，第1根k线构成向上笔的结束、向下笔的开始

|
|     |
|   | |
| | | |
| |   |
|     |
      |

（4） 如果，第4根k线的low、high都大于第1根k线的low、high，则方向是SEARCHING_TOP，第1根k线构成向下笔的结束、向上笔的开始

      |
|     |
|   | |
| | | |
| |   |
|

新算法，相对于旧算法，优点在于：可以更精准的识别出笔的最高点、最低点所在的k线，并且，每1笔的最高点、最低点，一定是k线中的最高、最低点；
                                旧算法在处理包含关系的时候，会导致笔的最高、最低点数值不准确。

*/
void Class_LeiBi::FenBi_Step1()
{
	container = new ContainerType();
	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();


	float possibleBot((*start).getLow()), possibleTop((*start).getHigh());
	baseItemType_Container::iterator posBotBars(start), posTopBars(start);  // pos : possible
	baseItemType_Container::iterator curBotBars(start), curTopBars(start); // cur : current

	enum { UNSURE, SEARCHING_TOP, SEARCHING_BOT } goal = UNSURE;
	typedef enum {
		// 拐点性质
		INIT = 0, //算法开始时未知状态
		IS_PEAK = 1, //峰
		IS_TROUGH = -1  //谷
	} INFLECTION_POINT;
	INFLECTION_POINT  lastInflectionPoint = INIT, firstBarIs = INIT;

	baseItemType_Container::iterator current;
	for (current = start + 1; current < end; current++)
	{
		float curLow = (*current).getLow();
		float curHigh = (*current).getHigh();

		if (goal == UNSURE)
		{
			if (curLow <= possibleBot)
			{
				if (curHigh < possibleTop)
				{
					if (lastInflectionPoint != IS_PEAK)
					{
						// 情形1： INIT 时的 UNSURE状态
						// 情形2： SEARCHING_TOP过程中，遇到包含之后的UNSURE状态

						// 找到了顶拐点， 顶拐点左侧的向上一笔确认了
						if (posTopBars > curBotBars) // 仅对于情形2
							container->push_back(ContainerType::value_type(&(*curBotBars), &(*(posTopBars)), possibleTop, (*curBotBars).getLow(), ASCENDING, posTopBars - curBotBars));
					
						curTopBars = posTopBars;
						posTopBars = start - 1;
						lastInflectionPoint = IS_PEAK;
						if (firstBarIs == INIT)
							firstBarIs = IS_PEAK;
					}
					// 情形3： SEARCHING_BOTTOM过程中，遇到包含之后的UNSURE状态
					posBotBars = current;
					possibleBot = curLow;
					possibleTop = curHigh;
					goal = SEARCHING_BOT;
				}
				else
				{
					possibleBot = curLow;
					possibleTop = curHigh;
					posTopBars = posBotBars = current;
				}
			}
			else if (curHigh > possibleTop)
			{
				if (lastInflectionPoint != IS_TROUGH)
				{
					// 情形1： INIT 时的 UNSURE状态
					// 情形2： SEARCHING_BOTTOM过程中，遇到包含之后的UNSURE状态

					// 找到了底拐点， 底拐点左侧的向下的一笔确认了
					if (posBotBars > curTopBars) // 仅对于情形2
						container->push_back(ContainerType::value_type(&(*curTopBars), &(*(posBotBars)), (*curTopBars).getHigh(), possibleBot, DESCENDING, posBotBars - curTopBars));
				
					curBotBars = posBotBars;
					posBotBars = start - 1;
					lastInflectionPoint = IS_TROUGH;
					if (firstBarIs == INIT)
						firstBarIs = IS_TROUGH;
				}
				// 情形3: SEARCHING_TOP的过程中，遇到包含之后的UNSURE状态
				posTopBars = current;
				possibleBot = curLow;
				possibleTop = curHigh;
				goal = SEARCHING_TOP;
			}
			else
				continue;
		}
		else if (goal == SEARCHING_TOP)
		{
			if (curHigh > possibleTop)
			{
				possibleBot = curLow;
				possibleTop = curHigh;
				posTopBars = current;
			}
			else if (curLow < possibleBot)
			{
				// 找到了顶拐点， 顶拐点左侧的向上的一笔确认了
				container->push_back(ContainerType::value_type(&(*curBotBars), &(*(posTopBars)), possibleTop, (*curBotBars).getLow(), ASCENDING, posTopBars - curBotBars));
			
				curTopBars = posTopBars;
				posBotBars = current;
				posTopBars = start - 1;
				possibleBot = curLow;
				possibleTop = curHigh;
				goal = SEARCHING_BOT;
				lastInflectionPoint = IS_PEAK;
			}
			else
				goal = UNSURE;
		}
		else // SEARCHING_BOTTOM
		{
			if (curLow <= possibleBot)
			{
				possibleBot = curLow;
				possibleTop = curHigh;
				posBotBars = current;
			}
			else if (curHigh > possibleTop)
			{
				// 找到了底拐点， 底拐点左侧的向下的一笔确认了
				container->push_back(ContainerType::value_type(&(*curTopBars), &(*(posBotBars)), (*curTopBars).getHigh(), possibleBot, DESCENDING, posBotBars - curTopBars));
			
				curBotBars = posBotBars;
				posBotBars = start - 1;
				posTopBars = current;
				possibleTop = curHigh;
				possibleBot = curLow;
				goal = SEARCHING_TOP;
				lastInflectionPoint = IS_TROUGH;
			}
			else
				goal = UNSURE;

		}
	}
	/*
	补全最后一个拐点右侧的1笔
	  若最后拐点 是顶拐点，则右侧一笔方向向下； 
	  若最后拐点 是底拐点，则右侧一笔方向向上；
	*/
	if (lastInflectionPoint == IS_PEAK)
	{
		container->push_back(ContainerType::value_type(&(*curTopBars), &(*(posBotBars)), (*curTopBars).getHigh()	, possibleBot, DESCENDING, posBotBars - curTopBars));
	}
	else if (lastInflectionPoint == IS_TROUGH)
	{
		container->push_back(ContainerType::value_type(&(*curBotBars), &(*(posTopBars)), possibleTop, (*curBotBars).getLow(), ASCENDING, posTopBars - curBotBars));
	}
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
		if (d == IComparable::getDirection(*former, *latter))
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
	float ZIG_PERCENT = 5;
	container = new ContainerType();

	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();

	float possibleBot((*start).getDirection() == ASCENDING ? (*start).getLow() : -1);
	float possibleTop((*start).getDirection() == DESCENDING ? (*start).getHigh() : -1);


	/* 
	
	SEARCHING_TOP 过程中 用到：curBotBars(已确认底拐点右侧的第1笔)、posTopBars（待确认顶拐点左侧的第1笔），方向都是向上的； 
	possibleTop 仅当 向下的一笔低点(curLow)跌幅超过ZIG_PERCENT时才被确认


								   (possibleTop：待确认的顶拐点)
									   /\
						/\            /  \
					   /  \          / (ZIG_PERCENT)
					  /    \  posTopBars   \
   					 /      \      /        \  /
				curBotBars   \    /          \/
				   /          \  /        (curLow) 相对于possibleTop跌幅超过ZIG_PERCENT，则possibleTop才被确认为 顶拐点
				  /            \/             
				 /
		(已确认的底拐点)

	 SEARCHING_BOTTOM 过程中 用到：curTopBars(已确认顶拐点的右侧第1笔)、posBotBars（待确认底拐点的左侧第1笔）,方向都是向下的； 
	 possibleBot 仅当 向上的一笔高点（curHigh）涨幅超过ZIG_PERCENT时才被确认


		(已确认的顶拐点) 
				  \
				   \            /\            (curHigh)相对于possibleBot涨幅超过ZIG_PERCENT，则possibleBot才被确认为 底拐点
				curTopBars     /  \            /\
					 \        /    \          /  \
					  \      /      \        /
					   \    / posBotBars    /
						\  /          \  (ZIG_PERCENT)
						 \/            \  /
										\/
									(possibleBot：待确认的底拐点)
 
		*/ 

	/*
	初始的时候，如果 第1笔 向上， 则 第1笔的起点 被假定是一个 已确认的底拐点， 该底拐点 左侧的一笔（posBotBars = start-1）方向向下（虽然这1笔不存在）;
	            如果 第1笔 向下， 则 第1笔的起点 被假定是一个 已确认的顶拐点， 该顶拐点 左侧的一笔（possibleTop = start-1）方向向上（虽然这1笔不存在）.
	*/
	baseItemType_Container::iterator posBotBars((*start).getDirection() == ASCENDING ? start-1 : start); // pos: possible
	baseItemType_Container::iterator posTopBars((*start).getDirection() == DESCENDING ? start-1 : start); // pos: possible
	baseItemType_Container::iterator curBotBars(end), curTopBars(end); // cur: current

	
	enum { INIT, SEARCHING_TOP, SEARCHING_BOT } goal = INIT;
	enum { NONE, IS_PEAK, IS_TROUGH } lastInflectionPoint = NONE;


	baseItemType_Container::iterator current; 
	for (current = start; current < end; current++)
	{
		float curHigh = (*current).getHigh();
		float curLow = (*current).getLow();
		Direction d = (*current).getDirection();

		if (goal == INIT)
		{
			if (d == ASCENDING)
			{
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh)
				{
					curBotBars = posBotBars + 1;
					posBotBars = end;
					possibleTop = curHigh;
					posTopBars = current;
					goal = SEARCHING_TOP;
					lastInflectionPoint = IS_TROUGH;

					assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);
				}
				else if (curHigh > possibleTop)
				{
					posTopBars = current;
					possibleTop = curHigh;
				}
			}
			else {
				// DESCENDING
				if (curLow*  (1 + ZIG_PERCENT / 100) <= possibleTop)  // (curLow) 相对于possibleTop跌幅超过ZIG_PERCENT
				{
					curTopBars = posTopBars + 1;
					posTopBars = end;
					possibleBot = curLow;
					posBotBars = current;
					goal = SEARCHING_BOT;
					lastInflectionPoint = IS_PEAK;

					assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);

				}
				else if (curLow < possibleBot)
				{
					posBotBars = current;
					possibleBot = curLow;
				}
			}
		}
		else if (goal == SEARCHING_TOP)  // 寻找顶拐点： 找到一个向下的类笔，类笔的低点对于当前高点转折超过  ZIG_PERCENT
		{
			if (d == ASCENDING) 
			{
				if (curHigh > possibleTop)
				{
					possibleTop = curHigh;
					posTopBars = current;
				}
			}
			else 
			{
				if (curLow * (1 + ZIG_PERCENT / 100) <= possibleTop)
				{
					// 找到 顶拐点， 顶拐点左侧的向上笔 被确认
					container->push_back(ContainerType::value_type(&(*curBotBars), &(*posTopBars)));
					assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);

					curTopBars = posTopBars + 1;
					posTopBars = start -1;
					posBotBars = current;
					possibleBot = curLow;
					goal = SEARCHING_BOT;
					lastInflectionPoint = IS_PEAK;
				}
			}
		}
		else // SEARCHING_BOT
		{
			if (d == DESCENDING)
			{
				if (curLow < possibleBot)
				{
					possibleBot = curLow;
					posBotBars = current;
				}
			}
			else 
			{
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh) // (curHigh)相对于possibleBot涨幅超过ZIG_PERCENT
				{
					// 找到 底拐点， 底拐点左侧的向下笔 被确认
					container->push_back(ContainerType::value_type(&(*curTopBars), &(*posBotBars)));
					assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);

					curBotBars = posBotBars + 1;
					posBotBars = start-1;
					posTopBars = current;
					possibleTop = curHigh;
					goal = SEARCHING_TOP;
					lastInflectionPoint = IS_TROUGH;
				}
			}
		}
	}

	/*
	添加 最后一个已确认拐点右侧的1笔， 即： 最后一个已确认拐点 和 确认该拐点的关键点(critical point)  
	*/
	if (lastInflectionPoint == IS_PEAK) {
		container->push_back(ContainerType::value_type(&(*curTopBars), &(*posBotBars)));
		assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);
	}
	else if (lastInflectionPoint == IS_TROUGH) {
		container->push_back(ContainerType::value_type(&(*curBotBars), &(*posTopBars)));
		assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);
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




