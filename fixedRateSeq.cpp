#include "fixedRateSeq.h"


fixedRateSeq::fixedRateSeq(pointSeq pS,double r,double l,double al)
{
	setRate(r);
	setL(l);
	setAlpha(al);
	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	nRR = 0;
	RR = 0;
	nAD = 0;
	AD = 0;
	number_AD = 0;
	area = 0;
	AL = 0;
	SD = 0;
	SDL = 0;

/*	double t;
	for( t = 0; t < ptSeq.totalTime; t += rate)
	{
		//if(t>181)
		//	cout<<endl;
		sItem tmp;
		tmp.order = ptSeq.store[0].order;
		tmp.obdTime = ptSeq.store[0].obdTime + t;
		pair<double,double> temp = ptSeq.getLocationT(t);
		tmp.lng = temp.first;
		tmp.lat = temp.second;
		tmp.acceleration = ptSeq.getAcc(t);
		tmp.speed = ptSeq.getSpeed(t);
		tmp.priority = 0;
		tmp.add_priority = 0;
		store.push_back(tmp);

		out_increase_time.push_back(rate);//increase time
	}
	out_increase_time.pop_back();
	{
		double seeg = ptSeq.totalTime - t + rate;
		t = ptSeq.totalTime;
		sItem tmp;
		tmp.order = ptSeq.store[0].order;
		tmp.obdTime = ptSeq.store[0].obdTime + t;
		pair<double,double> temp = ptSeq.getLocationT(t);
		tmp.lng = temp.first;
		tmp.lat = temp.second;
		tmp.acceleration = ptSeq.getAcc(t);
		tmp.speed = ptSeq.getSpeed(t);
		tmp.priority = 0;
		tmp.add_priority = 0;
		store.push_back(tmp);

		out_increase_time.push_back(seeg);
	}
*/
	for(int i = 0;i < ptSeq.store.size();i++)//相当于没处理，稀疏数据
	{
		store.push_back(ptSeq.store[i]);
	}
}

void fixedRateSeq::setRate(double r)
{
	rate = r;
}
void fixedRateSeq::setL(double l)
{
	L = l;
}
void fixedRateSeq::setAlpha(double al)
{
	alpha = al;
}

fixedRateSeq::~fixedRateSeq(void)
{
}

pair<double,double> fixedRateSeq::getLocationL(double l)
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

pair<double,double> fixedRateSeq::getLocationT(double t)
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


void fixedRateSeq::calc()
{
	number = store.size();
	time.push_back(0);
	length.push_back(0);
	for(int i = 0;i < number - 1; i ++ )
	{
		double ti = store[i+1].obdTime - store[i].obdTime;
		totalTime += ti;
		time.push_back(totalTime);

		out_speed.push_back(store[i].speed);//sudu cunchu

		double s = Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		totalLength += s;
		length.push_back(totalLength);
	}
	out_speed.push_back(store[number-1].speed);//终点速度
}

void fixedRateSeq::calcRR()
{
	int count = 0;
	for(int i = 0;i < store.size()-1; i++)
	{
		count += Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat)< alpha*L;
	}
	nRR = count;
	RR = (double)count/store.size();
}

void fixedRateSeq::calcAD()
{
	int L1 = 10;
	int i;
	double counter = 0;
	for(i = 0; i < (int)(ptSeq.totalLength/L1); i ++ )
	{
		//if(i==8)
		//	cout<<endl;
		double t = ptSeq.l2t(i*L1);
		counter += Distance(getLocationT(t).first,getLocationT(t).second,ptSeq.getLocationL(i*L1).first,ptSeq.getLocationL(i*L1).second);
	}
	double t = totalTime - 0.1;
	counter += Distance(getLocationT(t).first,getLocationT(t).second,ptSeq.getLocationT(t).first,ptSeq.getLocationT(t).second);
	number_AD = (int)(ptSeq.totalLength/L1) + 1;
	nAD = counter;
	AD = counter/number_AD/L1;
}
void fixedRateSeq::calcArea()
{
	double a = 0;
	int start = 0;
	int c =0;
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
		
		if(middlePoint.size() == 0)
		{
			out_aera.push_back(0);//面积存储
			//cout<<"area : "<<endl;
		}

		if(middlePoint.size() != 0 )
		{
			c++;
			if(c == 153)
				cout<<"";
			double temp = countArea_new(make_pair(store[i].lng,store[i].lat),make_pair(store[i+1].lng,store[i+1].lat),middlePoint);
			//cout<< temp <<"("<<c<<")  ";
			//if(store[i].obdTime - store[0].obdTime < 50 || store[store.size()-1].obdTime - store[i].obdTime < 50)
			a +=temp;
			//分阶段把面积存起来
			int segment = store[i].speed / 2;//每两米每秒一个阶段
			if(segment < 0)
				segment = 0;
			if(segment > 19)
				//segment = store[i-1].speed / 2;
				segment = 19;
			aera_segment[segment] += temp;
			count_segment[segment] += 1;

			out_aera.push_back(temp);
			//if(temp == 0)
				//cout<<"nima"<<endl;
		}
	}
	area = a;
}

void fixedRateSeq::calcAL()
{
	AL = length[number - 1]/(number - 1);
}

void fixedRateSeq::calcSD()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SD += (length[i] - length[i-1] - AL)*(length[i] - length[i-1] - AL);
	}
	SD /= number - 1;
}



void fixedRateSeq::calcSDL()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SDL += (length[i] - length[i-1] - L)*(length[i] - length[i-1] - L);
	}
	SDL /= number - 1;
}

void fixedRateSeq::calcDistance()
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
		out_distance.push_back(dist);
	}
}