#pragma once
#include "item.h"
#include "pointSeq.h"
class uniform
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


	uniform(pointSeq pS,double tdtr_length_limit);
	~uniform(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
};

