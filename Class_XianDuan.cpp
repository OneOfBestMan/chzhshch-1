#include "stdafx.h"
#include "Class_XianDuan.h"


Class_XianDuan<1>::baseItemType_Container* Class_XianDuan<1>::baseItems = (Class_XianDuan<1>::baseItemType_Container*)NULL;
Class_XianDuan<1>::ContainerType* Class_XianDuan<1>::container = (Class_XianDuan<1>::ContainerType*)NULL;


template<>
void FenXianDuan<typename Class_XianDuan<1>>(bool release)
{
	Class_XianDuan<1>::FenXianDuanClass::doWork(release);
}


