#include "tdtr.h"


tdtr::tdtr(pointSeq pS,double tdtr_length_limit)
{
	
	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;

	//double rate = 10;

	int size = ptSeq.store.size();
	vector <sItem> temp_buffer;
	temp_buffer.push_back(ptSeq.store[0]);
	temp_buffer.push_back(ptSeq.store[size-1]);
	ptSeq.store[0].priority = 1;
	ptSeq.store[size-1].priority = 1;

	int compare_size = temp_buffer.size();//size不变化就停止处理
	while(true)
	{
		int before_start = 0;
		int start = 0;
		for(int i = 0;i < temp_buffer.size()-1;i++)
		{
			before_start = start;
			double t1 = temp_buffer[i].obdTime - temp_buffer[0].obdTime;
			double t2 = temp_buffer[i+1].obdTime - temp_buffer[0].obdTime;
			//对两点之间的片段计算sed
			for(int j = start; j < ptSeq.store.size(); j++)
			{
				if(ptSeq.time[j] > t1 && ptSeq.time[j] < t2)
				{
					double t1 = ptSeq.store[j].obdTime - temp_buffer[i].obdTime;
					double t2 = temp_buffer[i+1].obdTime - temp_buffer[i].obdTime;
					double sed = SEDistance(temp_buffer[i].lng,temp_buffer[i].lat,ptSeq.store[j].lng,ptSeq.store[j].lat,temp_buffer[i+1].lng,temp_buffer[i+1].lat,t1,t2);
					ptSeq.store[j].add_priority = sed;
				}

				if(ptSeq.time[j] == t2)//等于的点开始，考虑到没有时间大于最后一点
				{
					start = j;
					break;
				}
			}
			//标注完了之后，寻找最大的
			int weizhi = 0;
			double max_add_priority = tdtr_length_limit;
			for(int j = before_start + 1;j < start;j++)
			{
				if(ptSeq.store[j].add_priority >= max_add_priority)
				{
					max_add_priority = ptSeq.store[j].add_priority;
					weizhi = j;
				}
			}
			//找到中间最大一点后，进行标记//要大于界限才会标记
			if(max_add_priority >= tdtr_length_limit)
				ptSeq.store[weizhi].priority = 1;
		}
		//处理过一遍之后，把temp_buffer清空，然后把标记为1的重新压入
		temp_buffer.clear();
		for(int i = 0;i < ptSeq.store.size();i++)
		{
			if(ptSeq.store[i].priority == 1)
				temp_buffer.push_back(ptSeq.store[i]);
		}
		//存储完之后check一下size，相同则退出，不同则赋值
		if(compare_size == temp_buffer.size())
			break;
		else
			compare_size = temp_buffer.size();
	}
	//结束之后，把temp_buffer抄给store
	for(int i = 0; i < temp_buffer.size();i++)
	{
		store.push_back(temp_buffer[i]);
	}
	//得到整条轨迹，打完收工
}

tdtr::~tdtr(void)
{
}

pair<double,double> tdtr::getLocationL(double l)
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

pair<double,double> tdtr::getLocationT(double t)
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

void tdtr::calc()
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

void tdtr::calcArea()
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

void tdtr::calcDistance()
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