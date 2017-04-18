#pragma once
#include "item.h"
#include "pointSeq.h"
class fixedRateSeq
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
	double L;
	double alpha;
	int nRR;
	double RR;

	double aera_segment[20];//分速度段查看面积，2m/s为一段
	int count_segment[20];

	vector <double> out_distance;
	vector <double> out_aera;
	vector <double> out_speed;//速度要输出前后速度
	vector <double> out_increase_time;

	double nAD;
	int number_AD;
	double AD;

	double area;

	double AL;
	double SD;
	double SDL;

	fixedRateSeq(pointSeq pS,double r,double l,double al);
	~fixedRateSeq(void);
	void setRate(double r);
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

