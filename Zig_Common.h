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

	SEARCHING_TOP 过程中 用到：curBotBars(已确认底拐点右侧的第1笔)、posTopBars（待确认顶拐点左侧的第1笔），方向都是向上的；
	possibleTop 仅当 向下的一笔低点(curLow)跌幅超过ZIG_PERCENT时才被确认


	(possibleTop：待确认的顶拐点)
	/\
	/\            /  \
	/  \          / (ZIG_PERCENT)
	/    \  posTopBars   \
	/      \      /        \  /
	curBotBars   \    /          \/
	/          \  /        (curLow) 相对于possibleTop跌幅超过ZIG_PERCENT，则possibleTop才被确认为 顶拐点
	/            \/
	/
	(已确认的底拐点)

	SEARCHING_BOTTOM 过程中 用到：curTopBars(已确认顶拐点的右侧第1笔)、posBotBars（待确认底拐点的左侧第1笔）,方向都是向下的；
	possibleBot 仅当 向上的一笔高点（curHigh）涨幅超过ZIG_PERCENT时才被确认


	(已确认的顶拐点)
	\
	\            /\            (curHigh)相对于possibleBot涨幅超过ZIG_PERCENT，则possibleBot才被确认为 底拐点
	curTopBars     /  \            /\
	\        /    \          /  \
	\      /      \        /
	\    / posBotBars    /
	\  /          \  (ZIG_PERCENT)
	\/            \  /
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

