#pragma once

#ifndef IDISPLAYABLE_H
#define IDISPLAYABLE_H

class Class_KXian;

class IDisplayable
{
public:
	/* 这两个接口， 获取 类笔、笔、线段 的开始、结束K线，在indicator中会用得着 */
	virtual Class_KXian* getStartRec() = 0;
	virtual Class_KXian* getEndRec() = 0;
};

#endif