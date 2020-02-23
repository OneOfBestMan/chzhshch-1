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

  �������ϵ��߶�˵�� TurningPoint(TP)��ָ �����ĵ㣺 TP1��TP2

               TP2       /
                /\      /
     TP1       /  \    /
      /\      /    \  /
     /  \    /      \/
    /    \  /
   /      \/
  /
 
 �������µ��߶Σ�TurningPoint��ָ �����ĵ㣺TP1��TP2
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
  �������ϵ��߶�˵�� Juxtaposition(JP)��ָ ���������ڣ������غ�����ģ��߶�: JP0/JP1/JP2/JP3 ��  JP2/JP3/JP4/JP5

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

  �������µ��߶�˵�� Juxtaposition��ָ ���������ڣ������غ�����ģ��߶�: JP0/JP1/JP2/JP3 ��  JP2/JP3/JP4/JP5

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

  �������е��߶�(��������߶η��������ϵ�)��Juxtaposition��ָ�� JP0/JP1/JP2/JP3/JP4/JP5
   
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

 �������е��߶�(��������߶η��������µ�)��Juxtaposition��ָ�� JP0/JP1/JP2/JP3
   
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