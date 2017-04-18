#pragma once
#include "item.h"
#include "pointSeq.h"
class TrueSquishE
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


	TrueSquishE(pointSeq pS,double compression_rate);
	~TrueSquishE(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
};

