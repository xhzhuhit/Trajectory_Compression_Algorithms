#include "recovery.h"


recovery::recovery(pointSeq fixedRateSeq,pointSeq compressionSeq,double rate)
{
	
	ptSeq = fixedRateSeq;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	
	int size = compressionSeq.store.size();
/*	for(int i = 0;i < compressionSeq.store.size()-1;i++)
	{
		store.push_back(compressionSeq.store[i]);
		double deta_time = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
		int number = int(deta_time / rate);
		if(number > 1)//大于1说明中间应该插入点，按顺序计算出来，并且存到新轨迹里面
		{
			for(int j = 0;j < number-1;j++)
			{
				sItem tmp;
				tmp.order = 1;
				tmp.priority = 1;
				tmp.acceleration = 0;//前三项无关紧要
				//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
				tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat) / (compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime) ;
				tmp.obdTime = compressionSeq.store[i].obdTime + (j+1) * rate;
				//SED方式
				tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+1].lng - compressionSeq.store[i].lng) / number * (j + 1);
				tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+1].lat - compressionSeq.store[i].lat) / number * (j + 1);

				store.push_back(tmp);
			}
		}
	}
*/
	int start = 0;
	for(int i = 0;i < compressionSeq.store.size()-1;i++)
	{
		store.push_back(compressionSeq.store[i]);
		double t1 = compressionSeq.store[i].obdTime - compressionSeq.store[0].obdTime;
		double t2 = compressionSeq.store[i+1].obdTime - compressionSeq.store[0].obdTime;
		for(int j = start; j < ptSeq.number; j++)
		{
			if(ptSeq.time[j] > t1 + 0.0001 && ptSeq.time[j] < t2 - 0.0001)
			{
				double t3 = ptSeq.store[j].obdTime - compressionSeq.store[i].obdTime;
				double t4 = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
				//计算出f复原点
				sItem tmp;
				tmp.order = 1;
				tmp.priority = 1;
				tmp.acceleration = 0;//前三项无关紧要
				//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
				tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat) / (compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime) ;
				tmp.obdTime = compressionSeq.store[i].obdTime + t3;
				//SED方式
				tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+1].lng - compressionSeq.store[i].lng) / t4 * t3;
				tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+1].lat - compressionSeq.store[i].lat) / t4 * t3;

				store.push_back(tmp);
				
				
				continue;
			}
			if(ptSeq.time[j] >= t2 - 0.0001)
			{
				start = j;
				break;
			}
		}
	}
	store.push_back(compressionSeq.store[size-1]);
	
	//for(int i = 0;i < store.size();i++)
	//{
	//	debuout<<store[i].lng<<" "<<store[i].lat<<" "<<store[i].obdTime.hour<<":"<<store[i].obdTime.minute<<":"<<store[i].obdTime.second<<endl;
	//}

	//for(int i = 0;i < compressionSeq.store.size();i++)
	//{
	//	debuout<<compressionSeq.store[i].lng<<" "<<compressionSeq.store[i].lat<<" "<<compressionSeq.store[i].obdTime.hour<<":"<<compressionSeq.store[i].obdTime.minute<<":"<<compressionSeq.store[i].obdTime.second<<endl;
	//}
}

recovery::~recovery(void)
{
}

pair<double,double> recovery::getLocationL(double l)
{
	int i;
	for(i = 0; i < number - 1; i ++)
	{
		if(length[i] <= l && length[i+1] >= l)
			break;
	}
	double s0 = l-length[i];
	double s = length[i+1] - length[i];
	double lng,lat;
	lng = store[i].lng+s0/s*(store[i+1].lng - store[i].lng);
	lat = store[i].lat+s0/s*(store[i+1].lat - store[i].lat);
	return make_pair(lng,lat);
}

pair<double,double> recovery::getLocationT(double t)
{
	int i;
	for(i = 0; i < number - 1; i ++)
	{
		if(time[i] <= t && time[i+1] >= t)
			break;
	}
	double t0 = t - time[i];
	double lng,lat;
	lng = store[i].lng+t0/rate*(store[i+1].lng - store[i].lng);
	lat = store[i].lat+t0/rate*(store[i+1].lat - store[i].lat);
	return make_pair(lng,lat);
}

void recovery::calc()
{
	number = store.size();
	time.push_back(0);
	length.push_back(0);
	for(int i = 0;i < number - 1; i ++ )
	{
		double ti = store[i+1].obdTime - store[i].obdTime;
		totalTime += ti;
		time.push_back(totalTime);

		double s = Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		totalLength += s;
		length.push_back(totalLength);
	}
}

