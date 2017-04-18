#pragma once
#include "item.h"
#include "pointSeq.h"
class SimpleSeq
{
public:
	int number;
	int isAdaptive;
	vector <sItem> store;
	pointSeq ptSeq;
	vector <double> time;
	vector <double> length;
	double totalTime;
	double totalLength;

	
	

	double timeUnit;
	double L;
	double alpha;
	int nRR;
	double RR;

	double nAD;
	int number_AD;
	double AD;

	double area;

	double AL;
	double SD;
	double SDL;

	SimpleSeq(pointSeq pS,double r,double l,double al,int isAdap);
	~SimpleSeq(void);
	void setTimeUnit(double r);
	void setL(double l);
	void setAlpha(double al);

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcRR();
	void calcAD();
	void calcArea();
	void calcAL();
	void calcSD();
	void calcSDL();
};

