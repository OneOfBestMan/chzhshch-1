	typedef struct CharacterVec
	{
		// ���������� �������������ֻ��1�ʣ���ôstart��endָ��ͬһ���ʣ����򣬷ֱ�ָ���һ�����һ�ʣ�
		baseItemIterator start; 
		baseItemIterator end;
		CharacterVec(baseItemIterator &s, baseItemIterator &e) {start = s; end = e;}
		CharacterVec() {}
	} CharacterVec;

	typedef vector<CharacterVec> analyzeStack;

	static Direction getDirection(baseItemType &former, baseItemType &latter)
	{
		return baseItemType::getDirection(former, latter);
	}


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
			d = getDirection(lastBi, suspect);
			if (d == hints || (lastBi << suspect))
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
			d = getDirection(*biFormer, *(biFormer + 2));
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
			while  (biFormer < end - 2 &&  (getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter) || (*biFormer >> *biLatter)) )
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

				if (getDirection(*biFormer, *biLatter) == d || (*biFormer >> *biLatter))
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