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
		// ���������� �������������ֻ��1�ʣ���ôstart��endָ��ͬһ���ʣ����򣬷ֱ�ָ���һ�����һ�ʣ�
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
				//������ǻ����߶εľ����߼�
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

	static bool FXD_Case1(Direction hints, const baseItemIterator &start, const baseItemIterator &end, analyzeStack& lastXianDuan_CharacVecStack)
	{
		baseItemIterator prevChacVecEnd;  
		baseItemIterator possibleNewXianDuanEnd = FXD_Merge(hints, start, end, prevChacVecEnd);

		
		/*  ���Ϻϲ�������ϵ�õ�����һ�ʣ� �����ܳ���ת�۵ıʣ�һ�������٣�3�ʣ���ô���߶ξ������������3��ȷ���߶�*/
		if (possibleNewXianDuanEnd - prevChacVecEnd >= 2) 
			return true;
		else
		{
			/* ���򣬴���ϲ���ϵ�õ�����һ�ʣ�������ԭ�߶ε�����������ԭ�߶���Ȼ���� */
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
			/*���߶��У�Ѱ�ҿ��ܳ���ת�۵���һ�ʵ�λ�ã� ������ǰ���󡢷�����ԭ�߶��෴�������������ǰ����Ϊ���¸߻��µͣ�*/
			while  (biFormer + 2 < end &&  (getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biFormer + 1));
				biFormer = biLatter;
				biLatter += 2;
			}

			if (FSD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* ԭ�߶�����*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				continue;
			} else
			{
				/* ԭ�߶α��ƻ��� ��ת���� */
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