void recovery::calcArea()
{
	double a = 0;
	int start = 0;
	for(int i = 0; i < store.size() - 1; i ++)
	{
		double t1 = store[i].obdTime - store[0].obdTime;
		double t2 = store[i + 1].obdTime - store[0].obdTime;
		vector <pair<double,double> > middlePoint;
		for(int j = start; j < ptSeq.number; j ++ ) 
		{
			if(ptSeq.time[j] > t1 + 0.0001 && ptSeq.time[j] < t2 - 0.0001)
			{
				middlePoint.push_back(make_pair(ptSeq.store[j].lng,ptSeq.store[j].lat));
				continue;
			}
			if(ptSeq.time[j] >= t2 - 0.0001)
			{
				start = j;
				break;
			}
		}

		if(middlePoint.size() != 0 )
		{
			double temp = countArea_new(make_pair(store[i].lng,store[i].lat),make_pair(store[i+1].lng,store[i+1].lat),middlePoint);
			//cout<< temp <<"("<<c<<")  ";
			//if(store[i].obdTime - store[0].obdTime < 50 || store[store.size()-1].obdTime - store[i].obdTime < 50)
			a +=temp;
			
		}
	}
	area = a;
}

void recovery::calcDistance()
{
	double a = 0;
	int start = 0;
	int c =0;
	for(int i = 0; i < store.size() - 1; i ++)
	{
		double dist = 0;

		double t1 = store[i].obdTime - store[0].obdTime;
		double t2 = store[i + 1].obdTime - store[0].obdTime;
		vector <pair<double,double> > middlePoint;
		for(int j = start; j < ptSeq.number; j ++ )
		{
			if(ptSeq.time[j] > t1 + 0.0001 && ptSeq.time[j] < t2 - 0.0001)
			{
				middlePoint.push_back(make_pair(ptSeq.store[j].lng,ptSeq.store[j].lat));
				continue;
			}
			if(ptSeq.time[j] >= t2 - 0.0001)
			{
				start = j;
				break;
			}
		}

		if(middlePoint.size() != 0 )
		{
			int size = middlePoint.size();
			if(middlePoint.size() == 1)
				dist = Distance(store[i].lng,store[i].lat,middlePoint[0].first,middlePoint[0].second) + Distance(middlePoint[0].first,middlePoint[0].second,store[i+1].lng,store[i+1].lat);
			else
			{
				dist += Distance(store[i].lng,store[i].lat,middlePoint[0].first,middlePoint[0].second);
				dist += Distance(middlePoint[0].first,middlePoint[0].second,store[i+1].lng,store[i+1].lat);
				for(int m = 0;m < size-1;m++)
				{
					dist += Distance(middlePoint[m].first,middlePoint[m].second,middlePoint[m+1].first,middlePoint[m+1].second);
				}
			}
		}
		else
			dist += Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
	}
}

void recovery::calcSpeedError()
{
	double error = 0;
	//if(store.size() != ptSeq.store.size())
		//cout<<store.size()<<"  "<<ptSeq.store.size()<<endl;
	for(int i = 0;i < store.size() && i < ptSeq.store.size();i++)
	{
		double err = abs(ptSeq.store[i].speed - store[i].speed);
		error += err;
	}
	speed_error = error;
}

void recovery::calcLengthError()
{
	double error = 0;
	for(int i = 0;i < store.size() && i < ptSeq.store.size();i++)
	{
		double err = Distance(store[i].lng,store[i].lat,ptSeq.store[i].lng,ptSeq.store[i].lat);
		error += err;
	}
	length_error = error;
}

void recovery::calcError(pointSeq fixRateSeq,pointSeq compressionSeq)
{
	double error = 0;
	double l_error = 0;
	double s_error = 0;
	int size = compressionSeq.store.size();
	int start = 0;
	for(int i = 0;i < compressionSeq.store.size()-1;i++)
	{
		double t1 = compressionSeq.store[i].obdTime - compressionSeq.store[0].obdTime;
		double t2 = compressionSeq.store[i+1].obdTime - compressionSeq.store[0].obdTime;
		for(int j = start; j < ptSeq.number; j++)
		{
			if(ptSeq.time[j] > t1 + 0.0001 && ptSeq.time[j] < t2 - 0.0001)
			{
				//middlePoint.push_back(make_pair(ptSeq.store[j].lng,ptSeq.store[j].lat));
				//计算出映射点
				vector <double> tmmp = ptSeq.calcProjection(compressionSeq.store[i],ptSeq.store[j],compressionSeq.store[i+1]);
				//vector <double> tmmp = ptSeq.calcSed(compressionSeq.store[i],ptSeq.store[j],compressionSeq.store[i+1]);
				
				l_error += sqrt(tmmp[0] * tmmp[0] + tmmp[1] * tmmp[1]);
				s_error += tmmp[2];
				continue;
			}
			if(ptSeq.time[j] >= t2 - 0.0001)
			{
				start = j;
				break;
			}
		}
	}
	
	speed_error = s_error;
	length_error = l_error;
}