#pragma once

/*
ZIG算法
*/

template<class XianDuan_or_Bi>
typename XianDuan_or_Bi::ContainerType* ZIG_PEAK_TROUGH(float ZIG_PERCENT=5)
{
	/*
	根据输入的zig_percent，对base_Container中的元素寻找拐点。 
	*/

	typedef typename XianDuan_or_Bi::ContainerType ContainerType;
	typedef typename XianDuan_or_Bi::baseItemType_Container baseItemType_Container;
	typedef typename XianDuan_or_Bi::baseItemIterator baseItemIterator;


	ContainerType* result = new ContainerType();

	baseItemIterator start = XianDuan_or_Bi::baseItems->begin();
	baseItemIterator end = XianDuan_or_Bi::baseItems->end();

	float possibleBot((*start).getDirection() == ASCENDING ? (*start).getLow() : -1);
	float possibleTop((*start).getDirection() == DESCENDING ? (*start).getHigh() : -1);

	/* 
	INIT过程 会一直持续到 遇到 一对低点、高点  之间的幅度 大于等于ZIG_PERCENT，才会转入SEARCHING_TOP或者SEARCHING_BOTTOM；
	一旦转入SEARCHING_TOP或者SEARCHING_BOTTOM，将不会再进入INIT；
	在找到这样的一对低点、高点之前，INIT 过程仅仅是：
        遇到向下1笔，若低点低于possibleBot则更新possibleBot；遇到向上1笔，若高点高于possibleTop则更新possibleTop；
        不断比较possibleBot、possibleTop之间的幅度，若大于ZIG_PERCENT，若当前位置是possibleTop，则确认的是possibleBot，设置好curBotBars为posBotBars，进入SEARCHING_TOP；
        若当前位置是possibleBot，则确认的是possibleTop，设置好curTopBars为posTopBars，进入SEARCHING_BOT
	舍弃 从开始第1根k线 到 第一个被确认的顶或者底，因为：这一段的幅度肯定不足ZIG_PERCENT，并且，也不一定满足“顶开始于向下1笔，底满足于向上1笔”，譬如：

	    （假设 zig 设置成为30%）
                                        拐点3
	                                      /\
	                                    /    \
	     拐点1                     /        \
             /\                     /          -32%
       +5%   \                /                \
        /      -15%       +40%               \
  开始点         \        /                         \
                       \    /                          拐点4
                         \/
				   拐点2

    算法会舍弃从开始点 到拐点2这一部分； 首先，这一段的zig肯定不足30%；第二，虽然开始点 相对于拐点2，应该是顶（因为拐点2是底，并且开始点不可能低于拐点2），但是其到拐点1之前的1笔却是向上的，并不满足“顶开始于向下1笔，底满足于向上1笔”，所以非常别扭。。。。
	因此，从开始点 到 拐点2这一段，会被舍弃。

	SEARCHING_TOP 过程中 用到：curBotBars(已确认底拐点右侧的第1笔)、posTopBars（待确认顶拐点左侧的第1笔），方向都是向上的； 
	possibleTop 仅当 向下的一笔低点(curLow)跌幅超过ZIG_PERCENT时才被确认
	此过程 寻找curLow相对于possibleTop跌幅超过ZIG_PERCENT，新建从curBotBars到posTopBars的向上线段；


                                                                (possibleTop：待确认的顶拐点)
                                                                               /\
                                                     /\                     /    \
                                                   /    \                 /   (ZIG_PERCENT)
                                                 /        \   posTopBars    \
                                               /            \          /               \    /
                                       curBotBars     \        /                  \/
                                            /                  \    /              (curLow) 相对于possibleTop跌幅超过ZIG_PERCENT，则possibleTop才被确认为 顶拐点
                                          /                      \/             
                                        /
                                (已确认的底拐点)

	 SEARCHING_BOTTOM 过程中 用到：curTopBars(已确认顶拐点的右侧第1笔)、posBotBars（待确认底拐点的左侧第1笔）,方向都是向下的； 
	 possibleBot 仅当 向上的一笔高点（curHigh）涨幅超过ZIG_PERCENT时才被确认
	 此过程 寻找curHigh相对于possibleBot涨幅超过ZIG_PERCENT，新建从curTopBars到posBotBars的向下线段；


                                           (已确认的顶拐点) 
                                                  \
                                                    \                  /\            (curHigh)相对于possibleBot涨幅超过ZIG_PERCENT，则possibleBot才被确认为 底拐点
                                              curTopBars      /    \                       /\
                                                       \            /       \                    /   \
                                                         \        /           \                /
                                                           \    /  posBotBars         /
                                                             \/                    \     (ZIG_PERCENT)
                                                                                      \    /
                                                                                        \/
                                                                                (possibleBot：待确认的底拐点)
 
		*/ 


	/*
	初始的时候，如果 第1笔 向上， 则 第1笔的起点 被假定是一个 已确认的底拐点， 该底拐点 左侧的一笔（posBotBars = start-1）方向向下（虽然这1笔不存在）;
	如果 第1笔 向下， 则 第1笔的起点 被假定是一个 已确认的顶拐点， 该顶拐点 左侧的一笔（possibleTop = start-1）方向向上（虽然这1笔不存在）.
	*/
	baseItemType_Container::iterator posBotBars((*start).getDirection() == ASCENDING ? start - 1 : start); // pos: possible
	baseItemType_Container::iterator posTopBars((*start).getDirection() == DESCENDING ? start - 1 : start); // pos: possible
	baseItemType_Container::iterator curBotBars(end), curTopBars(end); // cur: current


	enum { INIT, SEARCHING_TOP, SEARCHING_BOT } goal = INIT;
	enum { NONE, IS_PEAK, IS_TROUGH } lastInflectionPoint = NONE;


	baseItemType_Container::iterator current;
	for (current = start; current < end; current++)
	{
		float curHigh = (*current).getHigh();
		float curLow = (*current).getLow();
		Direction d = (*current).getDirection();

		if (goal == INIT)
		{
			if (d == ASCENDING)
			{
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh)
				{
					curBotBars = posBotBars + 1;
					posBotBars = end;
					possibleTop = curHigh;
					posTopBars = current;
					goal = SEARCHING_TOP;
					lastInflectionPoint = IS_TROUGH;

					assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);
				}
				else if (curHigh > possibleTop)
				{
					posTopBars = current;
					possibleTop = curHigh;
				}
			}
			else {
				// DESCENDING
				if (curLow*  (1 + ZIG_PERCENT / 100) <= possibleTop)  // (curLow) 相对于possibleTop跌幅超过ZIG_PERCENT
				{
					curTopBars = posTopBars + 1;
					posTopBars = end;
					possibleBot = curLow;
					posBotBars = current;
					goal = SEARCHING_BOT;
					lastInflectionPoint = IS_PEAK;

					assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);

				}
				else if (curLow < possibleBot)
				{
					posBotBars = current;
					possibleBot = curLow;
				}
			}
		}
		else if (goal == SEARCHING_TOP)  // 寻找顶拐点的确认点： 找到一个向下的类笔，类笔的低点对于当前高点（possibleTop）转折超过  ZIG_PERCENT
		{
			if (d == ASCENDING)
			{
				if (curHigh > possibleTop)
				{
					possibleTop = curHigh;
					posTopBars = current;
				}
			}
			else
			{
				if (curLow * (1 + ZIG_PERCENT / 100) <= possibleTop)
				{
					// 找到 顶拐点， 顶拐点左侧的向上笔 被确认
					result->push_back(ContainerType::value_type(curBotBars, posTopBars));
					assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);

					curTopBars = posTopBars + 1;
					posTopBars = start - 1;
					posBotBars = current;
					possibleBot = curLow;
					goal = SEARCHING_BOT;
					lastInflectionPoint = IS_PEAK;
				}
			}
		}
		else // SEARCHING_BOT  寻找底拐点的确认点： 找到一个向上的类笔，类笔的高点对于当前低点（possibleBot）转折超过  ZIG_PERCENT
		{
			if (d == DESCENDING)
			{
				if (curLow < possibleBot)
				{
					possibleBot = curLow;
					posBotBars = current;
				}
			}
			else
			{
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh) // (curHigh)相对于possibleBot涨幅超过ZIG_PERCENT
				{
					// 找到 底拐点， 底拐点左侧的向下笔 被确认
					result->push_back(ContainerType::value_type(curTopBars, posBotBars));
					assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);

					curBotBars = posBotBars + 1;
					posBotBars = start - 1;
					posTopBars = current;
					possibleTop = curHigh;
					goal = SEARCHING_TOP;
					lastInflectionPoint = IS_TROUGH;
				}
			}
		}
	}

	/*
	添加 最后一个已确认拐点右侧的1笔， 即： 最后一个已确认拐点 和 确认该拐点的关键点(critical point)
	*/
	if (lastInflectionPoint == IS_PEAK) {
		result->push_back(ContainerType::value_type(curTopBars, posBotBars));
		assert((*curTopBars).getDirection() == (*posBotBars).getDirection() && (*curTopBars).getDirection() == DESCENDING);
	}
	else if (lastInflectionPoint == IS_TROUGH) {
		result->push_back(ContainerType::value_type(curBotBars, posTopBars));
		assert((*curBotBars).getDirection() == (*posTopBars).getDirection() && (*curBotBars).getDirection() == ASCENDING);
	}

	return result;

}

