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
	vector <double> out_speed;//�ٶ�Ҫ���ǰ���ٶ�
	vector <double> out_increase_time;
	vector <double> out_distance;//����ֻҪ���ǰ����ʵ�ʾ��룬�������ۼƾ��룬û������
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

