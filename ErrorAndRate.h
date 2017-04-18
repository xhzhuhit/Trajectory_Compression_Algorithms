#pragma once
#include "item.h"
#include "pointSeq.h"
class ErrorAndRate
{
public:
	int number;
	vector <sItem> store;
	pointSeq ptSeq;
	vector <double> time;
	vector <double> length;
	double totalTime;
	double totalLength;


	double rate;
	double area;


	ErrorAndRate(pointSeq pS,double compression_rate,int compression_time,double priority_limit);
	~ErrorAndRate(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
};

