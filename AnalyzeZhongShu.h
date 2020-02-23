#ifndef ANALYZEZHONGSHU_H
#define ANALYZEZHONGSHU_H




template <class XianDuanClass>
class AnalyzeZhongShu_Template
{
public:
	typedef typename XianDuanClass::baseItemType_Container::iterator baseItemIterator;
	typedef typename XianDuanClass::baseItemType baseItemType;
	typedef typename XianDuanClass::ContainerType ContainerType;
	typedef typename XianDuanClass::baseItemType_Container baseItemType_Container;


	void handleTurningPoint()
	{
/*

  对于向上的线段说， TurningPoint(TP)是指 这样的点： TP1、TP2

               TP2       /
                /\      /
     TP1       /  \    /
      /\      /    \  /
     /  \    /      \/
    /    \  /
   /      \/
  /
 
 对于向下的线段，TurningPoint是指 这样的点：TP1、TP2
  \
   \        /\
    \      /  \
	 \    /    \        /\
	  \  /      \      /  \
	   \/        \    /    \
      TP1         \  /      \
                   \/        \
                  TP2         \
                               \
*/


	}

	void handleJuxtaposition()
	{
/*
  对于向上的线段说， Juxtaposition(JP)是指 这样的相邻（且有重合区域的）线段: JP0/JP1/JP2/JP3 、  JP2/JP3/JP4/JP5

                           /\
                          /  \      /
                         /   JP5   /
                /\     JP4     \  /
               /  \    /        \/
      /\     JP2  JP3 /
     /  \    /      \/
   JP0  JP1 /
   /      \/
  /

  对于向下的线段说， Juxtaposition是指 这样的相邻（且有重合区域的）线段: JP0/JP1/JP2/JP3 、  JP2/JP3/JP4/JP5

  \
   \      /\
   JP0  JP1 \          
	 \  /    \        /\
      \/    JP2     JP3 \
               \    /    \          /
                \  /      \        /
                 \/       JP4    JP5
                            \    /
                             \  /
                              \/

  对于下列的线段(如果父亲线段方向是向上的)，Juxtaposition是指： JP0/JP1/JP2/JP3/JP4/JP5
   
                                 /\
                                /  \
                               /    \
                /\            /      \
     /\        /  \          /        \
    /  \      /    \        /          \
  JP0  JP1  JP2    JP3    JP4          JP5
  /      \  /        \    /              \
          \/          \  /                \
                       \/

 对于下列的线段(如果父亲线段方向是向下的)，Juxtaposition是指： JP0/JP1/JP2/JP3
   
                                 /\
                                /  \
                               /    \
                /\            /      \
     /\        /  \          /        \
    /  \      /    \        /          \
   /  JP0   JP1    JP2    JP3           \
  /      \  /        \    /              \
          \/          \  /                \
                       \/

*/

	}

	void doWork() 
	{
		handleTurningPoint();
		handleJuxtaposition();
	}
};



template <class XianDuanClass>
AnalyzeZhongShu_PostOrder()
{
	typedef typename XianDuanClass::baseItemType baseItemType;

	AnalyzeZhongShu_PostOrder<baseItemType>();
	AnalyzeZhongShu_Template<XianDuanClass>();
}



#endif