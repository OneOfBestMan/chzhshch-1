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


void Class_LeiBi::FenBi_Step1()
{
	Direction d = ASCENDING; // ���ʼ�ĵ�1��2��k�ߣ����Ǽ���֮ǰ�ķ�����ascending�ģ��������㴦�������ϵ��

	ContainerType* intermidiate = new ContainerType();

	baseItemType_Container::iterator start = base_Container->begin();
	baseItemType_Container::iterator end = base_Container->end();

	Class_KXian temp = *start;

	int KXianCnt = 1; // һ���� �ޡ�������ϵ����K�ߵ������� �����͡��׷��͵�K�ߣ������ڣ� һ�ʵ�KXianCnt��Ӧ�ô��ڵ���5
	baseItemType_Container::iterator p = start;

	
	/*
	ע��: һ���ʵĸߡ��ͼ�λ�������Ǽ򵥵�ͳ�Ƹñ�������K�ߵ����ֵ�����ֵ�����뿼�ǰ�����ϵ��Ʃ�磺
          |
          |___high
      |   |
      | | |
        | |
          |___low

      ����ʵķ��������µģ��������3��k�ߣ� �ʵ�high��low����ͼ��ʾ�����ѷ��֣��ʵ�highֵ�����ǵ�3��k�ߵ���ߵ㣬�������ں�2��k�ߣ������˰�����ϵ����
	  ��Ʃ�磺����ıʷ������ϣ����ʵ�low����������Ǹ�k�ߵ����ֵ���������ں�2��k�ߣ������˰�����ϵ����.
           _____high
          |
        | |
      | | |
      |   |_____low
          |
	*/
	float high = start->getHigh(); // ͳ������� �� high
	float low = start->getLow();  // ͳ�� ����� �� low

	do
	{
		while (p != end && temp == *p) // == ��ʾ��������
		{
			temp.merge(*p, d);
			p++;
		}
		high = max(high, temp.getHigh());
		low = min(low, temp.getLow());

		if (p == end)
		{
			// TODO: ��������һ��  ��-��
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
				/* KXianCnt ��1��ֻ���ܷ������ʼ ����K�ߣ����ڰ�����ϵ�� ���磺

                     |
                   | |   |
                   | | | |
                   | | | |
                     | |
                     |

				  ���ڣ��Ҽٶ�����ʼ�ķ�����acscending�����ԣ��ڴ����˵�1����2 k�߰�����ϵ�󣬷��ֵ�3��k�� ��֮ �����෴����ʱ��KXianCnt��1�����԰�ascending��Ϊdescending��
				  ����һ�Σ����ǣ��ڴ����˵�1��2��3k�ߵİ�����ϵ�󣬵�4��k�����������ϵk���෴����ʱKXianCnt��Ȼ��1�����ԣ���ʼ�����⼸��������ϵk�ߣ��Ȳ���˵�����ϡ�Ҳ����˵�����£�
				  ���ԣ�����������������Ҷԡ���ʡ������� 2�� û�а�����ϵ��K�߹��ɵ���ʶ�� ����ķ�ʽ�ǣ� �����1��k�ߣ�֮ǰ��һ�����޵�k�ߣ�����һ����ʡ�
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
	// ��ʣ���K������1�����
	if (start != end -  1)
		intermidiate->push_back(ContainerType::value_type(&(*start), &(*(p-1)), high, low, d, KXianCnt));


	/* ��intermidiate�еĸ�����ʣ�����������������Σ��� ��3����ʣ�����KXianCnt����2�� ���� ��1����3 ��� ͬ�������� ����3����ʣ��ϲ���Ϊ1����ʣ��ܱ�����4

      �� 
            ��
          ��   
                 ��
       
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
			�������߶Σ�ǰ����merge֮��Ӧ���ǰ���dfȡ former�����෴�ķ�ʽ��merge
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
	// ����ʣ������
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
		case 1����1��ʡ���3��� ͬ�� �� �����1�����2�����3������  1��
        ��
          ��      ��
            ��  ��  ��
              ��      ��
                        ��

		case 2����1��ʡ���3��� ���ǰ�� �򿼲죨1�����1 �Ƿ�� ǰ����� ����1�ʣ�2�����1�����2 �Ƿ���Ժ�ǰһ��� ����1�ʡ����췽���ǣ�
		���� ɨ�裬 ���������� �� ����ʱ�� ֹͣ�� ���ͬ����ô�����ҵ��� 1�ʣ������������ǰ����ɨ�衣
                       ��
         ��          ��  ��
           ��      ��      ��
             ��  ��          ��
               ��              ��
                                 ��
		case 3����1��ʡ���3��� ǰ������ǰɨ�裬 ��� ���ǰ �� ���� ֹͣ��  ���ǰ���� ���� ͬ�� ��ǰɨ�衣
         ��
           ��          ��
             ��      ��  ��
               ��  ��      ��
                 ��
		case 4����1��ʡ���3��� �����򿼲죺 ��1����2��� ����ɨ�裻  ��1��� ����ɨ�衣
                      ��
        ��          ��  ��
          ��      ��      ��
            ��  ��
              ��

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




