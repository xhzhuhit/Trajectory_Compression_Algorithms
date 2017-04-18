#pragma once
#include "item.h"
#include "pointSeq.h"
class speedCompression
{
public:
	int number;
	vector <sItem> store;//sItem.order 标为0/1来表示速度限制后是否保留
	pointSeq ptSeq;
	vector <double> time;
	vector <double> length;
	double totalTime;
	double totalLength;


	double rate;
	double area;


	speedCompression(pointSeq pS,double sped,double compression_rate,int compression_time,double priority_limit);
	~speedCompression(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
};

