	typedef struct CharacterVec
	{
		// 特征向量； 如果，特征向量只有1笔，那么start和end指向同一个笔，否则，分别指向第一、最后一笔；
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
	
		/* 先合并（可能的）新线段开始处的包含（前包后）的几笔 */
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

		/* 找可能出现转折的位置 */
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

		
		/*  算上合并包含关系得到的那一笔， 到可能出现转折的笔，一共（至少）3笔，那么新线段就算成立，即：3笔确立线段*/
		if (possibleNewXianDuanEnd - prevChacVecEnd >= 2) 
			return true;
		else
		{
			/* 否则，处理合并关系得到的这一笔，被当做原线段的特征向量，原线段仍然继续 */
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
			/*在线段中，寻找可能出现转折的那一笔的位置； 包括，前包后、方向与原线段相反；不包括：后包前（因为有新高或新低）*/
			while  (biFormer + 2 < end &&  (getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biFormer + 1));
				biFormer = biLatter;
				biLatter += 2;
			}

			if (FXD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* 原线段延续*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				continue;
			} else
			{
				/* 原线段被破坏，将该线段添加到container中 */
				if (!resultSet)
				{
					resultSet = new ContainerType();
				}
				resultSet->push_back(XianDuanClass(biStart, biFormer, d));
				


				/* 新线段需要 翻转方向 */
				d = -d;
				/* 新的线段的第一笔*/
				biStart = ++ biFormer;
				/* 新的线段的特征向量栈初始化 */
				CharacVecStack.clear();

				/* 新的线段，开始的部分，有一组互相包含的线段，可以看成是之前线段的特征向量，可以应用包含关系。合并它们 */
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