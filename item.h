#ifndef _ITEM_H_
#define _ITEM_H_
#include <string>
#include "utility.h"
using namespace std;


class item// point - all attributes
{
public:
	int number;
	int id;
	string vin;
	string obdId;
	double lng;
	double lat;
	double speed;
	int engineSpeed;
	char gpsStat;
	Time obdTime;
	Time serverTime;
	void output(ofstream &outFile);
};
class sItem// simplified version of point
{
public:
	//string obdId;
	int order;
	double lng;
	double lat;
	double acceleration;//tianjia
	double speed;
	Time obdTime;
	void output(ofstream &outFile);
	
	double priority;
	double add_priority;
};


#endif