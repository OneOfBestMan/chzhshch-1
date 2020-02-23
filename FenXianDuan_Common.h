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
		while (current + 2 < end)
		{
			if (possiblePrevXianDuanChracVec >> *(current + 2))
			{
				possiblePrevXianDuanChracVec.merge(*(current+2), -hints);
				current += 2;
				continue;
			}
			else
				break;
		}

		prevCharacVecEnd = current;

		/* �ҿ��ܳ���ת�۵�λ�� */
		baseItemType lastBi = possiblePrevXianDuanChracVec;
		while (current + 2 < end)
		{
			d = getDirection(lastBi, *(current + 2));
			if (d == hints)
			{
				lastBi = *(current + 2);
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



	static ContainerType* startFenXianDuan(baseItemIterator start, baseItemIterator end)
	{
		ContainerType* resultSet = (ContainerType*)NULL;

		baseItemIterator biStart = start;
		baseItemIterator biFormer = start;
		Direction d = ENCLOSING;
		while (biFormer + 2 < end)
		{
			d = getDirection(*biFormer, *(biFormer + 2));
			if (d == ENCLOSING)
			{
				biFormer += 2;
				continue;
			}
			else
				break;
		}

		if (biFormer + 2 >= end) return resultSet;

		analyzeStack CharacVecStack;

		baseItemIterator biLatter = biFormer + 2;

		do 
		{
			/*���߶��У�Ѱ�ҿ��ܳ���ת�۵���һ�ʵ�λ�ã� ������ǰ���󡢷�����ԭ�߶��෴�������������ǰ����Ϊ���¸߻��µͣ�*/
			while  (biFormer + 2 < end &&  (getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biFormer + 1));
				biFormer = biLatter;
				biLatter += 2;
			}

			if (FXD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* ԭ�߶�����*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				continue;
			} else
			{
				/* ԭ�߶α��ƻ��������߶���ӵ�container�� */
				if (!resultSet)
				{
					resultSet = new ContainerType();
				}
				resultSet->push_back(XianDuanClass(biStart, biFormer, d));
				


				/* ���߶���Ҫ ��ת���� */
				d = -d;
				/* �µ��߶εĵ�һ��*/
				biStart = ++ biFormer;
				/* �µ��߶ε���������ջ��ʼ�� */
				CharacVecStack.clear();

				/* �µ��߶Σ���ʼ�Ĳ��֣���һ�黥��������߶Σ����Կ�����֮ǰ�߶ε���������������Ӧ�ð�����ϵ���ϲ����� */
				baseItemType temp = *biFormer;
				while (biFormer + 2 < end)
				{
					if (temp >> *(biFormer + 2))
					{
						temp.merge(*(biFormer + 2), -d);
						biFormer += 2;
						continue;
					}
					else
						break;
				}
				biLatter = biFormer + 2;
			}
		} while (biFormer + 2 < end);

		return resultSet;
	}