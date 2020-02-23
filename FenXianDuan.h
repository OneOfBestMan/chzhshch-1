#pragma once

#ifndef FENXIANDUAN_H
#define FENXIANDUAN_H


template<class XianDuanClass >
class FenXianDuanTemplate
{
public:

	typedef typename XianDuanClass::baseItemType_Container::iterator baseItemIterator;
	typedef typename XianDuanClass::baseItemType baseItemType;

	typedef struct CharacterVec
	{
		// 特征向量； 如果，特征向量只有1笔，那么start和end指向同一个笔，否则，分别指向第一、最后一笔；
		baseItemIterator start; 
		baseItemIterator end;
		CharacterVec(baseItemIterator &s, baseItemIterator &e) {start = s; end = e;}
		CharacterVec() {}
	} CharacterVec;

	typedef vector<CharacterVec> analyzeStack;


	static void doWork(bool release)
	{
		if (release == false)
		{
			XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
			
			if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
			{
				//下面就是划分线段的具体逻辑
				XianDuanClass::container = XianDuanClass::HuaFenXianDuan();
			}
		}
		else
		{
			delete XianDuanClass::container;
			XianDuanClass::container = NULL;
			XianDuanClass::baseItems = NULL;
		}
	}

	static Direction getDirection(baseItemType &former, baseItemType &latter)
	{
		return baseItemType::getDirection(former, latter);
	}


	static baseItemIterator FXD_Merge(Direction hints, const baseItemIterator & start, const baseItemIterator & end, baseItemIterator &prevCharacVecEnd)
	{
		assert(hints != ENCLOSING);

		baseItemIterator current = start;
		Direction d;
	
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

	static bool FXD_Case1(Direction hints, const baseItemIterator &start, const baseItemIterator &end, analyzeStack& lastXianDuan_CharacVecStack)
	{
		baseItemIterator prevChacVecEnd;  
		baseItemIterator possibleNewXianDuanEnd = FXD_Merge(hints, start, end, prevChacVecEnd);

		
		if (possibleNewXianDuanEnd - prevChacVecEnd >= 2) 
			return true;
		else
		{
			lastXianDuan_CharacVecStack.push(CharacterVec(start, prevChacVecEnd));
			return false;
		}
	}



	static void startFenXianDuan(baseItemIterator start, baseItemIterator end)
	{
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

		if (biFormer + 2 >= end) return;

		analyzeStack CharacVecStack;

		baseItemIterator biLatter = biFormer + 2;
		baseItemIterator biStart = biFormer;

		do 
		{
			while  (biFormer + 2 < end &&  (getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biFormer + 1));
				biFormer = biLatter;
				biLatter += 2;
			}

			if (FSD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* 原线段延续*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				continue;
			} else
			{
				d = -d;
				biStart = ++ biFormer;
				CharacVecStack.clear();

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
	}

	/*
	static bool FXD_Case2(Direction hints, const baseIterator &firstChildXianDuanStart, const baseIterator &end, stack<baseItemIterator> &lastXianDuan_CharacVecStack, baseIterator &newXianDuanStart)
	{
		baseIterator secondChildXianDuanStart;
		if (!FXD_Case1(hints, firstChildXianDuanStart, end, &lastXianDuan_CharacVecStack, secondChildXianDuanStart))
			return false;

		baseIterator newXianDuanStart;
		if (!FXD_Case1(-hints, secondChildXianDuanStart, end, NULL, newXianDuanStart))
		{
			lastXianDuan_CharacVecStack.push(CharacterVec(firstChildXianDuanStart, seondXianDuanStart - 1));
			return false;
		}

		return true;
	}*/

};


template<class XianDuan_or_Bi >
void FenXianDuan(bool release )
{
	FenXianDuan<XianDuan_or_Bi::baseItemType>(release);

	XianDuan_or_Bi::FenXianDuanClass::doWork(release);
}


#endif