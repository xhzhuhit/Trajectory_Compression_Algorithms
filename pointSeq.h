#pragma once
#include "item.h"
class pointSeq
{
public:
	int number;//点总数目
	vector <sItem> store;
	vector <double> time;//记录点序列的各个时间
	double totalTime;//总时间
	
	//a,b,c用于拟合时， v = at^2 + bt +c
	vector <double> a;
	vector <double> b;
	vector <double> c;

	vector <double> raw_deta_length;
	vector <double> raw_deta_time;
	vector <double> length;//记录点序列的各个长度
	double totalLength;//总长度

	pointSeq(void);
	~pointSeq(void);
	void clear();
	void calc();//计算a,b,c,time,length相关参数
	double getAcc(double t);//time -> Acc
	pair<double,double> getLocationT(double t);//time -> point
	pair<double,double> getLocationL(double l);//length -> point
	double l2t(double l);//length -> time
	double getSpeed(double t); //time -> speed
	
	double caDistance(sItem p1,sItem p2);
	vector <double> calcProjection(sItem p1,sItem p2,sItem p3);

	simple_point calcArcPoint(sItem p1,sItem p2,int time,double angle_recovery,int number);
};

