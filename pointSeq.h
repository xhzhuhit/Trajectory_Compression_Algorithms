#pragma once
#include "item.h"
class pointSeq
{
public:
	int number;//������Ŀ
	vector <sItem> store;
	vector <double> time;//��¼�����еĸ���ʱ��
	double totalTime;//��ʱ��
	
	//a,b,c�������ʱ�� v = at^2 + bt +c
	vector <double> a;
	vector <double> b;
	vector <double> c;

	vector <double> raw_deta_length;
	vector <double> raw_deta_time;
	vector <double> length;//��¼�����еĸ�������
	double totalLength;//�ܳ���

	pointSeq(void);
	~pointSeq(void);
	void clear();
	void calc();//����a,b,c,time,length��ز���
	double getAcc(double t);//time -> Acc
	pair<double,double> getLocationT(double t);//time -> point
	pair<double,double> getLocationL(double l);//length -> point
	double l2t(double l);//length -> time
	double getSpeed(double t); //time -> speed
	
	double caDistance(sItem p1,sItem p2);
	vector <double> calcProjection(sItem p1,sItem p2,sItem p3);

	simple_point calcArcPoint(sItem p1,sItem p2,int time,double angle_recovery,int number);
};

