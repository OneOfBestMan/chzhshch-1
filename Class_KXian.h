#pragma once

#ifndef CLASS_KXIAN_H
#define CLASS_KXIAN_H

class Class_KXian
{
public:
	Class_KXian(int pos);
	~Class_KXian(void);

	float getHigh();
	float getLow();
private:
	int pos;
};

#endif