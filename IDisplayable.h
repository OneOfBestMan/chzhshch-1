#pragma once

#ifndef IDISPLAYABLE_H
#define IDISPLAYABLE_H

class Class_KXian;

class IDisplayable
{
public:
	/* �������ӿڣ� ��ȡ ��ʡ��ʡ��߶� �Ŀ�ʼ������K�ߣ���indicator�л��õ��� */
	virtual Class_KXian* getStartRec() = 0;
	virtual Class_KXian* getEndRec() = 0;
};

#endif