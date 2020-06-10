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
			// ���� K�� vector
			Class_KXian::initialize();
			base_Container = Class_KXian::container;
		}
		if (base_Container && !container)
		{
			// step 1: ����K�߰�����ϵ���ҳ� ��-�����͡���-�׷��ͣ����ǣ���û�п��ǣ������͡��׷���֮�� �� 5��K�ߵ�Ҫ��
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
��ʵ����v2�ĶԱ�

����ʵ�֣�

���ϵıʣ��������3��k�ߣ�ͷ����k����Ҫ���������ϵ��high��low��ȡ��ֵ�������3��k�߲����а�����ϵ�����գ��ʵ�high��low���£�

|
|___high
|   |
| | |
| |
|___low

���ѷ��֣��ʵ�highֵ��������3��k���е���ߵ㣬�������ڶ�ǰ2��k�߽����˰�����ϵ����

���µıʣ��������3��k�ߣ� ͷ����k����Ҫ���������ϵ��high��low��ȡ��ֵ�������3��k�߲����а�����ϵ�����գ��ʵ�high��low����ͼ��ʾ
_____high
|
| |
| | |
|   |_____low
|

�ʵ�low��������3��k�ߵ����ֵ���������ڶ�ǰ2��k�߽����˰�����ϵ����

����ʵ�֣�

���ϵıʣ���3��k���еĺ�2�����������ڵ�1��k���С� ���ԣ�ֻ��ס��1��k�ߵ�low��high����2��k�߶����Ժ��ԣ�������UNSURE��

|
|
|   |
| | |
| |
|

���ڵ�4��k�߷��ࣺ

��1�� �������Ȼ����1��k�߰���������UNSURE����������һ��k��

|
|     |
|   | |
| | | |
| |   |
|     |
|
��2�� �����������1��k�ߣ� �����low��highΪ��4��k�ߵ�low��high��������UNSURE����������һ��k��

      |
|     |
|   | |
| | | |
| |   |
|     |
      |

��3�� �������4��k�ߵ�low��high��С�ڵ�1��k�ߵ�low��high��������SEARCHING_BOTTOM����1��k�߹������ϱʵĽ��������±ʵĿ�ʼ

|
|     |
|   | |
| | | |
| |   |
|     |
      |

��4�� �������4��k�ߵ�low��high�����ڵ�1��k�ߵ�low��high��������SEARCHING_TOP����1��k�߹������±ʵĽ��������ϱʵĿ�ʼ

      |
|     |
|   | |
| | | |
| |   |
|

���㷨������ھ��㷨���ŵ����ڣ����Ը���׼��ʶ����ʵ���ߵ㡢��͵����ڵ�k�ߣ����ң�ÿ1�ʵ���ߵ㡢��͵㣬һ����k���е���ߡ���͵㣻
                                ���㷨�ڴ��������ϵ��ʱ�򣬻ᵼ�±ʵ���ߡ���͵���ֵ��׼ȷ��

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
		// �յ�����
		INIT = 0, //�㷨��ʼʱδ֪״̬
		IS_PEAK = 1, //��
		IS_TROUGH = -1  //��
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
						// ����1�� INIT ʱ�� UNSURE״̬
						// ����2�� SEARCHING_TOP�����У���������֮���UNSURE״̬

						// �ҵ��˶��յ㣬 ���յ���������һ��ȷ����
						if (posTopBars > curBotBars) // ����������2
							container->push_back(ContainerType::value_type(&(*curBotBars), &(*(posTopBars)), possibleTop, (*curBotBars).getLow(), ASCENDING, posTopBars - curBotBars));
					
						curTopBars = posTopBars;
						posTopBars = start - 1;
						lastInflectionPoint = IS_PEAK;
						if (firstBarIs == INIT)
							firstBarIs = IS_PEAK;
					}
					// ����3�� SEARCHING_BOTTOM�����У���������֮���UNSURE״̬
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
					// ����1�� INIT ʱ�� UNSURE״̬
					// ����2�� SEARCHING_BOTTOM�����У���������֮���UNSURE״̬

					// �ҵ��˵׹յ㣬 �׹յ��������µ�һ��ȷ����
					if (posBotBars > curTopBars) // ����������2
						container->push_back(ContainerType::value_type(&(*curTopBars), &(*(posBotBars)), (*curTopBars).getHigh(), possibleBot, DESCENDING, posBotBars - curTopBars));
				
					curBotBars = posBotBars;
					posBotBars = start - 1;
					lastInflectionPoint = IS_TROUGH;
					if (firstBarIs == INIT)
						firstBarIs = IS_TROUGH;
				}
				// ����3: SEARCHING_TOP�Ĺ����У���������֮���UNSURE״̬
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
				// �ҵ��˶��յ㣬 ���յ��������ϵ�һ��ȷ����
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
				// �ҵ��˵׹յ㣬 �׹յ��������µ�һ��ȷ����
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
	��ȫ���һ���յ��Ҳ��1��
	  �����յ� �Ƕ��յ㣬���Ҳ�һ�ʷ������£� 
	  �����յ� �ǵ׹յ㣬���Ҳ�һ�ʷ������ϣ�
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
			// step 2: ��϶����͡��׷���֮�䣬����5��k�ߵ�Ҫ�����ɸ����ʣ�
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
		// �� cmpTo ����
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
	
	SEARCHING_TOP ������ �õ���curBotBars(��ȷ�ϵ׹յ��Ҳ�ĵ�1��)��posTopBars����ȷ�϶��յ����ĵ�1�ʣ������������ϵģ� 
	possibleTop ���� ���µ�һ�ʵ͵�(curLow)��������ZIG_PERCENTʱ�ű�ȷ��


								   (possibleTop����ȷ�ϵĶ��յ�)
									   /\
						/\            /  \
					   /  \          / (ZIG_PERCENT)
					  /    \  posTopBars   \
   					 /      \      /        \  /
				curBotBars   \    /          \/
				   /          \  /        (curLow) �����possibleTop��������ZIG_PERCENT����possibleTop�ű�ȷ��Ϊ ���յ�
				  /            \/             
				 /
		(��ȷ�ϵĵ׹յ�)

	 SEARCHING_BOTTOM ������ �õ���curTopBars(��ȷ�϶��յ���Ҳ��1��)��posBotBars����ȷ�ϵ׹յ������1�ʣ�,���������µģ� 
	 possibleBot ���� ���ϵ�һ�ʸߵ㣨curHigh���Ƿ�����ZIG_PERCENTʱ�ű�ȷ��


		(��ȷ�ϵĶ��յ�) 
				  \
				   \            /\            (curHigh)�����possibleBot�Ƿ�����ZIG_PERCENT����possibleBot�ű�ȷ��Ϊ �׹յ�
				curTopBars     /  \            /\
					 \        /    \          /  \
					  \      /      \        /
					   \    / posBotBars    /
						\  /          \  (ZIG_PERCENT)
						 \/            \  /
										\/
									(possibleBot����ȷ�ϵĵ׹յ�)
 
		*/ 

	/*
	��ʼ��ʱ����� ��1�� ���ϣ� �� ��1�ʵ���� ���ٶ���һ�� ��ȷ�ϵĵ׹յ㣬 �õ׹յ� ����һ�ʣ�posBotBars = start-1���������£���Ȼ��1�ʲ����ڣ�;
	            ��� ��1�� ���£� �� ��1�ʵ���� ���ٶ���һ�� ��ȷ�ϵĶ��յ㣬 �ö��յ� ����һ�ʣ�possibleTop = start-1���������ϣ���Ȼ��1�ʲ����ڣ�.
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
				if (curLow*  (1 + ZIG_PERCENT / 100) <= possibleTop)  // (curLow) �����possibleTop��������ZIG_PERCENT
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
		else if (goal == SEARCHING_TOP)  // Ѱ�Ҷ��յ㣺 �ҵ�һ�����µ���ʣ���ʵĵ͵���ڵ�ǰ�ߵ�ת�۳���  ZIG_PERCENT
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
					// �ҵ� ���յ㣬 ���յ��������ϱ� ��ȷ��
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
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh) // (curHigh)�����possibleBot�Ƿ�����ZIG_PERCENT
				{
					// �ҵ� �׹յ㣬 �׹յ��������±� ��ȷ��
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
	��� ���һ����ȷ�Ϲյ��Ҳ��1�ʣ� ���� ���һ����ȷ�Ϲյ� �� ȷ�ϸùյ�Ĺؼ���(critical point)  
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
	// һ���ʵ������˵㣬�������point; ����˵㣬�Ƕ� ���� �ף���flags��ǣ� ��������׷��͵�ֵ����val����
	typedef struct stackItem {
		baseItemType* point;
		FLAGS flag; 
		float val;

		stackItem(baseItemType* p, FLAGS f, float v) {point =p; flag = f; val = v;}
		stackItem() {point = NULL; flag = (FLAGS)0; val = 0; }
		bool is_top() {return flag & TOP;}
		bool is_bot() {return flag & BOTTOM;}
		bool is_start() {return is_top() && is_bot(); /*��㣬���Ե��ɶ����*/}
	} stackItem;
	vector<stackItem> analyzeStack;

	if (intermediate == NULL) return;

	ContainerType::iterator current = intermediate->begin(); // ��ǰ ����ʡ��� ����ʡ� ��Ҫ�����㶥����֮������5������������K�ߣ�
	ContainerType::iterator end = intermediate->end();

	// ĳ����Ʊ�����е�һ��Ŀ��̼۸���Ϊ��һ���ʵ� �׶˵�ļ۸�
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
						// lastBot��start point
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

	// ���� analyzeStack �еĸ��� �����׷��ͣ� ���� ���б�
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
ʵ�� ��
��1���ʵĹ��ɣ� �׷��͡������͹���һ��
��2���ʵ��ƻ���
��3���ʵ����ԣ�
      �׷���
	  ������
	  
*/




