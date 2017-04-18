#pragma once
#include "item.h"
#include "pointSeq.h"
class recovery
{
public:
	int number;
	vector <sItem> store;//sItem.order ��Ϊ0/1����ʾ�ٶ����ƺ��Ƿ���
	pointSeq ptSeq;
	vector <double> time;
	vector <double> length;
	double totalTime;
	double totalLength;


	double rate;
	double area;
	double speed_error;
	double length_error;


	recovery(pointSeq fixedRateSeq,pointSeq compressionSeq,double rate);
	~recovery(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
	void calcSpeedError();
	void calcLengthError();
	void calcError(pointSeq fixRateSeq,pointSeq compressionSeq);
};

