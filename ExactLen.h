#pragma once
#include "item.h"
#include "pointSeq.h"
class ExactLen
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

	
	vector <double> out_level;
	vector <double> out_speed;//速度要输出前后速度
	vector <double> out_increase_time;
	vector <double> out_distance;//这里只要输出前后点的实际距离，而不是累计距离，没有意义
	vector <double> out_aera;
	vector <double> out_acceleration;
	

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

	ExactLen(pointSeq pS,double r,double l,double al,int isAdap);
	~ExactLen(void);
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

	void calcDistance();
};

