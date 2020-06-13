#pragma once

/*
ZIG�㷨
*/

template<class XianDuan_or_Bi>
typename XianDuan_or_Bi::ContainerType* ZIG_PEAK_TROUGH(float ZIG_PERCENT=5)
{
	/*
	���������zig_percent����base_Container�е�Ԫ��Ѱ�ҹյ㡣 
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
	INIT���� ��һֱ������ ���� һ�Ե͵㡢�ߵ�  ֮��ķ��� ���ڵ���ZIG_PERCENT���Ż�ת��SEARCHING_TOP����SEARCHING_BOTTOM��
	һ��ת��SEARCHING_TOP����SEARCHING_BOTTOM���������ٽ���INIT��
	���ҵ�������һ�Ե͵㡢�ߵ�֮ǰ��INIT ���̽����ǣ�
        ��������1�ʣ����͵����possibleBot�����possibleBot����������1�ʣ����ߵ����possibleTop�����possibleTop��
        ���ϱȽ�possibleBot��possibleTop֮��ķ��ȣ�������ZIG_PERCENT������ǰλ����possibleTop����ȷ�ϵ���possibleBot�����ú�curBotBarsΪposBotBars������SEARCHING_TOP��
        ����ǰλ����possibleBot����ȷ�ϵ���possibleTop�����ú�curTopBarsΪposTopBars������SEARCHING_BOT
	���� �ӿ�ʼ��1��k�� �� ��һ����ȷ�ϵĶ����ߵף���Ϊ����һ�εķ��ȿ϶�����ZIG_PERCENT�����ң�Ҳ��һ�����㡰����ʼ������1�ʣ�������������1�ʡ���Ʃ�磺

	    ������ zig ���ó�Ϊ30%��
                                        �յ�3
	                                      /\
	                                    /    \
	     �յ�1                     /        \
             /\                     /          -32%
       +5%   \                /                \
        /      -15%       +40%               \
  ��ʼ��         \        /                         \
                       \    /                          �յ�4
                         \/
				   �յ�2

    �㷨�������ӿ�ʼ�� ���յ�2��һ���֣� ���ȣ���һ�ε�zig�϶�����30%���ڶ�����Ȼ��ʼ�� ����ڹյ�2��Ӧ���Ƕ�����Ϊ�յ�2�ǵף����ҿ�ʼ�㲻���ܵ��ڹյ�2���������䵽�յ�1֮ǰ��1��ȴ�����ϵģ��������㡰����ʼ������1�ʣ�������������1�ʡ������Էǳ���Ť��������
	��ˣ��ӿ�ʼ�� �� �յ�2��һ�Σ��ᱻ������

	SEARCHING_TOP ������ �õ���curBotBars(��ȷ�ϵ׹յ��Ҳ�ĵ�1��)��posTopBars����ȷ�϶��յ����ĵ�1�ʣ������������ϵģ� 
	possibleTop ���� ���µ�һ�ʵ͵�(curLow)��������ZIG_PERCENTʱ�ű�ȷ��
	�˹��� Ѱ��curLow�����possibleTop��������ZIG_PERCENT���½���curBotBars��posTopBars�������߶Σ�


                                                                (possibleTop����ȷ�ϵĶ��յ�)
                                                                               /\
                                                     /\                     /    \
                                                   /    \                 /   (ZIG_PERCENT)
                                                 /        \   posTopBars    \
                                               /            \          /               \    /
                                       curBotBars     \        /                  \/
                                            /                  \    /              (curLow) �����possibleTop��������ZIG_PERCENT����possibleTop�ű�ȷ��Ϊ ���յ�
                                          /                      \/             
                                        /
                                (��ȷ�ϵĵ׹յ�)

	 SEARCHING_BOTTOM ������ �õ���curTopBars(��ȷ�϶��յ���Ҳ��1��)��posBotBars����ȷ�ϵ׹յ������1�ʣ�,���������µģ� 
	 possibleBot ���� ���ϵ�һ�ʸߵ㣨curHigh���Ƿ�����ZIG_PERCENTʱ�ű�ȷ��
	 �˹��� Ѱ��curHigh�����possibleBot�Ƿ�����ZIG_PERCENT���½���curTopBars��posBotBars�������߶Σ�


                                           (��ȷ�ϵĶ��յ�) 
                                                  \
                                                    \                  /\            (curHigh)�����possibleBot�Ƿ�����ZIG_PERCENT����possibleBot�ű�ȷ��Ϊ �׹յ�
                                              curTopBars      /    \                       /\
                                                       \            /       \                    /   \
                                                         \        /           \                /
                                                           \    /  posBotBars         /
                                                             \/                    \     (ZIG_PERCENT)
                                                                                      \    /
                                                                                        \/
                                                                                (possibleBot����ȷ�ϵĵ׹յ�)
 
		*/ 


	/*
	��ʼ��ʱ����� ��1�� ���ϣ� �� ��1�ʵ���� ���ٶ���һ�� ��ȷ�ϵĵ׹յ㣬 �õ׹յ� ����һ�ʣ�posBotBars = start-1���������£���Ȼ��1�ʲ����ڣ�;
	��� ��1�� ���£� �� ��1�ʵ���� ���ٶ���һ�� ��ȷ�ϵĶ��յ㣬 �ö��յ� ����һ�ʣ�possibleTop = start-1���������ϣ���Ȼ��1�ʲ����ڣ�.
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
				if (curLow*  (1 + ZIG_PERCENT / 100) <= possibleTop)  // (curLow) �����possibleTop��������ZIG_PERCENT
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
		else if (goal == SEARCHING_TOP)  // Ѱ�Ҷ��յ��ȷ�ϵ㣺 �ҵ�һ�����µ���ʣ���ʵĵ͵���ڵ�ǰ�ߵ㣨possibleTop��ת�۳���  ZIG_PERCENT
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
					// �ҵ� ���յ㣬 ���յ��������ϱ� ��ȷ��
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
		else // SEARCHING_BOT  Ѱ�ҵ׹յ��ȷ�ϵ㣺 �ҵ�һ�����ϵ���ʣ���ʵĸߵ���ڵ�ǰ�͵㣨possibleBot��ת�۳���  ZIG_PERCENT
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
				if (possibleBot*(1 + ZIG_PERCENT / 100) <= curHigh) // (curHigh)�����possibleBot�Ƿ�����ZIG_PERCENT
				{
					// �ҵ� �׹յ㣬 �׹յ��������±� ��ȷ��
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
	��� ���һ����ȷ�Ϲյ��Ҳ��1�ʣ� ���� ���һ����ȷ�Ϲյ� �� ȷ�ϸùյ�Ĺؼ���(critical point)
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

