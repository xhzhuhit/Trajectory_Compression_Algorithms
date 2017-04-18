#include "utility.h"
#include "config.h"
#include "item.h"
#include "pointSeq.h"
#include "fixedRateSeq.h"
#include "recovery.h"
#include "squishExtend.h"
#include "ErrorAndRate.h"
#include "tdtr.h"
#include "AdaptiveAccumulation.h"
#include "arcRecovery.h"
#include "TrueSquishE.h"
#include "douglas.h"
#include "uniform.h"

void parseData(string &str, sItem &data)
{
	vector<string> st = splitEx_NoEmpty(str,",+-=: ");
	sItem temp;

	temp.order = 0;
	temp.lng = atof(st[1].c_str());
	temp.lat = atof(st[2].c_str());
	temp.speed = atof(st[3].c_str())/3.6;


	temp.obdTime.year = atoi(st[4].c_str());
	temp.obdTime.month = atoi(st[5].c_str());
	temp.obdTime.day = atoi(st[6].c_str());
	temp.obdTime.hour = atoi(st[7].c_str());
	temp.obdTime.minute = atoi(st[8].c_str());
	temp.obdTime.second = atof(st[9].c_str());

	temp.priority = 0;
	temp.add_priority = 0;
	data = temp;
}
vector <pointSeq> readData(ConfigType cr,int fileOrder)
{
	const char* path=getConfigStr("path",cr);
	const char* pathSeq=getConfigStr("path_MDP",cr);
	const char* pathReport=getConfigStr("path_MDPr",cr);
	//const double L = getConfigFloat("L",cr);
	const double alpha = getConfigFloat("alpha",cr);
	const double timeUnit = getConfigFloat("timeUnit",cr);

	//read the file
	char inFileName[100];
	sprintf(inFileName,"%s%05d.txt",path,fileOrder);

	//½«Êý¾Ý¶ÁÈë£¬´æ´¢ÔÚdata
	vector <pointSeq> data;//ÒÑ¾­È«¾Ö
	pointSeq tmp;
	ifstream inFile;
	inFile.open(inFileName,fstream::in);
	char line[LINE_LEN];
	inFile.getline(line,LINE_LEN);
	inFile.getline(line,LINE_LEN);
	while(inFile.getline(line,LINE_LEN))
	{
		string str(line);
		if(str[0]=='#')
		{
			if(tmp.store.size() >= 300)
				data.push_back(tmp);
			tmp.clear();
		}
		else
		{
			sItem tmpItem;
			parseData(str,tmpItem);
			tmp.store.push_back(tmpItem);
		}
	}
	inFile.close();

	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	return data;
}
vector <pointSeq> countFixedRateSeq(double rate,int fileOrder,vector <pointSeq> data)
{
	double L = 100;
	double alpha = 0.9;
	
	vector <pointSeq> fixrate_seq;
	for(int i = 0; i < data.size(); i++)
	{
		fixedRateSeq dataSeq(data[i],rate,L,alpha);
		//dataSeq.calc();

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		fixrate_seq.push_back(tmmp);
	}
	return fixrate_seq;
}
pair <vector <comp_result>,vector <pointSeq> > squishExtend_process(double compression_rate,int compression_time,vector <pointSeq> data,double priority_limit)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squishExtend_seq;
	for(int i = 0; i < data.size(); i++)
	{
		squishExtend dataSeq(data[i],compression_rate,compression_time,priority_limit);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squishExtend_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squishExtend_seq);
	return resul;
}
pair <vector <comp_result>,vector <pointSeq> > ErrorAndRate_process(double compression_rate,int compression_time,vector <pointSeq> data,double priority_limit)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squishExtend_seq;
	for(int i = 0; i < data.size(); i++)
	{
		ErrorAndRate dataSeq(data[i],compression_rate,compression_time,priority_limit);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squishExtend_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squishExtend_seq);
	return resul;
}
pair <vector <comp_result>,vector <pointSeq> > tdtr_process(double tdtr_length_limit,vector <pointSeq> data)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squish_seq;
	for(int i = 0; i < data.size(); i++)
	{
		tdtr dataSeq(data[i],tdtr_length_limit);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用面积/长度来衡量，即平均偏离
		//comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用函数计算得来，挨点计算求平均,转移到recovery函数
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squish_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squish_seq);
	return resul;
}
pair <vector <comp_result>,vector <pointSeq> > AdaptiveAccumulation_process(double compression_rate,int compression_time,vector <pointSeq> data,double priority_limit)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squishExtend_seq;
	for(int i = 0; i < data.size(); i++)
	{
		AdaptiveAccumulation dataSeq(data[i],compression_rate,compression_time,priority_limit);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squishExtend_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squishExtend_seq);
	return resul;
}
pair <vector <double>,vector <double> > recovery_process(vector <pointSeq> fixedRateSeq,vector <pointSeq> compressionSeq,double rate)
{
	for(int i = 0; i < fixedRateSeq.size(); i++)
		fixedRateSeq[i].calc();
	for(int i = 0; i < compressionSeq.size(); i++)
		compressionSeq[i].calc();

	pair <vector <double>,vector <double> > result;
	vector <double> length_error;
	vector <double> speed_error;

	for(int i = 0; i < compressionSeq.size(); i++)
	{
		recovery dataSeq(fixedRateSeq[i],compressionSeq[i],rate);
		dataSeq.calc();
		//dataSeq.calcArea();
		dataSeq.calcSpeedError();
		dataSeq.calcLengthError();
		//dataSeq.calcError(fixedRateSeq[i],compressionSeq[i]);//一次性计算出速度，距离的xyt误差

		length_error.push_back(dataSeq.length_error/dataSeq.store.size());
		speed_error.push_back(dataSeq.speed_error/dataSeq.store.size());
	}
	//cout<<"max length :"<<max_length_error<<"    min length :"<<min_length_error<<endl;
	//cout<<"max speed  :"<<max_speed_error<<"    min speed  :"<<min_speed_error<<endl;

	return make_pair(length_error,speed_error);
}
pair <vector <double>,vector <double> > arcRecovery_process(vector <pointSeq> fixedRateSeq,vector <pointSeq> compressionSeq,double rate,double sed_limit,double set_angle_recovery)
{
	for(int i = 0; i < fixedRateSeq.size(); i++)
		fixedRateSeq[i].calc();
	for(int i = 0; i < compressionSeq.size(); i++)
		compressionSeq[i].calc();

	pair <vector <double>,vector <double> > result;
	vector <double> length_error;
	vector <double> speed_error;

	ofstream debuout("arcRecovery.txt");

	ofstream rout("arcrecover.txt");

	for(int i = 0; i < compressionSeq.size(); i++)
	{
		arcRecovery dataSeq(fixedRateSeq[i],compressionSeq[i],rate,sed_limit,set_angle_recovery,debuout);
	/*	for(int j = 0;j < dataSeq.store.size() && j < fixedRateSeq[i].store.size();j++)
		{
			if(dataSeq.store[j].obdTime == fixedRateSeq[i].store[j].obdTime)
			{
				double speed_error = abs(dataSeq.store[j].speed - fixedRateSeq[i].store[j].speed);
				double juli = Distance(dataSeq.store[j].lng,dataSeq.store[j].lat,fixedRateSeq[i].store[j].lng,fixedRateSeq[i].store[j].lat);
				if(speed_error > max_speed_error)
					max_speed_error = speed_error;
				if(speed_error < min_speed_error)
					min_speed_error = speed_error;
				if(juli > max_length_error)
					max_length_error = juli;
				if(juli < min_length_error)
					min_length_error = juli;
			}
		}
	*/
		dataSeq.calc();
		//dataSeq.calcArea();
		dataSeq.calcSpeedError();
		dataSeq.calcLengthError();
		//dataSeq.calcError(fixedRateSeq[i],compressionSeq[i]);//一次性计算出速度，距离的xyt误差
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			rout<<setprecision(10)<<dataSeq.store[j].lng<<" ";
		}
		rout<<endl;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			rout<<setprecision(10)<<dataSeq.store[j].lat<<" ";
		}
		rout<<"######"<<endl;
		length_error.push_back(dataSeq.length_error/dataSeq.store.size());
		speed_error.push_back(dataSeq.speed_error/dataSeq.store.size());
	}
	//cout<<"max length :"<<max_length_error<<"    min length :"<<min_length_error<<endl;
	//cout<<"max speed  :"<<max_speed_error<<"    min speed  :"<<min_speed_error<<endl;

	return make_pair(length_error,speed_error);
}
pair <vector <comp_result>,vector <pointSeq> > true_squishe_process(double compression_rate,vector <pointSeq> data)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squish_seq;
	for(int i = 0; i < data.size(); i++)
	{
		TrueSquishE dataSeq(data[i],compression_rate);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用面积/长度来衡量，即平均偏离
		//comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用函数计算得来，挨点计算求平均,转移到recovery函数
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squish_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squish_seq);
	return resul;
}
pair <vector <comp_result>,vector <pointSeq> > douglas_process(double douglas_length_limit,vector <pointSeq> data)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squish_seq;
	for(int i = 0; i < data.size(); i++)
	{
		douglas dataSeq(data[i],douglas_length_limit);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用面积/长度来衡量，即平均偏离
		//comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用函数计算得来，挨点计算求平均,转移到recovery函数
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squish_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squish_seq);
	return resul;
}
pair <vector <comp_result>,vector <pointSeq> > uniform_process(double compression_rate,vector <pointSeq> data)
{
	for(int i = 0; i < data.size(); i++)
		data[i].calc();

	vector <comp_result> result;
	vector <pointSeq> squish_seq;
	for(int i = 0; i < data.size(); i++)
	{
		uniform dataSeq(data[i],compression_rate);
		dataSeq.calc();
		dataSeq.calcArea();
		//cout<<dataSeq.area<<" "<<dataSeq.totalLength<<endl;
		comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用面积/长度来衡量，即平均偏离
		//comp_result res = comp_result(dataSeq.area/dataSeq.totalLength,data[i].store.size(),dataSeq.number);//这一行偏离距离用函数计算得来，挨点计算求平均,转移到recovery函数
		result.push_back(res);

		pointSeq tmmp;
		for(int j = 0;j < dataSeq.store.size();j++)
		{
			tmmp.store.push_back(dataSeq.store[j]);
		}
		squish_seq.push_back(tmmp);
	}
	pair <vector <comp_result>,vector <pointSeq> > resul = make_pair(result,squish_seq);
	return resul;
}
void check_squishExtend(double compression_rate,int compression_time,double priority_limit,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_squishExtend.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);

	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	
	//cout<<"done fixed rate"<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = squishExtend_process(compression_rate,compression_time,ptseq1,priority_limit);
	//cout<<"done squishExtend"<<endl;
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	for(int i = 0;i < result.first.size();i++)
	{
		//total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
	}
	//recovery
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<total_error_speed * 3.6 / vec_recovery.first.size()<<" "<<total_error_length / vec_recovery.first.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_ErrorAndRate(double compression_rate,int compression_time,double priority_limit,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_ErrorAndRate.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);

	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	ofstream ffout("fixedrate.txt");
	//for(int i = 0;i < ptseq1.size();i++)
	int i = 15;
	{
		for(int j = 0;j < ptseq1[i].store.size();j++)
		{
			//ffout<<j<<", "<<ptseq1[i].store[j].lng<<", "<<setprecision(10)<<ptseq1[i].store[j].lat<<", "<<ptseq1[i].store[j].speed<<", ";
			//ffout<<ptseq1[i].store[j].obdTime.year<<"-"<<ptseq1[i].store[j].obdTime.month<<"-"<<ptseq1[i].store[j].obdTime.day<<" ";
			//ffout<<ptseq1[i].store[j].obdTime.hour<<":"<<ptseq1[i].store[j].obdTime.minute<<":"<<ptseq1[i].store[j].obdTime.second<<endl;
			ffout<<setprecision(10)<<ptseq1[i].store[j].lng<<" ";
		}
		ffout<<"####"<<endl;
		for(int j = 0;j < ptseq1[i].store.size();j++)
		{
			ffout<<setprecision(10)<<ptseq1[i].store[j].lat<<" ";
		}
	}

	//cout<<"done fixed rate"<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = ErrorAndRate_process(compression_rate,compression_time,ptseq1,priority_limit);
	//cout<<"done squishExtend"<<endl;

	//shuchu
	ofstream fout("errorandrate.txt");
	//for(int i = 0;i < result.second.size();i++)
	//int i = 0;
	{
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" ";
			//fout<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].priority - result.second[i].store[j].add_priority<<" ";
			//fout<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lng<<" ";
		}
		fout<<"####"<<endl;
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lat<<" ";
		}
	
	}
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	for(int i = 0;i < result.first.size();i++)
	{
		//total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
	}
	//recovery
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	//pair <vector <double>,vector <double> > vec_recovery = arcRecovery_process(ptseq1,result.second,rate,sed_limit,set_angle_recovery);//弯道复原

	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<compression_rate<<" "<<priority_limit<<" ";
	outAdapFixedCheck<<total_error_speed * 3.6 / vec_recovery.first.size()<<" "<<total_error_length / vec_recovery.first.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_tdtr(double tdtr_length_limit,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_tdtr_sed.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);
	
	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	//for(int i = 0;i < data.size();i++)
	//{
	//	cout<<ptseq1[i].store.size()<<" "<<data[i].store.size()<<endl;
	//}
	//cout<<ptseq1[0].store.size()<<endl;
	//cout<<ptseq1[0].totalTime<<endl;
	//cout<<"done fixed rate"<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = tdtr_process(tdtr_length_limit,ptseq1);
	//cout<<"done squish"<<endl;
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	ofstream fout("tdtr.txt");
	//for(int i = 0;i < result.second.size();i++)
	int i = 16;
	{
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lng<<" ";
		}
		fout<<"####"<<endl;
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lat<<" ";
		}
	}

	for(int i = 0;i < result.first.size();i++)
	{
//		total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
		//cout<<result.first[i].number_comp<<" "<<result.first[i].number_fix<<endl;
	}
	//recovery
	//char oufile[100];
	//ofstream error_out;
	//sprintf(oufile,"error.txt");
	//error_out.open(oufile,ios::out);

	
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
	//	cout<<i<<"  "<<vec_recovery.first[i]<<endl;
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<tdtr_length_limit<<" "<<total_error_speed * 3.6 / vec_recovery.second.size()<<" "<<total_error_length / vec_recovery.second.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_AdaptiveAccumulation(double compression_rate,int compression_time,double priority_limit,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_AdaptiveAccumulation.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);

	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	ofstream ffout("fixrat.txt");
	//for(int i = 0;i < ptseq1.size();i++)
	int i = 16;
	{
		for(int j = 0;j < ptseq1[i].store.size();j++)
		{
			//ffout<<j<<", "<<ptseq1[i].store[j].lng<<", "<<setprecision(10)<<ptseq1[i].store[j].lat<<", "<<ptseq1[i].store[j].speed<<", ";
			//ffout<<ptseq1[i].store[j].obdTime.year<<"-"<<ptseq1[i].store[j].obdTime.month<<"-"<<ptseq1[i].store[j].obdTime.day<<" ";
			//ffout<<ptseq1[i].store[j].obdTime.hour<<":"<<ptseq1[i].store[j].obdTime.minute<<":"<<ptseq1[i].store[j].obdTime.second<<endl;
			ffout<<setprecision(10)<<ptseq1[i].store[j].lng<<" ";
		}
		ffout<<"####"<<endl;
		for(int j = 0;j < ptseq1[i].store.size();j++)
		{
			ffout<<setprecision(10)<<ptseq1[i].store[j].lat<<" ";
		}
	}

	//cout<<"done fixed rate"<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = AdaptiveAccumulation_process(compression_rate,compression_time,ptseq1,priority_limit);
	//cout<<"done squishExtend"<<endl;

	//shuchu
	ofstream fout("AdaptiveAccumulation.txt");
	//for(int i = 0;i < result.second.size();i++)
	//int i = 3;
	{
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" ";
			//fout<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].priority - result.second[i].store[j].add_priority<<" ";
			//fout<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lng<<" ";
		}
		fout<<"####"<<endl;
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lat<<" ";
		}
	}

	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	for(int i = 0;i < result.first.size();i++)
	{
		//total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
	}
	//recovery
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<compression_rate<<" "<<priority_limit<<" ";
	outAdapFixedCheck<<total_error_speed * 3.6 / vec_recovery.first.size()<<" "<<total_error_length / vec_recovery.first.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_uniform(double compression_rate,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_uniform_sed.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);
	
	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	//cout<<ptseq1[0].store.size()<<endl;
	//cout<<ptseq1[0].totalTime<<endl;
	//cout<<"done fixed rate"<<endl;
	//cout<<ptseq1[0].store.size()<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = uniform_process(compression_rate,ptseq1);
	//cout<<"done squish"<<endl;
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	ofstream fout("uniform.txt");
	//for(int i = 0;i < result.second.size();i++)
	int i = 0;
	{
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lng<<" ";
		}
		fout<<"####"<<endl;
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lat<<" ";
		}
	}

	for(int i = 0;i < result.first.size();i++)
	{
//		total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
		//cout<<result.first[i].number_comp<<" "<<result.first[i].number_fix<<endl;
	}
	//recovery
	//char oufile[100];
	//ofstream error_out;
	//sprintf(oufile,"error.txt");
	//error_out.open(oufile,ios::out);

	
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<compression_rate<<" "<<total_error_speed * 3.6 / vec_recovery.second.size()<<" "<<total_error_length / vec_recovery.second.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_true_squishe(double compression_rate,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_truesquishe.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);
	
	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	//cout<<ptseq1[0].store.size()<<endl;
	//cout<<ptseq1[0].totalTime<<endl;
	//cout<<"done fixed rate"<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = true_squishe_process(compression_rate,ptseq1);
	//cout<<"done squish"<<endl;
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	for(int i = 0;i < result.first.size();i++)
	{
//		total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
		//cout<<result.first[i].number_comp<<" "<<result.first[i].number_fix<<endl;
	}
	//recovery
	//char oufile[100];
	//ofstream error_out;
	//sprintf(oufile,"error.txt");
	//error_out.open(oufile,ios::out);

	//vector <pointSeq> ptseq2 = countFixedRateSeq(1,fileOrder,data);

	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq1,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<total_error_speed * 3.6 / vec_recovery.second.size()<<" "<<total_error_length / vec_recovery.second.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
