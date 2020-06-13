#pragma once

#ifndef FENXIANDUAN_H
#define FENXIANDUAN_H

#include "Zig_Common.h"

template<class XianDuanClass >
class FenXianDuanTemplate
{
public:

	typedef typename XianDuanClass::baseItemType_Container::iterator baseItemIterator;
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType ContainerType;
	typedef typename XianDuanClass::baseItemType_Container baseItemType_Container;
	typedef typename ContainerType::iterator itemIterator;
	typedef XianDuanClass itemType;

	static void doWork(bool release)
	{
		if (release == false)
		{
			XianDuanClass::baseItems = XianDuanClass::baseItemType::container;
			
			if (XianDuanClass::baseItems &&  XianDuanClass::container == NULL)
			{
				//下面就是划分线段的具体逻辑
				baseItemIterator begin = XianDuanClass::baseItems->begin();
				baseItemIterator end = XianDuanClass::baseItems->end();
				XianDuanClass::container = startFenXianDuan(begin, end);
			}
		}
		else
		{
			delete XianDuanClass::container;
			XianDuanClass::container = NULL;
			XianDuanClass::baseItems = NULL;
		}
	}

private:

#include "FenXianDuan_Common.h"

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
void FenXianDuan_PostOrderTravel(bool release )
{
	FenXianDuan_PostOrderTravel<XianDuan_or_Bi::baseItemType>(release);

	XianDuan_or_Bi::FenXianDuanClass::doWork(release);
}


#endif