#include "speedCompression.h"


speedCompression::speedCompression(pointSeq pS,double sped,double compression_rate,int compression_time,double priority_limit)
{
	double k = sped;
	sped = 12;

	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	

	vector <sItem> temp_buffer;
	//按速度筛选出要保留的点
/*	double start_speed = pS.store[0].speed;
	pS.store[0].order = 1;
	for(int i = 0;i < pS.store.size();i++)
	{
		if(abs(pS.store[i].speed - start_speed) >= sped)
		{
			pS.store[i].order = 1;
			start_speed = pS.store[i].speed;
		}
		else
			pS.store[i].order = 0;
	}
	pS.store[pS.store.size()-1].order = 1;
*/

/*	double start_speed = pS.store[0].speed;
	pS.store[0].order = 1;//first point resereved

	double middle_speed = 0;
	int middle_weizhi = 0;
	//search middle point
	for(int i = 0;i < pS.store.size();i++)
	{
		if(abs(pS.store[i].speed - start_speed) >= sped)
		{
			middle_weizhi = i;
			middle_speed = pS.store[i].speed;
			break;
		}
	}
	//renew
	int st = middle_weizhi+1;
	for(int i = st;i < pS.store.size();i++)
	{
		if(abs(pS.store[i].speed - middle_speed) >= sped)//get the maybe point
		{
			if((middle_speed - start_speed) * (pS.store[i].speed - middle_speed) < 0)
			{
				pS.store[middle_weizhi].order = 1;
			}
			start_speed = middle_speed;
			middle_speed = pS.store[i].speed;
			middle_weizhi = i;
		}
	}
	pS.store[pS.store.size()-1].order = 1;//last point resereved
*/
	//每5分钟选择保留一定点数，保证压缩比 
	int start_mark = 0;
	int size = 0;
	bool flag = true;//确认不是所有点都是速度限制点
	for(int i = start_mark;i < pS.store.size();i++)
	{
		//没到规定数目点或者结尾，不断执行压入，删除的动作
		if( size != compression_time * 6 && flag)
		{
			if(temp_buffer.size() == int(compression_rate * (compression_time * 6))+2)
			{
				//计算优先级，即距离
				for(int j = 1;j < temp_buffer.size()-1;j++)
				{
					temp_buffer[j].priority = LineDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat);//点到线的距离
				}
				
				while(temp_buffer.size() == int(compression_rate * (compression_time * 6))+2)
				{
					//按速度筛选出要保留的点
/*					double start_speed = temp_buffer[0].speed;
					for(int j = 1;j < temp_buffer.size()-1;j++)
					{
						if(abs(temp_buffer[j].speed - start_speed) >= sped)
						{
							temp_buffer[j].order = 1;
							start_speed = temp_buffer[j].speed;
						}
					}
*/
/*					double start_speed = temp_buffer[0].speed;

					double middle_speed = 0;
					int middle_weizhi = 0;
					//search middle point
					for(int j = 0;j < temp_buffer.size();j++)
					{
						if(abs(temp_buffer[j].speed - start_speed) >= sped)
						{
							middle_weizhi = j;
							middle_speed = temp_buffer[j].speed;
							break;
						}
					}
					//renew
					if(middle_weizhi != 0)
					{
						int st = middle_weizhi+1;
						for(int j = st;j < temp_buffer.size();j++)
						{
							if(abs(temp_buffer[j].speed - middle_speed) >= sped)//get the maybe point
							{
								if((middle_speed - start_speed) * (temp_buffer[j].speed - middle_speed) < 0)
								{
									temp_buffer[middle_weizhi].order = 1;
								}
								start_speed = middle_speed;
								middle_speed = temp_buffer[j].speed;
								middle_weizhi = j;
							}
						}
					}
*/
					double start_speed = temp_buffer[0].speed;
					for(int j = 1;j < temp_buffer.size()-1;j++)
					{
						double a = (temp_buffer[j].speed - temp_buffer[j-1].speed) / (temp_buffer[j].obdTime - temp_buffer[j-1].obdTime);
						double b = (temp_buffer[j+1].speed - temp_buffer[j].speed) / (temp_buffer[j+1].obdTime - temp_buffer[j].obdTime);
						if(abs(temp_buffer[j].speed - temp_buffer[j-1].speed) >= sped || (abs(a/b) > k && b != 0) || (abs(b/a) > k && a != 0))
						{
							temp_buffer[j].order = 1;
						}
					}

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
					//删除最小优先级点(考虑到原来速度的限制，要判断一下order)
					if(temp_buffer[weizhi].order == 0)
					{
						const vector<sItem>::iterator it = temp_buffer.begin() + weizhi;
						temp_buffer.erase(it);
						//cout<<"erase"<<endl;
					}
					if(temp_buffer[weizhi].order == 1)
					{
						temp_buffer[weizhi].priority = 10001;//速度限制不可删除，就把他priority指标设为很大
					}

					//更新flag
					int pp = 0;
					for(int j = 1;j < temp_buffer.size()-1;j++)
					{
						if(temp_buffer[j].order == 0)
						{
							flag = true;
							break;
						}
						pp = j;
					}
					if(pp == temp_buffer.size()-2)
						flag = false;
					if(!flag)
						break;
					//cout<<flag<<endl;
				}
				//cout<<"njbvbhebhqdbhbqhbqhdbqhqbed"<<endl;
			}
		}
		//到达规定点数，把最后一个点留下，其他放入新轨迹
		if( size == compression_time * 6  || !flag)
		{
			//以上保证了压缩比，在这个基础上，还要把priority小于一定值的做一个删除
			vector <int> vec_weizhi;
			for(int j = 1;j < temp_buffer.size()-1;j++)
			{
				temp_buffer[j].priority = LineDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat);
				if(temp_buffer[j].priority <= priority_limit && temp_buffer[j].order != 1)
					vec_weizhi.push_back(j);//priority大于阈值的把它的位置记下
			}
			//按位置删除
			for(int j = 0;j < vec_weizhi.size();j++)
			{
				const vector<sItem>::iterator it = temp_buffer.begin() + vec_weizhi[j]-j;
				temp_buffer.erase(it);
			}
			//接下来做存储
			for(int j = 0;j < temp_buffer.size()-1;j++)
			{
				store.push_back(temp_buffer[j]);
			}
			sItem before_last_sitem = temp_buffer[temp_buffer.size()-2];
			sItem last_sitem = temp_buffer[temp_buffer.size()-1];
			temp_buffer.clear();
			temp_buffer.push_back(before_last_sitem);
			temp_buffer.push_back(last_sitem);

			//应对!flag情况
			if(!flag)
				flag = true;

			size = 2;
		}
		//到达末尾，buffer里面所有点放到新轨迹
		if( i == pS.store.size()-1)
		{
			//以上保证了压缩比，在这个基础上，还要把priority小于一定值的做一个删除
			vector <int> vec_weizhi;
			for(int j = 1;j < temp_buffer.size()-1;j++)
			{
				temp_buffer[j].priority = LineDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat);
				if(temp_buffer[j].priority <= priority_limit && temp_buffer[j].order != 1)
					vec_weizhi.push_back(j);//priority大于阈值的把它的位置记下
			}
			//按位置删除
			for(int j = 0;j < vec_weizhi.size();j++)
			{
				const vector<sItem>::iterator it = temp_buffer.begin() + vec_weizhi[j]-j;
				temp_buffer.erase(it);
			}
			//接下来做存储
			for(int j = 0;j < temp_buffer.size();j++)
			{
				store.push_back(temp_buffer[j]);
			}
			temp_buffer.clear();
			size = 0;
		}

		temp_buffer.push_back(pS.store[i]);
		size++;
	}

	//以上保证了压缩比，在这个基础上，还要把priority小于一定值的做一个删除
/*	vector <int> vec_weizhi;
	for(int i = 1;i < store.size()-1;i++)
	{
		store[i].priority = LineDistance(store[i-1].lng,store[i-1].lat,store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		if(store[i].priority <= priority_limit && store[i].order != 1)
			vec_weizhi.push_back(i);//priority大于阈值的把它的位置记下
	}
	//按位置删除
	for(int i = 0;i < vec_weizhi.size();i++)
	{
		const vector<sItem>::iterator it = store.begin() + vec_weizhi[i]-i;
		store.erase(it);
	}
*/
	//到这里就得到一条完整的新轨迹
}

speedCompression::~speedCompression(void)
{
}

pair<double,double> speedCompression::getLocationL(double l)
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

pair<double,double> speedCompression::getLocationT(double t)
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

void speedCompression::calc()
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

void speedCompression::calcArea()
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

void speedCompression::calcDistance()
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