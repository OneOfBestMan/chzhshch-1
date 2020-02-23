#include "stdafx.h"
#include "Class_Bi.h"
#include "Class_env.h"

/*
实现 笔
（1）笔的构成： 底分型、顶分型构成一笔
（2）笔的破坏：
（3）笔的属性：
      底分型
	  顶分型
	  
*/


template<class baseItem_Container>
Class_Bi<baseItem_Container>::~Class_Bi(void)
{
}


template<class baseItem_Container>
void Class_Bi<baseItem_Container>::accept(int bar)
{
	/*
	if (contains.empty())
	{
		contains.push(bar);
		return;
	} else
	{
		int lastBar = contains.top();
        if (bar - lastBar > 1)
		{
			// handle mergedTop and mergedBot
			if (Class_env::rangeContain(mergedBot, mergedTop, Class_env::getKLow(bar), Class_env::getKHigh(bar)))
				Class_env::updateMergedRange(direction, bar, mergedBot, mergedTop);
			else 
			{

			}
		}

	}*/


}