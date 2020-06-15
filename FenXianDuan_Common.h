

class CharacterVec: public IComparable
	{
	public:
		// ���������� �������������ֻ��1�ʣ���ôstart��endָ��ͬһ���ʣ����򣬷ֱ�ָ���һ�����һ�ʣ�
		baseItemIterator start, end;

		float getHigh() const {return High;}
		float getLow() const {return Low;}
		void setHigh(float h) {High = h;}
		void setLow(float l) {Low = l;}

		CharacterVec(baseItemIterator s, baseItemIterator e) 
		{
			assert((*s).getDirection() == (*e).getDirection());
			start = s; 
			end = e;
			d = (*s).getDirection();
			
			High = (*s).getHigh();
			Low = (*s).getLow();

			while (s != e)
			{
				s += 2;
				merge(*s, -d);
			}
		}

		CharacterVec(baseItemIterator e) : CharacterVec(e, e) 
		{
		}

		void updateEnd(baseItemIterator e)
		{
			assert((*start).getDirection() == (*e).getDirection());

			High = (*start).getHigh();
			Low = (*start).getLow();
			end = e;

			baseItemIterator s = start;
			while (s != e)
			{
				s += 2;
				merge(*s, -d);
			}
		}

	private:
		Direction d;
		float High, Low;	
	};

	typedef vector<CharacterVec> analyzeStack;

	static baseItemIterator FXD_Merge(Direction hints, const baseItemIterator & start, const baseItemIterator & end, baseItemIterator &prevCharacVecEnd)
	{
		assert(hints != ENCLOSING);

		baseItemIterator current = start;
		Direction d;
	
		/* �Ⱥϲ������ܵģ����߶ο�ʼ���İ�����ǰ���󣩵ļ��� */
		baseItemType possiblePrevXianDuanChracVec = *current;
		while (current < end - 2)
		{
			baseItemType& suspect = *(current + 2);
			if (possiblePrevXianDuanChracVec >> suspect)
			{
				possiblePrevXianDuanChracVec = suspect;
				current += 2;
				continue;
			}
			else
				break;
		}

		prevCharacVecEnd = current;

		/* �ҿ��ܳ���ת�۵�λ�� */
		baseItemType lastBi = possiblePrevXianDuanChracVec;
		while (current < end - 2)
		{
			baseItemType &suspect = *(current + 2);
			d = IComparable::getDirection(lastBi, suspect);
			if (d == hints || (lastBi << suspect && !(*start << suspect)))
			{
				lastBi = suspect;
				current += 2;
				continue;
			}
			else
				break;
		}

		return current;
	}

	static bool FXD_Case1(Direction hints, baseItemIterator &start, const baseItemIterator &end, analyzeStack& lastXianDuan_CharacVecStack)
	{
		baseItemIterator prevChacVecEnd;  
		baseItemIterator possibleNewXianDuanEnd = FXD_Merge(hints, start, end, prevChacVecEnd);

		
		/*  ���Ϻϲ�������ϵ�õ�����һ�ʣ� �����ܳ���ת�۵ıʣ�һ�������٣�3�ʣ���ô���߶ξ������������3��ȷ���߶�*/
		if (possibleNewXianDuanEnd - prevChacVecEnd >= 2) 
			return true;
		else
		{
			/* ���򣬴���ϲ���ϵ�õ�����һ�ʣ�������ԭ�߶ε�����������ԭ�߶���Ȼ���� */
			lastXianDuan_CharacVecStack.push_back(CharacterVec(start, prevChacVecEnd));
			return false;
		}
	}


	static void Normalize(Direction hints, baseItemIterator start, baseItemIterator end)
	{
		if (hints == (*start).getDirection()) return;

		baseItemIterator current = start;

		/* �����߶η����ȡ�����ȴ���������������߶� */
		while (current != end)
		{
			(*current).d = -(*current).d;

			current++;
			if (current == end)
				break;
			else
				current++;
		}

		/* ���������ż�����߶Σ����˷���ȡ��������Ҫ����ߡ��͵� */
		current = start + 1;
		while (current < end - 1) // ������������д�����ģ� ����ͼ��BC��CD��Ҫ����BC��ʱ�򣬺������ٻ���һ��CD���Ŵ���BC����������CD�������end����ô���� BC < end - 1��
		{
			if ((*current).getDirection() == DESCENDING)
			{
/*
              B                       A
             /\                        \
            /  \                        \
           /    \         D              \      C
          /      \        /               \    /\
         /        \      /     ===>        \  /  \
        /          \    /                   \/    \
       A            \  /                    B      \
                     \/                             \
                     C                               D
*/
				(*current).Low = (*(current - 1)).getLow();
				(*current).High = (*(current + 1)).getHigh();
			}
			else
			{
/*
                    C                            D
                   /\                           /
                  /  \                         /
        A        /    \                  B    /
         \      /      \      ===>       /\  /
          \    /        \               /  \/
           \  /         D              /    C
            \/                        /
            B                        A

*/
				assert((*current).getDirection() == ASCENDING);
				(*current).High = (*(current - 1)).getHigh();
				(*current).Low = (*(current + 1)).getLow();
			}
			(*current).d = -(*current).getDirection();

			current += 2;
		}
	}

	static void NormalizeV2(baseItemIterator start, baseItemIterator end)
	{
		/* ���������������������߶Σ����ӵ㴦�����ֵ�����ֵ ����ȵ�������������������ԭ������Ϊ���߶ε����ֵ�����ֵ�������߶��м�ĳ��K�ߣ������ǳ��������ӵ㴦 */
		baseItemIterator curr = start;

		while (curr != end - 1)
		{
			baseItemType &former = *curr;
			baseItemType &latter = *(curr + 1);


			if (former.getDirection() == ASCENDING)
			{
				latter.High = former.High = max(former.getHigh(), latter.getHigh());
			}
			else
			{
				latter.Low = former.Low = min(former.getLow(), latter.getLow());
			}
			curr++;
		}
	}

	static ContainerType* startFenXianDuan(baseItemIterator start, baseItemIterator end)
	{
		NormalizeV2(start, end);

		baseItemType_Container* backupBeforeNormalize = (baseItemType_Container*)NULL;
		ContainerType* resultSet = (ContainerType*)NULL;

		baseItemIterator biStart = start;
		baseItemIterator biFormer = start;
		Direction d = ENCLOSING;
		while (end - biFormer > 2)
		{
			d = IComparable::getDirection(*biFormer, *(biFormer + 2));
			Direction dXian = (*biFormer).getDirection();
			if (d == ENCLOSING || d == -dXian) // d == -dXian  ��Ϊ�˹��normalize
			{
				biFormer += 1;
				continue;
			}
			else
				break;
		}

		if (end - biFormer <= 2) return resultSet;

		if (d == -(*biFormer).getDirection())
		{
			assert(0); // normalize Ӧ�ò����ٱ������ˣ�����ǰ����� d == -dXian����
			backupBeforeNormalize = new baseItemType_Container();
			backupBeforeNormalize->assign(start,end);

			Normalize(d, biFormer, end);
		}

		analyzeStack CharacVecStack;

		baseItemIterator biLatter = biFormer + 2;

#ifdef DEBUG
		// ��δ���������������ѭ���ġ� 
		int debugCnt = 0;
		baseItemIterator oldBiFormer = biFormer;
		baseItemIterator oldBiLatter = biLatter;
		int count = 0;
#endif

		do 
		{
			/*���߶��У�Ѱ�ҿ��ܳ���ת�۵���һ�ʵ�λ�ã� ������ǰ���󡢷�����ԭ�߶��෴�������������ǰ����Ϊ���¸߻��µͣ�*/
			while  (biFormer < end - 2 &&  (IComparable::getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter) || (*biFormer >> *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biLatter - 1));
				biFormer = biLatter;

				if (biLatter < end - 2)
				{
					biLatter += 2;
				}
				else
				{
					/* baseItem�쵽����� */
					if (!resultSet)
					{
						resultSet = new ContainerType();
					}
					resultSet->push_back(XianDuanClass(biStart, biLatter, d));
#ifdef DEBUG
					debugCnt++;
#endif
				}
			}

			if (biFormer >= end - 2)
			{
				break;
			}

			if (FXD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* ԭ�߶�����*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				
				
#ifdef DEBUG
				// ��δ���������������ѭ���ġ� 
				if (((*biFormer) == (*oldBiFormer)) && ((*biLatter) == (*oldBiLatter)))
				{
					count++;
				}
				else
				{
					count = 0;
					oldBiFormer = biFormer;
					oldBiLatter = biLatter;
				}

				if (count == 4)
					printf("break me here\n");

				if (count == 5)
					break;
#endif

				if (biLatter >= end)  break;

				if (IComparable::getDirection(*biFormer, *biLatter) == d || (*biFormer >> *biLatter))
				{
					/*
                    ����1�� biFormer �� biLatter֮��ķ��� �� ԭ�߶η�����ͬ

                                                    /
                               /\                  /
                              /  \          /\  biLatter
                       /\ biFormer\        /  \  /
                      /  \  /      \      /    \/
                     /    \/        \    /
                    /                \  /        
                                      \/
                   |<  ԭ�߶� >|<-  �������� ->|<- ԭ�߶����� ->|

                   ����2�� biFormer ���� biLatter

                                 /\
                                /  \                  /\
                       /\      /    \        /\      /  \  /
                      /  \ biFormer  \      /  \ biLatter\/
                     /    \  /        \    /    \  /
                    /      \/          \  /      \/
                   /                    \/
                   
                   |<   ԭ�߶�  >|<-  �������� ->|<- ���߶� ->|
                                     
            
					*/
					if (biLatter < end - 2)
					{
						CharacVecStack.pop_back();
						
#if 0						
						if (*biFormer >> *biLatter)
						{
							biFormer = biLatter;
							biLatter += 2;
						}
#endif
						continue;
					}
					else
					{
						/* baseItem�쵽����� */
						if (!resultSet)
						{
							resultSet = new ContainerType();
						}
						resultSet->push_back(XianDuanClass(biStart, biFormer, d));
#ifdef DEBUG
						debugCnt++;
#endif
						break;
					}

				} else
				{
					/*
                    ����3�� biFormer �� biLatter֮��ķ��� �� ԭ�߶η����෴

                               /\                          /
                              /  \                  /\    /
                       /\ biFormer\        /\      /  \  /
                      /  \  /      \      /  \ biLatter\/
                     /    \/        \    /    \  /
                    /                \  /      \/
                                      \/
                   |<  ԭ�߶� >|<-  �������� ->|<- ���߶� ->|
                               |   �γ����߶�  |


                   ����4�� biFormer �� biLatter ����

                                                      /\
                               /\                    /  \  /
                              /  \                  /    \/
                       /\ biFormer\        /\   biLatter
                      /  \  /      \      /  \    /
                     /    \/        \    /    \  /
                    /                \  /      \/
                                      \/
                   |<  ԭ�߶� >|<-  �������� ->|<- ԭ�߶����� ->|
                               |   �γ����߶�  |


					*/

					// ���������� ��Ϊ һ���µ��߶�
					if (!resultSet)
					{
						resultSet = new ContainerType();
					}
					// ԭ�߶�
					resultSet->push_back(XianDuanClass(biStart, biFormer, d));
					// ���������γ����߶�
					resultSet->push_back(XianDuanClass(biFormer + 1, biLatter - 1, -d));
					// ���߶�
					CharacVecStack.clear();
					biFormer = biStart = biLatter;
#if 0
					/* �µ��߶Σ���ʼ�Ĳ��֣�������һ�黥��������߶� */
					baseItemType temp = *biFormer;
					while (biFormer < end - 2)
					{
						if (temp >> *(biFormer + 2))
						{
							temp = *(biFormer + 2);
							biFormer += 2;
							continue;
						}
						else
							break;
					}
					if (biFormer < end - 2)
						biLatter = biFormer + 2;
#endif
					if (biLatter < end - 2)
						biLatter += 2;

				}

			} else
			{
				/* ԭ�߶α��ƻ��������߶���ӵ�container�� */
				if (!resultSet)
				{
					resultSet = new ContainerType();
				}
				resultSet->push_back(XianDuanClass(biStart, biFormer, d));

				int debugEdgeCnt = biFormer - biStart + 1;

#ifdef DEBUG
				debugCnt++;
				if (debugCnt == 67)
					printf("break me here\n");
#endif

				/* ���߶���Ҫ ��ת���� */
				d = -d;
				/* �µ��߶εĵ�һ��*/
				biStart = ++ biFormer;
				/* �µ��߶ε���������ջ��ʼ�� */
				CharacVecStack.clear();
#if 0
				/* �µ��߶Σ���ʼ�Ĳ��֣���һ�黥��������߶Σ����Կ�����֮ǰ�߶ε���������������Ӧ�ð�����ϵ���ϲ����� */
				baseItemType temp = *biFormer;
				while (biFormer < end - 2)
				{
					if (temp >> *(biFormer + 2))
					{
						temp = *(biFormer + 2);
						biFormer += 2;
						continue;
					}
					else
						break;
				}
				biLatter = biFormer + 2;
#endif
				if (biFormer < end - 2)
					biLatter = biFormer + 2;
			}
		} while (1);

		if (backupBeforeNormalize)
		{
			XianDuanClass::baseItems->assign(backupBeforeNormalize->begin(), backupBeforeNormalize->end());
			delete backupBeforeNormalize;
		}

		return resultSet;
	}

	typedef enum {IS_CANCELED_TROUGH=-2,  IS_TROUGH = -1, NON = 0, IS_PEAK = 1 , IS_CANCELED_PEAK=2}  InflectionPoint;

	static void findFenXing(InflectionPoint  *map, baseItemIterator veryStart, baseItemIterator start, baseItemIterator end, bool findPeak)
	{
		/* ��start��end֮�䣬Ѱ�ҷ��͹յ㣬���Ҹ���map�� �����յ��Ӧ��map���±꣬ͨ����veryStart���ֵ�������㡣
		   ZIG�㷨��֤:  start��end��Ȼ��Ӧ�Ŷ����ߵף�����start��end������ͬ����start������1�ʣ���start��ʼ�ڵף�����start��ʼ�ڶ�����end������1�ʣ�������ڶ�����������ڵס�

		   ���裺�ʼ�㵽�յ�1���յ�1���յ�2�ķ��Ⱦ�������ZIG���յ�2���յ�5���ȣ�����ZIG���յ�5���յ�8�ĵ���Ҳ����ZIG��
                                               �յ�5   		
                              �յ�3          /\          �յ�7
        �յ�1                  /\      end   \        /\
              /\                /    \    /         \    /    \
            /    \            /        \/             \/        \
  veryStart   \      start   �յ�4      �յ�6         \
        /            \    /                                           \
 �ʼ��         \/                                            �յ�8
                   �յ�2                      
      |--������---|--- ZIG����1 ---|--  ZIG����2 --|
		*/
		assert((*start).getDirection() == (*end).getDirection());
		map[start - veryStart] = ((*start).getDirection() == ASCENDING ? IS_TROUGH : IS_PEAK);
		map[end - veryStart + 1] = ((*end).getDirection() == ASCENDING ? IS_PEAK : IS_TROUGH);

		baseItemIterator current;
		/*
		�ٶ� Ѱ�ҵ׷��ͣ� start�������¡�  left��mid��right�ֱ�������С��� 3����������������
      �յ�1
          \                                         �յ�6
            \        �յ�3      �յ�4        /\
        start        /\             /\    right  \
                \  left  \         /    \    /        \
                  \/        \   mid     \/            \
              �յ�2         \/          �յ�5       end
                               �յ�4                       \ 
                          �� �׷��͹յ㣩               \
                                                               �յ�7
		*/

		if (findPeak)
		{
			current = (*start).getDirection() == ASCENDING ? start + 1 : start;
		}
		else
		{
			current = (*start).getDirection() == ASCENDING ? start : start + 1;
		}
		while (current < end)
		{
			CharacterVec  leftCharacterVec(current);   // ���͵�����������

			//���������������������á�ǰ���󡱺ϲ�
			while ((current + 2) < end &&  leftCharacterVec >> *(current + 2))
			{
				leftCharacterVec.updateEnd(current + 2);
				current += 2;
			}

			current += 2;
			if (current > end) 
				break;

			CharacterVec midCharacterVec(current);

			if (findPeak)
			{
				if (leftCharacterVec > midCharacterVec)// || leftCharacterVec << midCharacterVec)   // �ο����۵�81�����Ǹ����ӣ������˸��ϸ��Ҫ�� ���ǰ ���㡣
				{
					current = midCharacterVec.start;
					continue;
				}
			}
			else
			{
				if (leftCharacterVec < midCharacterVec)// || leftCharacterVec << midCharacterVec) // �ο����۵�81�����Ǹ����ӣ������˸��ϸ��Ҫ�� ���ǰ ���㡣
				{
					current = midCharacterVec.start;
					continue;
				}

			}
			assert(findPeak ? (leftCharacterVec.getHigh() <= midCharacterVec.getHigh()) : (leftCharacterVec.getLow() >= midCharacterVec.getLow()));

			//�����Ҳ��������������á�ǰ����"�ϲ�
			while ((current + 2) < end && midCharacterVec >> *(current + 2))
			{
				midCharacterVec.updateEnd(current + 2);
				current += 2;
			}

			current += 2;
			if (current > end)
				break;

			CharacterVec rightCharacterVec(current);
			if (findPeak)
			{
				if (midCharacterVec > rightCharacterVec)
				{
					// �ҵ� ������
					map[midCharacterVec.start - veryStart] = IS_PEAK;
				}
				else
				{
					assert(midCharacterVec << rightCharacterVec || midCharacterVec < rightCharacterVec);
					current = midCharacterVec.start;
				}
			}
			else
			{
				if (midCharacterVec < rightCharacterVec)
				{
					// �ҵ� �׷���
					map[midCharacterVec.start - veryStart] = IS_TROUGH;
				}
				else
				{
					assert(midCharacterVec << rightCharacterVec || midCharacterVec > rightCharacterVec);
					current = midCharacterVec.start;
				}
			}
		}
	}

	static bool getPrevPoint(InflectionPoint  *map, int start, int &last) {
		if (last <= start)
			return false;

		do {
			last--;
			if (map[last] == IS_PEAK || map[last] == IS_TROUGH)
				return true;
		} while (last > start);

		return false;
	}

	static bool getNextPoint(InflectionPoint  *map, int end, int &next) {
		/*
		������һ����Ч�Ĺյ㡣
		�������end���򷵻ؼ�*/
		if (next >= end)
			return false;

		do {
			next++;
			if (map[next] == IS_PEAK || map[next] == IS_TROUGH)
				return true;
		} while (next < end);

		return false;
	}

	static void cancelPoint(InflectionPoint  *map, int idx)
	{
		assert(map[idx] == IS_PEAK || map[idx] == IS_TROUGH);
		map[idx] == IS_PEAK ? map[idx] = IS_CANCELED_PEAK : map[idx] = IS_CANCELED_TROUGH;
	}

	static void handleConflict(InflectionPoint  *map, baseItemIterator veryStart, baseItemIterator start, baseItemIterator end)
	{
		/*
		��ͻ1��
		����������ף�������

		                      ��                             ��
                                 /\                           /\
                               /    \        /\            /    \
              /\            /        \    /    \        /        \    /\
            /    \        /            \/        \    /            \/    \
          /        \    /                           \/                       \
        /            \/                                                         \ 
      /
		��ͻ2��
		���ڶ�����֮�䲻��3�ʣ�
		                                ��
                                          /\
                                        /    \                   /\
                   /\                /        \        /\    /    \
        \        /    \            /            \    /   \/         \ 
          \    /        \        /                \/                   \
            \/            \    /                                          \
                             \/ 
                           ��
		*/


		int s = start - veryStart;
		int e = end + 1 - veryStart;


		bool changed; 
		do
		{
			changed = false;

			int last = s, current1 = s, current2; // 3���ڵ�Ĺ������ڣ� current1��current2 �ǵ�ǰ��������Ľڵ�

			if (!getNextPoint(map, e, current1))
				return;

			current2 = current1;
			if (!getNextPoint(map, e, current2))
				return;

			if (current1 - s < 3 || map[s] == map[current1])
			{
				cancelPoint(map, current1);
				changed = true;
				continue;
			}

		
			while (current2 <= e)
			{
				if (current2 - current1 < 3) // ���ڶ�����֮�䲻��3��
				{
					int next = current2;
					if (getNextPoint(map, e, next))
					{
						
						if (map[last] == map[current1])
						{
							// ���current1 �� lastͬ���ͣ����� current1 û�б�last�����ˣ���������͡���������ߣ�����current1����; ���� ��last����
							if (map[current1] == IS_TROUGH ? (*(veryStart + current1)).getLow() >= (*(veryStart + last)).getLow() : (*(veryStart + current1)).getHigh() <= (*(veryStart + last)).getHigh())
							{
								cancelPoint(map, current1);   // Ʃ�磺 ��1(last) -��2(current1) - ��(current2)���ҵ�1 ���� ��2
								changed = true;
								current1 = current2;
								if (!getNextPoint(map, e, current2))
									return;
							}
							else
							{
								cancelPoint(map, last); // Ʃ�磺 ��1(last) -��2(current1) - ��(current2)���ҵ�1 ���� ��2�� last�϶�������start,��ΪZIG�㷨��֤��ߡ���͵㶼��������β����1���ڵ�2�����Ե�1������͵㣬Ҳ�Ͳ�����start��
								changed = true;
								last = current1;
								current1 = current2;
								if (!getNextPoint(map, e, current2))
									return;
							}
						}
						else if (map[current2] == map[next])
						{
							if (map[current2] == IS_TROUGH ? (*(veryStart + current2)).getLow() >= (*(veryStart + next)).getLow() : (*(veryStart + current2)).getHigh() <= (*(veryStart + next)).getHigh())
							{
								cancelPoint(map, current2);  // Ʃ�磺 ��1(last) - ��1(current1) - ��2(current2) - ��3���Ҷ�3 ���ڶ�2
								changed = true;
								last = current1;
								current1 = current2;
								if (!getNextPoint(map, e, current2))
									return;
							}
							else
							{
								/* Ʃ�磺 ��1(last) - ��1(current1) - ��2(current2) - ��3���Ҷ�3 ���� ��2�� ��Ҫע����ǣ�start�϶����Ƕ�1����ΪZIG�㷨��֤�ߵ͵㶼��������β��
								     ����start���Ҳ�Ǹ���(����ڶ�2)������start�Ƕ�����ôstartҲ������ڶ�2������1���ڶ�2�����Զ�1�϶�������start��
									 ��ˣ�last���Ա���ȫ��cancel����
                                                                              ��2
                                                                               /\                           ��3
                                                                             /    \                           /\
                                                   ��1                   /        \           /\         /    \            /\
                                                   /\                   /             \       /    \    /        \        /     \
                                      /\        /    \    /\        /                 \    /       \/            \    /         \
                                    /    \    /        \/    \    /                     \/                          \/             \
                                  /        \/                   \/                                                                     \
                                /                              ��1
								*/
								// ȡ�� ��1����1
								cancelPoint(map, last);
								cancelPoint(map, current1);
								changed = true;
								getPrevPoint(map, s, last);
								current1 = current2;
								if (!getNextPoint(map, e, current2))
									return;
							}
						}
						else {
							// Ʃ�磺 ��1- ��1 -��2- ��2
							
							if (
								(map[last] == IS_TROUGH ? (*(veryStart + last)).getLow() <= (*(veryStart + current2)).getLow() : (*(veryStart + last)).getHigh() >= (*(veryStart + current2)).getHigh())
								&&
								(map[next] == IS_TROUGH ? (*(veryStart + next)).getLow() <= (*(veryStart + current1)).getLow() : (*(veryStart + next)).getHigh() >= (*(veryStart + current1)).getHigh())
								)
							{
								// ��� ��1 ���� ��2 ������ ��2 ���� ��1
								cancelPoint(map, current1);
								cancelPoint(map, current2);
								changed = true;
								current2 = current1 = next;
								if (!getNextPoint(map, e, current2))
									return;
							}
							else
								// �һ�û�л���������ͼ�Ρ�ͷʹ����������
								assert(0);
						}
					}
					else
					{
						// current2 ���� end����ˣ�current2���뱣����lastҲ�п�����start�����Ҳ��Ҫ���������ԣ�ֻȡ��current1
						cancelPoint(map, current1);
						changed = true;
						break;
					}
					
				}
				else if (map[current1] == map[current2])  // ����������ף�����
				{
					if (map[current1] == IS_TROUGH ? (*(veryStart + current1)).getLow() >= (*(veryStart + current2)).getLow() : (*(veryStart + current1)).getHigh() <= (*(veryStart + current2)).getHigh())
					{
						cancelPoint(map, current1);
						changed = true;
						current1 = current2;
						if (!getNextPoint(map, e, current2))
							break;
					}
					else {
						cancelPoint(map, current2);
						changed = true;
						if (!getNextPoint(map, e, current2))
							break;
					}
				}
				else 
				{
					// ���쳣
					last = current1;
					current1 = current2;
					if (!getNextPoint(map, e, current2))
						break;
				}
			}

		} while (changed);
	}

	static void createResult(ContainerType* container, InflectionPoint  *map, baseItemIterator veryStart, baseItemIterator start, baseItemIterator end)
	{
		int last = start - veryStart;
		int e = end - veryStart + 1;
		int cur = last;
		while (cur < e)
		{
			getNextPoint(map, e, cur);
			container->push_back(XianDuanClass(veryStart+last, veryStart+cur-1));
			last = cur;
		}
	}

	static ContainerType* startFenXianDuan_v2()
	{
		/*
		�յ������������ÿһ���յ��Ҳ�1�ʣ��Ǹùյ����������������ͼ�еĢ٢ڢۡ��ܢݢ޷ֱ��Ƕ�Ӧ�Ĺյ�1��2��3��4��5��6������������

		�׷��͡��׷��͹յ㣺�ٵĵ͵���ڢڵĵ͵㣬���� �۵ĸߵ㡢�͵㶼Ҫ���ڢڵĸߵ㡢�͵㣬��յ�1��2��3���� �׷��ͣ��յ�2�� �׷��͹յ㡣 
		ֱ������˵���׷��;��ǣ���3�����ϵ�ת�ۣ�����λ���м���Ǹ�ת�ۣ��յ�2����ͣ����������Ǹ�ת������1�ʣ��յ�3�������������м��Ǹ�ת������1�ʣ��յ�2���������������¸ߣ����� �յ�3��ķ������ڹյ�2��ķ�����

		\                  ���յ�3��ķ������ڹյ�2��ķ�����
		 \      /\              /
		  \    /  \      /\    /
		   \  ��   \    /  \  ��
		    \/      \  ��   \/
		  �յ�1      \/    �յ�3
		           �յ�2
		    (�յ�2�� �׷��͹յ�)

		�����͡������͹յ㣺�ܵĸߵ���ڢݵĸߵ㣬���� �޵ĸߵ㡢�͵㶼Ҫ���ڢݵĸߵ㡢�͵㣻��յ�4��5��6���� �����ͣ��յ�5�� �����͹յ�
		ֱ������˵�������;��ǣ���3�����µ�ת�ۣ�����λ���м���Ǹ�ת�ۣ��յ�5����ߣ����������Ǹ�ת������1�ʣ��յ�6�������������м��Ǹ�ת������1�ʣ��յ�5���������������µ�

			(�յ�5�ǹ��� �����͹յ�)
			       �յ�5�����͹յ㣩
		�յ�4         /\
		   /\        /  \
		  /  \      /    \      �յ�6
		 /    ��   /     ��      /\
		/      \  /        \    /  \
		        \/          \  /   ��
				             \/      \
							          \
					              ���յ�6�Ҳ��1�ʽϹյ�5�Ҳ�һ�ʴ����µͣ�

		����ͼ�п��Կ��������յ㹹�ɶ�/�׷��ͣ���ùյ�������������ڡ��ݣ���ǰ�󣬶�������1������������ͼ�еĢ٢ۡ��ܢޣ�����3������������������͵�Ҫ��

		��������Ĵ��� ���������İ�����ϵ

		   ���͹յ� ���յ�������������������ҵ�˳�򣬿ɽ��С�ǰ���󡱵ĺϲ�������Ϊ����������û���¸ߡ��µͣ����Կ��Ա�ǰ���������ϲ�����
		   ����ͼ�У��յ�3�Ƿ��͹յ㣬�����Ĺյ�1���յ�2�������������ߡ��ࣩ�����ڡ�ǰ���󡱹�ϵ���߰����ࣩ���ɽ��кϲ������ϲ�������������á�+����ʾ���ϲ��� �յ�2��ʧ�ˡ��յ�1��3��4���ɶ����ͣ��յ�3�Ƕ����͹յ�

		                           �յ�3                                                      �յ�3�������͹յ㣩
		       �յ�1                 /\      �յ�4                               �յ�1                /\       �յ�4
				/\        �յ�2     /  \      /\                                  /+                 /  \      /\
		       /  \        /\      /    \    /  \           �ϲ���               /    +             /    \    /  \
		      /    \      /  \    /      \  /    \       =============��        /        +         /      \  /    \
		     /     ��    /   ��  /        \/      \                            /            +     /        \/      \
		    /        \  /      \/                  \							  /                + /                  \
		   /          \/														 /          	
		  /																	/

		   ����ϲ���Ĺյ����û�������յ������������Ҫ����ô�ùյ�϶����Ƿ��͹յ㡣Ʃ�磬��ͼ�йյ�2���ϲ������ڹյ�1���û�е������Ĺյ���֮���ɷ��ͣ����Թյ�1һ�����Ƿ��͹յ�

		   ���͹յ����������������������ҵ�˳������ǡ����ǰ������ �����������������ɽ��а����������� ��Ϊ������������������Ϊ���������� ���¸� �� �µͣ���Ʃ�� ���������ܺ͢����㡰���ǰ�������ڢݴ����¸ߣ����Ԣ��Ƕ���������������

		   �յ��Ҳ�������������������ҵ�˳�򣬿ɽ��С�ǰ���󡱵ĺϲ�������Ϊ����������û���¸ߺ��µͣ���Ʃ����ͼ�еĢ�����㡰ǰ���󡱣��ϲ�������������á�+����ʾ���յ�3��ʧ�ˡ��յ�1��2��4���ɶ����ͣ��յ�2�Ƕ����͹յ�

		             �յ�2                                                         �յ�2
		               /\             �յ�4                                         /+           �յ�4
		    �յ�1     /  \     �յ�3    /\                               �յ�1     /   +          /\
		       /\    /   ��      /\    /  \                                 /\    /      +       /  \
		      /  \  /      \    / ��  /    \          �ϲ���               /  \  /         +    /    \
		     /    \/        \  /    \/      \       =============��       /    \/            + /      \ 
	        /   �յ�5        \/   �յ�7      \                           /   �յ�5           �յ�7     \
		   /              �յ�6                                         /

		   ��ͼ�У�ͬʱ������ �յ�1��2��4���㶥���ͣ� �յ�5��6��7����׷��͵������ �յ�2�Ƕ����͹յ㡢�յ�6�ǵ׷��͹յ㡣�յ�2��6֮�䣬������1�������ӣ����߶�������3�ʹ��ɣ�����յ�2��6ͬʱ�����������γɵ��߶�����3�ʣ���ˣ����͹յ�2���յ�6 ����ͬʱ������
		   ��ʱ�򣬾�Ҫ��Ϲյ�1֮ǰ���յ�4֮��Ķ����׷ֲ�״�������� �յ�2 �� �յ�6 ����ȡ�᣻ ԭ���޷Ǿ��ǣ� �������������ײ��������ڡ�

		   һ���������õķ�ʽ�ǣ� ��ʵ��� ���������߶� ���γɵĴ�����ƣ�Ȼ�����ÿһ�����ƣ������ڲ������߶λ��֡� ���磬���ֱ���ʹ�õ�zig������5%����ô�� ������ �γɵ����ƣ�����ȹ��ƾ���5*5%=25%��һ���������ƣ����ٰ���2�����࣬��ô�������ࡢ�嶥���ڵף�����3��5%���ȣ�Ҳ����5��5%����
		   ������������Ƶķ��ȣ�Ҳ�����߶μ���2�Ļ������ȡ� Ȼ���� ����2���߶� ���ɵ����࣬����Ⱦ���25�� ������������ ���ɵ����ƣ�����Ⱦ���5*25%��  
		   ����߶μ���2����������Ƶķ��ȣ������߶μ���3�Ļ������ȡ� �Դ����� �� �߶μ���7��

		 */
		ContainerType* resultSet = (ContainerType*)NULL;

		ContainerType* bigPicture = ZIG_PEAK_TROUGH<XianDuanClass, XianDuanClass::GRADE>(); // �Ȱ���zig ������۵��߶λ���

		itemIterator firstXianDuan = bigPicture->begin();
		itemIterator lastXianDuan = bigPicture->end();
		itemIterator curXianDuan;

		

	    // ÿ���յ㣬��Ӧ1��״̬λ�����Ͽ�ͷ����β��һ����XianDuanClass::baseItems->size()+1����
		
		InflectionPoint* map = new InflectionPoint[XianDuanClass::baseItems->size()+1];
		for (int i = 0; i < XianDuanClass::baseItems->size() + 1; i++)
			map[i] = NON;

		baseItemIterator veryStart = XianDuanClass::baseItems->begin(); // �ʼ��

		for (curXianDuan = firstXianDuan; curXianDuan < lastXianDuan; curXianDuan++)  //����ÿ������߶λ���
		{
			baseItemIterator biStart = (*curXianDuan).getStart(); //�߶εĿ�ʼ��
			baseItemIterator biEnd = (*curXianDuan).getEnd();   //�߶εĽ�����
			
			findFenXing(map, veryStart, biStart, biEnd, true); // Ѱ�� ������
			findFenXing(map, veryStart, biStart, biEnd, false); // Ѱ�� �׷���

			handleConflict(map, veryStart, biStart, biEnd);  // ���� �������׵����ڣ��Լ� ������֮��û��3�ʵ����

			if (!resultSet)
			{
				resultSet = new ContainerType();
			}
			createResult(resultSet, map, veryStart, biStart, biEnd);
		}

		delete bigPicture;
		delete[] map;

		return resultSet;
	}
