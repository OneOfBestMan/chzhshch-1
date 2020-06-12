  class CharacterVec: public IComparable
	{
	public:
		// 特征向量； 如果，特征向量只有1笔，那么start和end指向同一个笔，否则，分别指向第一、最后一笔；
		baseItemIterator start, end;

		float getHigh() const {return High;}
		float getLow() const {return Low;}
		void setHigh(float h) {High = h;}
		void setLow(float l) {Low = l;}

		CharacterVec(baseItemIterator &s, baseItemIterator &e) 
		{
			assert((*s).getDirection() == (*e).getDirection());
			start = s; 
			end = e;
			d = (*s).getDirection();
			
			High = (*s).getHigh();
			Low = (*s).getLow();

			while (s != e)
			{
				s += 2;
				merge(*s, -d);
			}
		}

	private:
		Direction d;
		float High, Low;	
	};

	typedef vector<CharacterVec> analyzeStack;

	static baseItemIterator FXD_Merge(Direction hints, const baseItemIterator & start, const baseItemIterator & end, baseItemIterator &prevCharacVecEnd)
	{
		assert(hints != ENCLOSING);

		baseItemIterator current = start;
		Direction d;
	
		/* 先合并（可能的）新线段开始处的包含（前包后）的几笔 */
		baseItemType possiblePrevXianDuanChracVec = *current;
		while (current < end - 2)
		{
			baseItemType& suspect = *(current + 2);
			if (possiblePrevXianDuanChracVec >> suspect)
			{
				possiblePrevXianDuanChracVec = suspect;
				current += 2;
				continue;
			}
			else
				break;
		}

		prevCharacVecEnd = current;

		/* 找可能出现转折的位置 */
		baseItemType lastBi = possiblePrevXianDuanChracVec;
		while (current < end - 2)
		{
			baseItemType &suspect = *(current + 2);
			d = IComparable::getDirection(lastBi, suspect);
			if (d == hints || (lastBi << suspect && !(*start << suspect)))
			{
				lastBi = suspect;
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


	static void Normalize(Direction hints, baseItemIterator start, baseItemIterator end)
	{
		if (hints == (*start).getDirection()) return;

		baseItemIterator current = start;

		/* 所有线段方向均取反，先处理序号是奇数的线段 */
		while (current != end)
		{
			(*current).d = -(*current).d;

			current++;
			if (current == end)
				break;
			else
				current++;
		}

		/* 对于序号是偶数的线段，除了方向取反，还需要处理高、低点 */
		current = start + 1;
		while (current < end - 1) // 这个条件是如何写出来的： 看下图，BC和CD，要求处理BC的时候，后面至少还有一个CD，才处理BC；因此如果，CD后面就是end，那么就有 BC < end - 1了
		{
			if ((*current).getDirection() == DESCENDING)
			{
/*
              B                       A
             /\                        \
            /  \                        \
           /    \         D              \      C
          /      \        /               \    /\
         /        \      /     ===>        \  /  \
        /          \    /                   \/    \
       A            \  /                    B      \
                     \/                             \
                     C                               D
*/
				(*current).Low = (*(current - 1)).getLow();
				(*current).High = (*(current + 1)).getHigh();
			}
			else
			{
/*
                    C                            D
                   /\                           /
                  /  \                         /
        A        /    \                  B    /
         \      /      \      ===>       /\  /
          \    /        \               /  \/
           \  /         D              /    C
            \/                        /
            B                        A

*/
				assert((*current).getDirection() == ASCENDING);
				(*current).High = (*(current - 1)).getHigh();
				(*current).Low = (*(current + 1)).getLow();
			}
			(*current).d = -(*current).getDirection();

			current += 2;
		}
	}

	static void NormalizeV2(baseItemIterator start, baseItemIterator end)
	{
		/* 这个函数，处理各个相邻线段，连接点处，最低值、最高值 不相等的情况；出现这种情况的原因，是因为，线段的最低值、最高值出现在线段中间某根K线，而不是出现在连接点处 */
		baseItemIterator curr = start;

		while (curr != end - 1)
		{
			baseItemType &former = *curr;
			baseItemType &latter = *(curr + 1);


			if (former.getDirection() == ASCENDING)
			{
				latter.High = former.High = max(former.getHigh(), latter.getHigh());
			}
			else
			{
				latter.Low = former.Low = min(former.getLow(), latter.getLow());
			}
			curr++;
		}
	}

	static ContainerType* startFenXianDuan(baseItemIterator start, baseItemIterator end)
	{
		NormalizeV2(start, end);

		baseItemType_Container* backupBeforeNormalize = (baseItemType_Container*)NULL;
		ContainerType* resultSet = (ContainerType*)NULL;

		baseItemIterator biStart = start;
		baseItemIterator biFormer = start;
		Direction d = ENCLOSING;
		while (end - biFormer > 2)
		{
			d = IComparable::getDirection(*biFormer, *(biFormer + 2));
			Direction dXian = (*biFormer).getDirection();
			if (d == ENCLOSING || d == -dXian) // d == -dXian  是为了规避normalize
			{
				biFormer += 1;
				continue;
			}
			else
				break;
		}

		if (end - biFormer <= 2) return resultSet;

		if (d == -(*biFormer).getDirection())
		{
			assert(0); // normalize 应该不会再被调用了，由于前面加了 d == -dXian条件
			backupBeforeNormalize = new baseItemType_Container();
			backupBeforeNormalize->assign(start,end);

			Normalize(d, biFormer, end);
		}

		analyzeStack CharacVecStack;

		baseItemIterator biLatter = biFormer + 2;

#ifdef DEBUG
		// 这段代码是用来调试死循环的。 
		int debugCnt = 0;
		baseItemIterator oldBiFormer = biFormer;
		baseItemIterator oldBiLatter = biLatter;
		int count = 0;
#endif

		do 
		{
			/*在线段中，寻找可能出现转折的那一笔的位置； 包括，前包后、方向与原线段相反；不包括：后包前（因为有新高或新低）*/
			while  (biFormer < end - 2 &&  (IComparable::getDirection(*biFormer, *biLatter) == d || (*biFormer << *biLatter) || (*biFormer >> *biLatter)) )
			{
				CharacVecStack.push_back(CharacterVec(biFormer + 1, biLatter - 1));
				biFormer = biLatter;

				if (biLatter < end - 2)
				{
					biLatter += 2;
				}
				else
				{
					/* baseItem快到最后了 */
					if (!resultSet)
					{
						resultSet = new ContainerType();
					}
					resultSet->push_back(XianDuanClass(biStart, biLatter, d));
#ifdef DEBUG
					debugCnt++;
#endif
				}
			}

			if (biFormer >= end - 2)
			{
				break;
			}

			if (FXD_Case1(-d, biFormer + 1, end, CharacVecStack) == false)
			{
				/* 原线段延续*/
				biFormer = CharacVecStack.back().start - 1;
				biLatter = CharacVecStack.back().end + 1;
				
				
#ifdef DEBUG
				// 这段代码是用来调试死循环的。 
				if (((*biFormer) == (*oldBiFormer)) && ((*biLatter) == (*oldBiLatter)))
				{
					count++;
				}
				else
				{
					count = 0;
					oldBiFormer = biFormer;
					oldBiLatter = biLatter;
				}

				if (count == 4)
					printf("break me here\n");

				if (count == 5)
					break;
#endif

				if (biLatter >= end)  break;

				if (IComparable::getDirection(*biFormer, *biLatter) == d || (*biFormer >> *biLatter))
				{
					/*
                    情形1： biFormer 与 biLatter之间的方向， 与 原线段方向相同

                                                    /
                               /\                  /
                              /  \          /\  biLatter
                       /\ biFormer\        /  \  /
                      /  \  /      \      /    \/
                     /    \/        \    /
                    /                \  /        
                                      \/
                   |<  原线段 >|<-  特征向量 ->|<- 原线段延续 ->|

                   情形2： biFormer 包含 biLatter

                                 /\
                                /  \                  /\
                       /\      /    \        /\      /  \  /
                      /  \ biFormer  \      /  \ biLatter\/
                     /    \  /        \    /    \  /
                    /      \/          \  /      \/
                   /                    \/
                   
                   |<   原线段  >|<-  特征向量 ->|<- 新线段 ->|
                                     
            
					*/
					if (biLatter < end - 2)
					{
						CharacVecStack.pop_back();
						
#if 0						
						if (*biFormer >> *biLatter)
						{
							biFormer = biLatter;
							biLatter += 2;
						}
#endif
						continue;
					}
					else
					{
						/* baseItem快到最后了 */
						if (!resultSet)
						{
							resultSet = new ContainerType();
						}
						resultSet->push_back(XianDuanClass(biStart, biFormer, d));
#ifdef DEBUG
						debugCnt++;
#endif
						break;
					}

				} else
				{
					/*
                    情形3： biFormer 与 biLatter之间的方向， 与 原线段方向相反

                               /\                          /
                              /  \                  /\    /
                       /\ biFormer\        /\      /  \  /
                      /  \  /      \      /  \ biLatter\/
                     /    \/        \    /    \  /
                    /                \  /      \/
                                      \/
                   |<  原线段 >|<-  特征向量 ->|<- 新线段 ->|
                               |   形成新线段  |


                   情形4： biFormer 被 biLatter 包含

                                                      /\
                               /\                    /  \  /
                              /  \                  /    \/
                       /\ biFormer\        /\   biLatter
                      /  \  /      \      /  \    /
                     /    \/        \    /    \  /
                    /                \  /      \/
                                      \/
                   |<  原线段 >|<-  特征向量 ->|<- 原线段延续 ->|
                               |   形成新线段  |


					*/

					// 将特征向量 作为 一个新的线段
					if (!resultSet)
					{
						resultSet = new ContainerType();
					}
					// 原线段
					resultSet->push_back(XianDuanClass(biStart, biFormer, d));
					// 特征向量形成新线段
					resultSet->push_back(XianDuanClass(biFormer + 1, biLatter - 1, -d));
					// 新线段
					CharacVecStack.clear();
					biFormer = biStart = biLatter;
#if 0
					/* 新的线段，开始的部分，可能有一组互相包含的线段 */
					baseItemType temp = *biFormer;
					while (biFormer < end - 2)
					{
						if (temp >> *(biFormer + 2))
						{
							temp = *(biFormer + 2);
							biFormer += 2;
							continue;
						}
						else
							break;
					}
					if (biFormer < end - 2)
						biLatter = biFormer + 2;
#endif
					if (biLatter < end - 2)
						biLatter += 2;

				}

			} else
			{
				/* 原线段被破坏，将该线段添加到container中 */
				if (!resultSet)
				{
					resultSet = new ContainerType();
				}
				resultSet->push_back(XianDuanClass(biStart, biFormer, d));

				int debugEdgeCnt = biFormer - biStart + 1;

#ifdef DEBUG
				debugCnt++;
				if (debugCnt == 67)
					printf("break me here\n");
#endif

				/* 新线段需要 翻转方向 */
				d = -d;
				/* 新的线段的第一笔*/
				biStart = ++ biFormer;
				/* 新的线段的特征向量栈初始化 */
				CharacVecStack.clear();
#if 0
				/* 新的线段，开始的部分，有一组互相包含的线段，可以看成是之前线段的特征向量，可以应用包含关系。合并它们 */
				baseItemType temp = *biFormer;
				while (biFormer < end - 2)
				{
					if (temp >> *(biFormer + 2))
					{
						temp = *(biFormer + 2);
						biFormer += 2;
						continue;
					}
					else
						break;
				}
				biLatter = biFormer + 2;
#endif
				if (biFormer < end - 2)
					biLatter = biFormer + 2;
			}
		} while (1);

		if (backupBeforeNormalize)
		{
			XianDuanClass::baseItems->assign(backupBeforeNormalize->begin(), backupBeforeNormalize->end());
			delete backupBeforeNormalize;
		}

		return resultSet;
	}

	static ContainerType* startFenXianDuan_v2(baseItemIterator start, baseItemIterator end)
	{
		/*
		拐点的特征向量：每一个拐点右侧1笔，是该拐点的特征向量，如下图中的①②③、④⑤⑥分别是对应的拐点1、2、3、4、5、6的特征向量。

		底分型、底分型拐点：①的低点高于②的低点，并且 ③的高点、低点都要高于②的高点、低点，则拐点1、2、3构成 底分型，拐点2是 底分型拐点。 
		直观上来说，底分型就是：有3个向上的转折，并且位于中间的那个转折（拐点2）最低，并且右面那个转折向上1笔（拐点3特征向量）较中间那个转折向上1笔（拐点2特征向量）创出新高（即： 拐点3后的反弹高于拐点2后的反弹）

		\                  （拐点3后的反弹高于拐点2后的反弹）
		 \      /\              /
		  \    /  \      /\    /
		   \  ①   \    /  \  ③
		    \/      \  ②   \/
		  拐点1      \/    拐点3
		           拐点2
		    (拐点2是 底分型拐点)

		顶分型、顶分型拐点：④的高点低于⑤的高点，并且 ⑥的高点、低点都要低于⑤的高点、低点；则拐点4、5、6构成 顶分型，拐点5是 顶分型拐点
		直观上来说，顶分型就是：有3个向下的转折，并且位于中间的那个转折（拐点5）最高，并且右面那个转折向下1笔（拐点6特征向量）较中间那个转折向下1笔（拐点5特征向量）创出新低

			(拐点5是构成 顶分型拐点)
			       拐点5（分型拐点）
		拐点4         /\
		   /\        /  \
		  /  \      /    \      拐点6
		 /    ④   /     ⑤      /\
		/      \  /        \    /  \
		        \/          \  /   ⑥
				             \/      \
							          \
					              （拐点6右侧的1笔较拐点5右侧一笔创出新低）

		从上图中可以看出，若拐点构成顶/底分型，则该拐点的特征向量（②、⑤）的前后，都各存在1个特征向量（图中的①③、④⑥），这3个特征向量，满足分型的要求。

		特殊情况的处理： 特征向量的包含关系

		   分型拐点 左侧拐点的特征向量，按从左到右的顺序，可进行“前包后”的合并处理（因为后特征向量没有新高、新低，所以可以被前特征向量合并）。
		   如下图中，拐点3是分型拐点，其左侧的拐点1、拐点2的特征向量（⑦、⑧），存在“前包后”关系（⑦包含⑧），可进行合并处理。合并后的特征向量用“+”表示，合并后 拐点2消失了。拐点1、3、4构成顶分型，拐点3是顶分型拐点

		                           拐点3                                                      拐点3（顶分型拐点）
		       拐点1                 /\      拐点4                               拐点1                /\       拐点4
				/\        拐点2     /  \      /\                                  /+                 /  \      /\
		       /  \        /\      /    \    /  \           合并后               /    +             /    \    /  \
		      /    \      /  \    /      \  /    \       =============》        /        +         /      \  /    \
		     /     ⑦    /   ⑧  /        \/      \                            /            +     /        \/      \
		    /        \  /      \/                  \							  /                + /                  \
		   /          \/														 /          	
		  /																	/

		   如果合并后的拐点左侧没有其它拐点与其满足分型要求，那么该拐点肯定不是分型拐点。譬如，上图中拐点2被合并后，由于拐点1左侧没有低于它的拐点与之构成分型，所以拐点1一定不是分型拐点

		   分型拐点左侧的特征向量，按从左到右的顺序，如果是“后包前”，则 这两个特征向量不可进行包含处理，而是 作为独立的特征向量（因为后特征向量 有新高 和 新低）。譬如 特征向量④和⑤满足“后包前”，由于⑤创出新高，所以⑤是独立的特征向量。

		   拐点右侧的特征向量，按从左到右的顺序，可进行“前包后”的合并处理（因为后特征向量没有新高和新低），譬如下图中的⑨⑩满足“前包后”，合并后的特征向量用“+”表示，拐点3消失了。拐点1、2、4构成顶分型，拐点2是顶分型拐点

		             拐点2                                                         拐点2
		               /\             拐点4                                         /+           拐点4
		    拐点1     /  \     拐点3    /\                               拐点1     /   +          /\
		       /\    /   ⑨      /\    /  \                                 /\    /      +       /  \
		      /  \  /      \    / ⑩  /    \          合并后               /  \  /         +    /    \
		     /    \/        \  /    \/      \       =============》       /    \/            + /      \ 
	        /   拐点5        \/   拐点7      \                           /   拐点5           拐点7     \
		   /              拐点6                                         /

		   上图中，同时出现了 拐点1、2、4满足顶分型， 拐点5、6、7满足底分型的情况， 拐点2是顶分型拐点、拐点6是底分型拐点。拐点2、6之间，仅存在1笔相连接，而线段至少由3笔构成，如果拐点2、6同时成立，则其形成的线段少于3笔，因此，分型拐点2、拐点6 不能同时成立。
		   这时候，就要结合拐点1之前、拐点4之后的顶、底分布状况，来对 拐点2 和 拐点6 进行取舍； 原则无非就是： 两个顶、两个底不可以相邻。

		   一个可以适用的方式是： 先按照zig（30%）来划分出大的走势区间（这样能够识别出一些大画面的顶、底），然后再在各个走势区间内部 进行线段划分。 这样的好处是：每个走势区间的头、尾对应的顶、底分型是确定的，只需要找出其内部的各个顶、底分型就可以了。

		   
		 */
	}