#include "item.h"
void item::output(ofstream &outFile)
{
	outFile<<id<<","<<setw(17)<<vin<<","<<obdId<<","<<setw(20)<<setprecision(15)<<lng<<","<<setw(20)<<lat<<","<<setw(5)<<speed<<","<<setw(7)<<engineSpeed<<","<<gpsStat<<",";
	outFile<<obdTime.year<<"-"<<obdTime.month<<"-"<<obdTime.day<<" "<<obdTime.hour<<":"<<obdTime.minute<<":"<<obdTime.second<<",";
	outFile<<serverTime.year<<"-"<<serverTime.month<<"-"<<serverTime.day<<" "<<serverTime.hour<<":"<<serverTime.minute<<":"<<serverTime.second<<endl;
}

void sItem::output(ofstream &outFile)
{
	//outFile<<obdId<<","<<setw(20)<<setprecision(15)<<lng<<","<<setw(20)<<lat<<","<<setw(5)<<speed<<",";
	//outFile<<obdId<<","<<setw(20)<<setprecision(15)<<lng<<","<<setw(20)<<lat<<","<<setw(5)<<speed<<",";
	//outFile<<obdTime.year<<"-"<<obdTime.month<<"-"<<obdTime.day<<" "<<obdTime.hour<<":"<<obdTime.minute<<":"<<obdTime.second<<endl;
	outFile<<"#"<<setw(4)<<setfill('0')<<order<<","<<obdTime.year<<"-"<<obdTime.month<<"-"<<obdTime.day<<" ";
	outFile<<obdTime.hour<<":"<<obdTime.minute<<":"<<obdTime.second<<" ,";
	outFile<<setprecision(10)<<lng<<","<<setprecision(10)<<lat<<","<<setprecision(5)<<speed<<endl;
}
