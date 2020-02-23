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
				XianDuanClass::container = new XianDuanClass::ContainerType(XianDuanClass::baseItems->size());
				//下面就是划分线段的具体逻辑
				XianDuanClass::HuaFenXianDuan();
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
	XianDuan_or_Bi::baseItemType::FenXianDuanClass::doWork(release);

	XianDuan_or_Bi::FenXianDuanClass::doWork(release);
}


#endif