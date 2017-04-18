#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <time.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <deque>

#define _NUM 12
#define _NUM2 144

using namespace std;

const int LINE_LEN = 1024;
const double PI = 3.1415926535823846;
const double R = 6371004.0; // radius of earth

class Time
{
public:
	int year;
	int month;
	int day;

	int hour;
	int minute;
	double second;

	Time(int = 0,int = 0, int = 0,int = 0,int = 0,double = 0);

	friend Time operator+(Time t,double gap);
	friend Time operator-(Time t,double gap);
	friend double operator-(Time t1,Time t2);
	friend bool operator<(Time t1,Time t2);
	friend bool operator>(Time t1,Time t2);
	friend bool operator==(Time t1,Time t2);

	double getSecond();//From 1970
	Time getTime(double gap);
	void setTime(double gap);
	bool isLeap();// 是否闰年
	void showTime();
};

struct point
{
    double x,y;
};

class result
{
public:
	int number;
	double aveLength;
	double area;
	result(int a, double b,double c)
	{
		number = a;
		aveLength = b;
		area = c;
	}
};

class comp_result
{
public:
	double length;
	int number_fix;
	int number_comp;
	comp_result(double a, int b,int c)
	{
		length = a;
		number_fix = b;
		number_comp = c;
	}
};

class simple_point
{
public:
	double lng;
	double lat;
	double speed;
	simple_point(double a, double b,double c)
	{
		lng = a;
		lat = b;
		speed = c;
	}
};

vector<string> splitEx(const string& src, string separate_character);
vector<string> splitEx_NoEmpty(const string& src, string separate_character);
double Distance(double lng1,double lat1,double lng2,double lat2);
int v2level(double v);
int a2level(double a);
double level2v(int level);
double level2a(int level);
int checkIfExist(int presentLevel,int D[]);
int getLevel(double presentV,double presentA);
int getNextLevel(int presentLevel,int matrix[][_NUM2],int D[]);
int getNextLevel(int presentLevel,int matrix[][_NUM2],int D[],int totalHappen[][_NUM2]);
double getSingleStayTime(int presentLevel,map<int,int> singleStayTime[],int totalHappen[][_NUM2],double timeUnit);
double getStayTime(int presentLevel,int nextLevel,map<int,int> stayTime[][_NUM2],int totalHappen[][_NUM2],double timeUnit);
pair<double,double> getVA(int nextLevel);
double polygon_areas( vector<point> p);
double countArea_old(pair<double,double> start,pair<double,double> end, vector <pair<double,double> > middle);
double countArea_new(pair<double,double> start,pair<double,double> end, vector <pair<double,double> > middle);
double cross(point p1,point p2,point p3);
point intersection(point p1,point p2,point p3,point p4);
double integral(point p1,point p2,point p3,point p4,double x,double y);
//vector<int> seatDistribution(int totalN1,vector<int> segNumber,int Number);//按照比例席位分配

double LineDistance(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3);
double SEDistance(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3,double t1,double t2);
double calcAngle(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3);
#endif
