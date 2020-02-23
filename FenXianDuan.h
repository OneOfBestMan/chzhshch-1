#pragma once

#ifndef FENXIANDUAN_H
#define FENXIANDUAN_H


template<class XianDuanClass >
class FenXianDuanTemplate
{
public:

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
};


template<class XianDuan_or_Bi >
void FenXianDuan(bool release )
{
	FenXianDuan<XianDuan_or_Bi::baseItemType>(release);

	XianDuan_or_Bi::FenXianDuanClass::doWork(release);
}


#endif