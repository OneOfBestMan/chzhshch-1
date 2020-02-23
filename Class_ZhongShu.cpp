#include "stdafx.h"
#include "Class_ZhongShu.h"



template<>
IZhongShu* createZhongShu<>(IZhongShu *zs, Class_XianDuan<1> *xianDuan)
{
	// 这个函数，仅仅用来通过语法检查
	assert(0); // 不能通过这种方式，来创建中枢0；中枢0，只能有线段1来创建；
	return NULL;
}