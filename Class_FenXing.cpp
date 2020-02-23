#include "stdafx.h"
#include "Class_FenXing.h"

/*
分型

方法：
  方向（根据 k线1、k线2 高低位置判断）

属性：
  k线1
  k线2

*/

Class_FenXing::Class_FenXing(int pos, type in) : bar(pos), t(in)
{

}

Class_FenXing::~Class_FenXing(void)
{
}