void check_douglas(double douglas_length_limit,double rate,int fileOrder,ConfigType cr)
{
	ofstream outAdapFixedCheck;
	char inFileName[150];
	const char* pathResult=getConfigStr("path_result",cr);
	sprintf(inFileName,"%s%04d_result_douglas_sed.txt",pathResult,fileOrder);
	outAdapFixedCheck.open(inFileName,ios::app);
	vector <pointSeq> data = readData(cr,fileOrder);
	
	//fixedrate
	vector <pointSeq> ptseq1 = countFixedRateSeq(rate,fileOrder,data);
	//cout<<ptseq1[0].store.size()<<endl;
	//cout<<ptseq1[0].totalTime<<endl;
	//cout<<"done fixed rate"<<endl;
	//cout<<ptseq1[0].store.size()<<endl;
	//speed compression
	pair <vector <comp_result>,vector <pointSeq> > result = douglas_process(douglas_length_limit,ptseq1);
	//cout<<"done squish"<<endl;
	double total_error_length = 0;
	double total_error_speed = 0;
	double total_rate_decrease = 0;

	ofstream fout("douglas.txt");
	//for(int i = 0;i < result.second.size();i++)
	int i = 0;
	{
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lng<<" ";
		}
		fout<<"####"<<endl;
		for(int j = 0;j < result.second[i].store.size();j++)
		{
			//fout<<j<<" "<<result.second[i].store[j].lng<<" "<<result.second[i].store[j].lat<<" "<<result.second[i].store[j].speed<<" ";
			//fout<<result.second[i].store[j].obdTime.year<<"-"<<result.second[i].store[j].obdTime.month<<"-"<<result.second[i].store[j].obdTime.day<<" ";
			//fout<<result.second[i].store[j].obdTime.hour<<":"<<result.second[i].store[j].obdTime.minute<<":"<<result.second[i].store[j].obdTime.second<<endl;
			fout<<setprecision(10)<<result.second[i].store[j].lat<<" ";
		}
	}

	for(int i = 0;i < result.first.size();i++)
	{
//		total_error_length += result.first[i].length;
		total_rate_decrease += result.first[i].number_comp * 0.99999999 / result.first[i].number_fix;
		//cout<<result.first[i].number_comp<<" "<<result.first[i].number_fix<<endl;
	}
	//recovery
	//char oufile[100];
	//ofstream error_out;
	//sprintf(oufile,"error.txt");
	//error_out.open(oufile,ios::out);
	vector <pointSeq> ptseq2 = countFixedRateSeq(1,fileOrder,data);
	
	pair <vector <double>,vector <double> > vec_recovery = recovery_process(ptseq2,result.second,rate);
	for(int i = 0;i < vec_recovery.first.size();i++)
	{
		total_error_length += vec_recovery.first[i];
		total_error_speed += vec_recovery.second[i];
	}
	//cout<<"done recovery"<<endl;
	//cout<<total_error_speed<<" "<<total_error_length<<" "<<vec_recovery.size()<<endl;
	outAdapFixedCheck<<douglas_length_limit<<" "<<total_error_speed * 3.6 / vec_recovery.second.size()<<" "<<total_error_length / vec_recovery.second.size()<<" "<<total_rate_decrease / result.first.size()<<endl;
	//outAdapFixedCheck<<total_error_length / vec_recovery.first.size()<<" ";
	outAdapFixedCheck.close();
}
int main(int argc, char* argv[])
{
	srand(time(NULL));
	//!read the config file
	ConfigType cr;
	AddConfigFromFile(cr,"config.txt");
	//AddConfigFromCmdLine(cr,argc,argv);
	ListConfig(cr);
	//analysisAV(cr);
	//double sped = 12;//要寻找最合适的值固定下来
	//double compression_rate = 0.11;
	double compression_time = 5;//要固定下来
	double priority_limit0 = 12;
	double priority_limit1 = 12;//随着需求变化，并且受压缩率的影响导致山间的点数发生变化,5m/yushe 14m
	double priority_limit2 = 10;
	double rate = 1;//要固定下来
	//double compression_rate[5]  = {1.0,1.5,2,2.5,3};
	double compression_rate[5]  = {1.0,2.5,5,7.5,10};
	//double compression_rate[5]  = {30,20,15,12,10};
	//double priority_limit[5] = {0,0.00005,0.035,0.45,0.6};//wash
	//double priority_limit[5] = {0,0.00002,0.030,0.42,0.55};1:0.5:3
	double priority_limit[5] = {0,0.00002,0.030,4,3};
	//double priority_limit[7] = {0,0,3,0,9,7,17};
	//double priority_limit[7] = {0,0,11,15,11,11,9};//sed
	//double priority_limit[7] = {0,4,11,3,1,11,13};//xyt
	//squish里面要更新rate
	//int fileOrder = 0;
	for(int fileOrder = 0; fileOrder < 1; fileOrder++)
	{
		//cout<<fileOrder<<endl;

		//for(double sped = 7.5;sped < 14.5;sped += 0.5)
		//for(double compression_time = 3;compression_time <= 17;compression_time += 1)
		//for(double priority_limit = 0;priority_limit < 20;priority_limit += 0.1)
		//for(double compression_rate = 0.01;compression_rate <= 0.1;compression_rate += 0.005)
		//for(double angle = 0.0174;angle <= 0.696;angle += 0.0174)
		for(int i = 0;i < 5;i++)
		{
			//check_squishExtend(1.0/compression_rate[i] * 10,compression_time,priority_limit0,rate,fileOrder,cr);
			//check_predict_compression(compression_rate,priority_limit,angle,rate,fileOrder,cr);
			//check_speed_compression(sped,compression_rate,compression_time,priority_limit,rate,fileOrder,cr);
			//check_squish(0.99999999/compression_rate[i],compression_time,rate,fileOrder,cr);
			//for(double priority_limit = 0;priority_limit < 5;priority_limit += 1)
			//for(double sed_limit = 10;sed_limit < 100.1;sed_limit += 10)
			{
				//double sed_limit = 8;
				{
					//check_ErrorAndRate(1.0/compression_rate[i],compression_time,priority_limit1,rate,fileOrder,cr);
					check_AdaptiveAccumulation(1.0/compression_rate[i],compression_time,priority_limit[i],rate,fileOrder,cr);//wash
				}
			}
		}
		
		//double tdtr_length_limit[7] = {0,57,143,233,295,387,461};//sed,xyt
		//double tdtr_length_limit[5] = {0,0.2,0.8,2,2.5};//1:0.5:3
		double tdtr_length_limit[5] = {0,0.8,13,27.4,40};
		//for(double tdtr_length_limit = 35;tdtr_length_limit < 40;tdtr_length_limit += 0.1)
		for(int i = 0;i < 5;i++)
		//double tdtr_length_limit = 0;
		{
			check_tdtr(tdtr_length_limit[i],rate,fileOrder,cr);
		}

		//double douglas_length_limit[5] = {22.15,11.48,7.72,5.91,4.66};//high
		//double douglas_length_limit[5] = {28.97,14.57,7.50,3.7,0};//low
		//double douglas_length_limit[5] = {0,0.1,0.3,1.5,2};//1:0.5:3
		double douglas_length_limit[5] = {0,0.8,12,25.1,34.9};
		//for(double douglas_length_limit = 25;douglas_length_limit < 35;douglas_length_limit += 0.1)
		for(int i = 0;i < 5;i++)
		{
			check_true_squishe(1.0/compression_rate[i],rate,fileOrder,cr);
			check_douglas(douglas_length_limit[i],rate,fileOrder,cr);
			check_uniform(compression_rate[i],rate,fileOrder,cr);
		}
	}
	return 0;
}