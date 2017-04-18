#include "squishExtend.h"


squishExtend::squishExtend(pointSeq pS,double compression_rate,int compression_time,double priority_limit)
{
	
	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	
	double rate = 10;

	vector <sItem> temp_buffer;
	//每5分钟选择保留一定点数，保证压缩比 
	int start_q_size = 0;
	int size = 0;
	bool flag = false;
	int set_buffer_size = int(compression_rate * (compression_time * 60 / rate))+2;
	for(int i = 0;i < pS.store.size();i++)
	{
		//没到规定数目点或者结尾，不断执行压入，删除的动作
		//每个buffer第一次到达规定数目时的priority纯靠计算得到
		if(start_q_size == set_buffer_size)
		{
			//计算优先级，即距离
			for(int j = 1;j < temp_buffer.size()-1;j++)
			{
				double t1 = temp_buffer[j].obdTime - temp_buffer[j-1].obdTime;
				double t2 = temp_buffer[j+1].obdTime - temp_buffer[j-1].obdTime;
				temp_buffer[j].priority = SEDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat,t1,t2);//点到线的距离
			}
			//第二点若是从上一段剩下来的，则要加上自己的add_priority
			//temp_buffer[1].priority += temp_buffer[1].add_priority;
		}

		if( size != compression_time * 60 / rate)
		{
			//if(temp_buffer.size() == set_buffer_size)
			{
				while(temp_buffer.size() == set_buffer_size)
				{
					//cout<<"in while "<<temp_buffer.size()<<endl;
					//遍历寻找最小优先级点
					int weizhi = 0;
					double youxianji = 10000;
					for(int j = 1;j < temp_buffer.size()-1;j++)
					{
						if(temp_buffer[j].priority < youxianji)
						{
							youxianji = temp_buffer[j].priority;
							weizhi = j;
						}
					}
					//删除最小优先级点
					const vector<sItem>::iterator it = temp_buffer.begin() + weizhi;
					temp_buffer.erase(it);

					//接下来更新前后
					if(weizhi >= 2 && temp_buffer.size() - weizhi >= 2)
					{
						double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
						double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//优先级重新更新


						double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
						double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//优先级重新更新
					}

					if(weizhi == 1 && temp_buffer.size() - weizhi >= 2)//挨着第一个点，那前面的就不用计算了
					{
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}


						double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
						double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//优先级重新更新
					}
					
					if(weizhi >= 2 && temp_buffer.size() - weizhi == 1)//挨着最后一个点，那后面的就不用计算了
					{
						double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
						double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//优先级重新更新


						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
					}
				}
				//cout<<"njbvbhebhqdbhbqhbqhdbqhqbed"<<endl;
			}
		}

		//到达规定点数，把最后两个点留下，其他放入新轨迹
		if(size == compression_time * 60 / rate)
		{
			//cout<<size<<" "<<temp_buffer.size()<<endl;
			//接下来做存储
			for(int j = 0;j < temp_buffer.size()-2;j++)
			{
				store.push_back(temp_buffer[j]);
			}
			sItem before_last_sitem = temp_buffer[temp_buffer.size()-2];
			sItem last_sitem = temp_buffer[temp_buffer.size()-1];
			temp_buffer.clear();
			temp_buffer.push_back(before_last_sitem);
			temp_buffer.push_back(last_sitem);
			size = 0;//需要30个
			start_q_size = 2;//为什么是0
			//flag = false;
		}

		//到达末尾，buffer里面所有点放到新轨迹
		if( i == pS.store.size()-1 && size != compression_time * 60 / rate)
		{
			//先删除
			int needSize = start_q_size * compression_rate;
			if(needSize <= 4)
			{
				store.push_back(temp_buffer[0]);
				store.push_back(temp_buffer[temp_buffer.size()-1]);
				temp_buffer.clear();
				size = 0;
			}
			else
			{
				while(temp_buffer.size() > needSize)
				{
					//cout<<"in while "<<temp_buffer.size()<<endl;
					//遍历寻找最小优先级点
					int weizhi = 0;
					double youxianji = 10000;
					for(int j = 1;j < temp_buffer.size()-1;j++)
					{
						if(temp_buffer[j].priority < youxianji)
						{
							youxianji = temp_buffer[j].priority;
							weizhi = j;
						}
					}
					//删除最小优先级点
					const vector<sItem>::iterator it = temp_buffer.begin() + weizhi;
					temp_buffer.erase(it);

					//接下来更新前后
					if(weizhi >= 2 && temp_buffer.size() - weizhi >= 2)
					{
						double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
						double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//优先级重新更新


						double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
						double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//优先级重新更新
					}

					if(weizhi == 1 && temp_buffer.size() - weizhi >= 2)//挨着第一个点，那前面的就不用计算了
					{
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}


						double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
						double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//优先级重新更新
					}
					
					if(weizhi >= 2 && temp_buffer.size() - weizhi == 1)//挨着最后一个点，那后面的就不用计算了
					{
						double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
						double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
						if(youxianji >= temp_buffer[weizhi-1].add_priority)
						{
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//优先级重新更新
							temp_buffer[weizhi-1].add_priority = youxianji;
						}
						else
							temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//优先级重新更新


						if(youxianji >= temp_buffer[weizhi].add_priority)
						{
							temp_buffer[weizhi].priority = youxianji;//优先级重新更新
							temp_buffer[weizhi].add_priority = youxianji;
						}
					}
				}
				for(int j = 0;j < temp_buffer.size();j++)
				{
					store.push_back(temp_buffer[j]);
				}
				temp_buffer.clear();
				size = 0;
			}
		}

		temp_buffer.push_back(pS.store[i]);
		size++;
		start_q_size++;
		int buffer_size = temp_buffer.size();
		if(buffer_size == int(compression_rate * (compression_time * 60 / rate))+2)
		{
			double t1 = temp_buffer[buffer_size-2].obdTime - temp_buffer[buffer_size-3].obdTime;
			double t2 = temp_buffer[buffer_size-1].obdTime - temp_buffer[buffer_size-3].obdTime;
			temp_buffer[buffer_size-2].priority = SEDistance(temp_buffer[buffer_size-3].lng,temp_buffer[buffer_size-3].lat,temp_buffer[buffer_size-2].lng,temp_buffer[buffer_size-2].lat,temp_buffer[buffer_size-1].lng,temp_buffer[buffer_size-1].lat,t1,t2) + temp_buffer[buffer_size-2].add_priority;//点到线的距离
		}
	}

	//到这里就得到一条完整的新轨迹
}

squishExtend::~squishExtend(void)
{
}

pair<double,double> squishExtend::getLocationL(double l)
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

pair<double,double> squishExtend::getLocationT(double t)
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

void squishExtend::calc()
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

void squishExtend::calcArea()
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

void squishExtend::calcDistance()
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