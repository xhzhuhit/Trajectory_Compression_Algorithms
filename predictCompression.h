#pragma once
#include "item.h"
#include "pointSeq.h"
class predictCompression
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


	predictCompression(pointSeq pS,double compression_rate,double priority_limit,double angle);
	~predictCompression(void);
	

	pair<double,double> getLocationL(double l);
	pair<double,double> getLocationT(double t);

	void calc();
	void calcArea();
	void calcDistance();
};

