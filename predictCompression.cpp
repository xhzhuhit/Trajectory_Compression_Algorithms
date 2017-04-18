#include "predictCompression.h"


predictCompression::predictCompression(pointSeq pS,double compression_rate,double priority_limit,double angle)//angle need be gave
{
	
	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	

	deque <sItem> temp_buffer;
	//先压入两点//这两点离太近显然不好，那就取多点做一次压缩
	temp_buffer.push_back(pS.store[0]);
	temp_buffer.push_back(pS.store[10]);
	for(int i = 11;i < pS.store.size()-1;i++)
	{
		// sample predict
		double deta_acc1 = abs(temp_buffer[1].speed - temp_buffer[0].speed) / (temp_buffer[1].obdTime - temp_buffer[0].obdTime);
		double deta_t_sample = pS.store[i].obdTime - temp_buffer[1].obdTime;
		double length_max_sample = temp_buffer[1].speed * deta_t_sample + 0.5 * deta_acc1 * pow(deta_t_sample,2);
		double length_min_sample = temp_buffer[1].speed * deta_t_sample - 0.5 * deta_acc1 * pow(deta_t_sample,2);

		double angle_sample = calcAngle(temp_buffer[0].lng,temp_buffer[0].lat,temp_buffer[1].lng,temp_buffer[1].lat,pS.store[i].lng,pS.store[i].lat);
		double distance_sample = Distance(temp_buffer[1].lng,temp_buffer[1].lat,pS.store[i].lng,pS.store[i].lat);
		bool flag_sample = false;
		//if(distance_sample >= length_min_sample && distance_sample <= length_max_sample && angle_sample < angle)
		if(angle_sample < angle)
			flag_sample = true;

		//trajectory predict
		double deta_acc2 = abs(pS.store[i-1].speed - pS.store[i-2].speed) / (pS.store[i-1].obdTime - pS.store[i-2].obdTime);
		double deta_t_tr = pS.store[i].obdTime - pS.store[i-1].obdTime;
		double length_max_tr = pS.store[i-1].speed * deta_t_tr + 0.5 * deta_acc2 * pow(deta_t_tr,2);
		double length_min_tr = pS.store[i-1].speed * deta_t_tr - 0.5 * deta_acc2 * pow(deta_t_tr,2);

		double angle_tr = calcAngle(pS.store[i-2].lng,pS.store[i-2].lat,pS.store[i-1].lng,pS.store[i-1].lat,pS.store[i].lng,pS.store[i].lat);
		double distance_tr = Distance(pS.store[i-1].lng,pS.store[i-1].lat,pS.store[i].lng,pS.store[i].lat);
		bool flag_tr = false;
		//if(distance_tr >= length_min_tr && distance_tr <= length_max_tr && angle_tr < angle)
		if(angle_tr < angle)
			flag_tr = true;

		//decide maintain or not
		if(!(flag_sample && flag_tr))//取或的效果反而有机会！！！！且的效果不好
		//if(!flag_sample)
		//if(!flag_tr)
		{
			store.push_back(temp_buffer[0]);
			temp_buffer.pop_front();
			temp_buffer.push_back(pS.store[i]);
		}
	}
	store.push_back(pS.store[pS.store.size()-1]);
}

predictCompression::~predictCompression(void)
{
}

pair<double,double> predictCompression::getLocationL(double l)
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

pair<double,double> predictCompression::getLocationT(double t)
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

void predictCompression::calc()
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

void predictCompression::calcArea()
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

void predictCompression::calcDistance()
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