#pragma once


#ifndef CLASS_FENXING_H
#define CLASS_FENXING_H


class Class_FenXing
{
public:
	enum type {
		TOP = 0,  // 顶分型
		BOT = 1   // 底分型
	};


	Class_FenXing(int pos, type t);  
	Class_FenXing(): bar(-1), t( (type) -1 ) {}

	~Class_FenXing(void);

	int getPos() {return bar;}
	type getType() {return t;}
private:
	int bar;
	type  t;
};

#endif