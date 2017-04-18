#include "AdaptiveAccumulation.h"


AdaptiveAccumulation::AdaptiveAccumulation(pointSeq pS,double compression_rate,int compression_time,double priority_limit)
{
	
	ptSeq = pS;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	

	double rate = 10;

	//priority_limit = 5;//�������ѹ��ǰƽ��ֵ����û��Ҫ����Ԥ��ֵ
	double modify_priority_limit = 0;//������������priority_limit

	vector <sItem> semi_buffer;
	vector <sItem> temp_buffer;
	bool flag = false;

	int all_in_size = 0;//��¼ʵ�ʽ���ĵ�������Ϊ�����ٶ�Ϊ0�ĵ㲻�����
	int need_buffer_size = 2;//�ۻ�
	int semi_buffer_size = 2;
	for(int p = 0;p < pS.store.size();p++)
	{
		//ÿ5����ѡ����һ����������֤ѹ����
		//��store��ѡ300�����semi_buffer
		all_in_size++;
		//if(pS.store[p].speed > 0)//0.1
		{
			semi_buffer.push_back(pS.store[p]);
			semi_buffer_size++;
		}

		if(semi_buffer_size == int(compression_time * 60 / rate) + 2 || p == pS.store.size()-1)//+2����Ҫ����
		{
			flag = true;//��priority_limitɾ����Ҫ����һ��ִ��

			//�������ȼ���������
			for(int j = 1;j < semi_buffer.size()-1;j++)
			{
				//temp_buffer[j].priority = LineDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat);//�㵽�ߵľ���
				//vector <double> xyt = pS.calcProjection(semi_buffer[j-1],semi_buffer[j],semi_buffer[j+1]);
				//semi_buffer[j].priority = sqrt(pow(xyt[0]/scale_x,2) + pow(xyt[1]/scale_y,2) + pow(xyt[2]/scale_t,2));
				double t1 = semi_buffer[j].obdTime - semi_buffer[j-1].obdTime;
				double t2 = semi_buffer[j+1].obdTime - semi_buffer[j-1].obdTime;
				semi_buffer[j].priority = SEDistance(semi_buffer[j-1].lng,semi_buffer[j-1].lat,semi_buffer[j].lng,semi_buffer[j].lat,semi_buffer[j+1].lng,semi_buffer[j+1].lat,t1,t2) + semi_buffer[j].add_priority;//�㵽�ߵľ���//add-priorityû������Ϊ0������ܴ�
			}


			//����small�ۼ����
			double sum_priority = 0;
			for(int j = 1;j < semi_buffer.size()-1;j++)
			{
				sum_priority += semi_buffer[j].priority;
			}
			modify_priority_limit = sum_priority / (semi_buffer.size()-2);

			//if(priority_limit == 0)
			//	priority_limit = modify_priority_limit;
			//else
			//	priority_limit = (priority_limit + modify_priority_limit) / 2;

			//cout<<priority_limit<<endl;
			
			//Ѱ����С���ȼ�
			int weizhi = 1;
			double youxianji = semi_buffer[1].priority;
			for(int j = 1;j < semi_buffer.size()-1;j++)
			{
				if(semi_buffer[j].priority < youxianji)
				{
					youxianji = semi_buffer[j].priority;
					weizhi = j;
				}
			}
			//while(youxianji < priority_limit && semi_buffer.size() >= need_buffer_size)//priority���ۼƻ�ѵ�ɾ�⣬����������
			while(youxianji < priority_limit && semi_buffer.size() > 2)
			{
				//cout<<youxianji<<endl;
				//cout<<"in while "<<semi_buffer.size()<<endl;
				
				//ɾ����С���ȼ���
				const vector<sItem>::iterator it = semi_buffer.begin() + weizhi;
				semi_buffer.erase(it);

				//����������ǰ��
				if(weizhi >= 2 && semi_buffer.size() - weizhi >= 2)
				{
					double t1 = semi_buffer[weizhi-1].obdTime - semi_buffer[weizhi-2].obdTime;
					double t2 = semi_buffer[weizhi].obdTime - semi_buffer[weizhi-2].obdTime;
					if(youxianji >=semi_buffer[weizhi-1].add_priority)
					{
						semi_buffer[weizhi-1].priority = SEDistance(semi_buffer[weizhi-2].lng,semi_buffer[weizhi-2].lat,semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,t1,t2) + youxianji;//���ȼ����¸���
						semi_buffer[weizhi-1].add_priority = youxianji;
					}
					else
						semi_buffer[weizhi-1].priority = SEDistance(semi_buffer[weizhi-2].lng,semi_buffer[weizhi-2].lat,semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,t1,t2) + semi_buffer[weizhi-1].add_priority;//���ȼ����¸���


					double t3 = semi_buffer[weizhi].obdTime - semi_buffer[weizhi-1].obdTime;
					double t4 = semi_buffer[weizhi+1].obdTime - semi_buffer[weizhi-1].obdTime;
					if(youxianji >= semi_buffer[weizhi].add_priority)
					{
						semi_buffer[weizhi].priority = SEDistance(semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,semi_buffer[weizhi+1].lng,semi_buffer[weizhi+1].lat,t3,t4) + youxianji;//���ȼ����¸���
						semi_buffer[weizhi].add_priority = youxianji;
					}
					else
						semi_buffer[weizhi].priority = SEDistance(semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,semi_buffer[weizhi+1].lng,semi_buffer[weizhi+1].lat,t3,t4) + semi_buffer[weizhi].add_priority;//���ȼ����¸���
				}

				if(weizhi == 1 && semi_buffer.size() - weizhi >= 2)//���ŵ�һ���㣬��ǰ��ľͲ��ü�����
				{
					if(youxianji >= semi_buffer[weizhi-1].add_priority)
					{
						semi_buffer[weizhi-1].priority = semi_buffer[weizhi-1].priority + youxianji - semi_buffer[weizhi-1].add_priority;//���ȼ����¸���
						semi_buffer[weizhi-1].add_priority = youxianji;
					}

					double t3 = semi_buffer[weizhi].obdTime - semi_buffer[weizhi-1].obdTime;
					double t4 = semi_buffer[weizhi+1].obdTime - semi_buffer[weizhi-1].obdTime;
					if(youxianji >= semi_buffer[weizhi].add_priority)
					{
						semi_buffer[weizhi].priority = SEDistance(semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,semi_buffer[weizhi+1].lng,semi_buffer[weizhi+1].lat,t3,t4) + youxianji;//���ȼ����¸���
						semi_buffer[weizhi].add_priority = youxianji;
					}
					else
						semi_buffer[weizhi].priority = SEDistance(semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,semi_buffer[weizhi+1].lng,semi_buffer[weizhi+1].lat,t3,t4) + semi_buffer[weizhi].add_priority;//���ȼ����¸���
				}
					
				if(weizhi >= 2 && semi_buffer.size() - weizhi == 1)//�������һ���㣬�Ǻ���ľͲ��ü�����
				{
					double t1 = semi_buffer[weizhi-1].obdTime - semi_buffer[weizhi-2].obdTime;
					double t2 = semi_buffer[weizhi].obdTime - semi_buffer[weizhi-2].obdTime;
					if(youxianji >= semi_buffer[weizhi-1].add_priority)
					{
						semi_buffer[weizhi-1].priority = SEDistance(semi_buffer[weizhi-2].lng,semi_buffer[weizhi-2].lat,semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,t1,t2) + youxianji;//���ȼ����¸���
						semi_buffer[weizhi-1].add_priority = youxianji;
					}
					else
						semi_buffer[weizhi-1].priority = SEDistance(semi_buffer[weizhi-2].lng,semi_buffer[weizhi-2].lat,semi_buffer[weizhi-1].lng,semi_buffer[weizhi-1].lat,semi_buffer[weizhi].lng,semi_buffer[weizhi].lat,t1,t2) + semi_buffer[weizhi-1].add_priority;//���ȼ����¸���

					if(youxianji >= semi_buffer[weizhi].add_priority)
					{
						//semi_buffer[weizhi].priority = youxianji;//���ȼ����¸��¡��ϸ���˵Ӧ�ÿ���add_priority,���Ƕ�semi_buffer��˵��add-priorityֵΪ0���ɲ�����
						semi_buffer[weizhi].priority = semi_buffer[weizhi].priority + youxianji - semi_buffer[weizhi].add_priority;
						semi_buffer[weizhi].add_priority = youxianji;
					}
				}
				//����Ѱ����С���ȼ���
				weizhi = 1;
				youxianji = semi_buffer[1].priority;
				for(int j = 1;j < semi_buffer.size()-1;j++)
				{
					if(semi_buffer[j].priority < youxianji)
					{
						youxianji = semi_buffer[j].priority;
						weizhi = j;
					}
				}
			}
		}
		//cout<<semi_buffer.size()<<endl;
		if(flag)
		{
			//int const_buffer_size = int(compression_rate * (compression_time * 60 / rate))+2;
			int const_buffer_size = int(compression_rate * all_in_size);
			//cout<<const_buffer_size<<endl;
			need_buffer_size += const_buffer_size;
			//cout<<need_buffer_size<<endl;
			//�������ѹ����
			if(need_buffer_size <= 4)
			{
				store.push_back(semi_buffer[0]);
				store.push_back(semi_buffer[semi_buffer.size()-1]);
			}

			if(p == pS.store.size()-1)//��Ҫ
				need_buffer_size -= 2;

			if(semi_buffer.size() <= need_buffer_size)
			{
				//cout<<"zhendexiaoyula"<<endl;
				if(p != pS.store.size()-1)
				{
					//�ٴ洢
					for(int j = 0;j < semi_buffer.size()-2;j++)
					{
						store.push_back(semi_buffer[j]);
					}
					sItem before_last_sitem = semi_buffer[semi_buffer.size()-2];
					sItem last_sitem = semi_buffer[semi_buffer.size()-1];
					temp_buffer.clear();
					semi_buffer.clear();
					//temp_buffer.push_back(before_last_sitem);
					//temp_buffer.push_back(last_sitem);
					semi_buffer.push_back(before_last_sitem);
					semi_buffer.push_back(last_sitem);
					semi_buffer_size = 2;
					all_in_size = 0;
					need_buffer_size -= semi_buffer.size();
				}

				if(p == pS.store.size()-1)
				{
					//�ٴ洢
					for(int j = 0;j < semi_buffer.size();j++)
					{
						store.push_back(semi_buffer[j]);
					}
					temp_buffer.clear();
					semi_buffer.clear();
					//all_in_size = 0;
					//need_buffer_size -= semi_buffer.size() + 2;
				}
			}

			if(semi_buffer.size() > need_buffer_size  && need_buffer_size > 2)//Ϊ�������޸�
			{
				int start_q_size = 0;
				for(int i = 0;i < semi_buffer.size();i++)
				{
					//bufferѹ���µĵ�
					temp_buffer.push_back(semi_buffer[i]);
					start_q_size++;

					int buffer_size = temp_buffer.size();
					if(buffer_size == need_buffer_size && start_q_size != need_buffer_size)//������ѹ���ǰһ���priority
					{
						//temp_buffer[buffer_size-2].priority = LineDistance(temp_buffer[buffer_size-3].lng,temp_buffer[buffer_size-3].lat,temp_buffer[buffer_size-2].lng,temp_buffer[buffer_size-2].lat,temp_buffer[buffer_size-1].lng,temp_buffer[buffer_size-1].lat);//�㵽�ߵľ���
						//vector <double> xyt = pS.calcProjection(temp_buffer[buffer_size-3],temp_buffer[buffer_size-2],temp_buffer[buffer_size-1]);
						//temp_buffer[buffer_size-2].priority = sqrt(pow(xyt[0]/scale_x,2) + pow(xyt[1]/scale_y,2) + pow(xyt[2]/scale_t,2)) + temp_buffer[buffer_size-2].add_priority;
						double t1 = temp_buffer[buffer_size-2].obdTime - temp_buffer[buffer_size-3].obdTime;
						double t2 = temp_buffer[buffer_size-1].obdTime - temp_buffer[buffer_size-3].obdTime;
						temp_buffer[buffer_size-2].priority = SEDistance(temp_buffer[buffer_size-3].lng,temp_buffer[buffer_size-3].lat,temp_buffer[buffer_size-2].lng,temp_buffer[buffer_size-2].lat,temp_buffer[buffer_size-1].lng,temp_buffer[buffer_size-1].lat,t1,t2) + temp_buffer[buffer_size-2].add_priority;//�㵽�ߵľ���
					}

					//����ĩβ��buffer�������е�ŵ��¹켣
					if( i == semi_buffer.size()-1 && p == pS.store.size()-1)
					{
						//���������洢
						for(int j = 0;j < temp_buffer.size();j++)
						{
							store.push_back(temp_buffer[j]);
						}
						temp_buffer.clear();
						//need_buffer_size = 2;
						break;
					}

					if( i == semi_buffer.size()-1 && p != pS.store.size()-1)
					{
						//���������洢
						for(int j = 0;j < temp_buffer.size()-2;j++)
						{
							store.push_back(temp_buffer[j]);
						}
						sItem before_last_sitem = temp_buffer[temp_buffer.size()-2];
						sItem last_sitem = temp_buffer[temp_buffer.size()-1];
						temp_buffer.clear();
						semi_buffer.clear();
						//temp_buffer.push_back(before_last_sitem);
						//temp_buffer.push_back(last_sitem);
						semi_buffer.push_back(before_last_sitem);
						semi_buffer.push_back(last_sitem);
						all_in_size = 0;
						need_buffer_size = 2;
						semi_buffer_size = 2;
						start_q_size = 0;
						break;
					}

					//ÿ��buffer��һ�ε���涨��Ŀʱ��priority��������õ�
					if(start_q_size == need_buffer_size)//����л�����һ��ģʽ���ͻ��漰��priorityû�м�������⣬��Ϊѹ����ȡ1
					{
						//�������ȼ���������
						for(int j = 1;j < temp_buffer.size()-1;j++)
						{
							//temp_buffer[j].priority = LineDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat);//�㵽�ߵľ���
							//vector <double> xyt = pS.calcProjection(temp_buffer[j-1],temp_buffer[j],temp_buffer[j+1]);
							//temp_buffer[j].priority = sqrt(pow(xyt[0]/scale_x,2) + pow(xyt[1]/scale_y,2) + pow(xyt[2]/scale_t,2)) + temp_buffer[j].add_priority;
							double t1 = temp_buffer[j].obdTime - temp_buffer[j-1].obdTime;
							double t2 = temp_buffer[j+1].obdTime - temp_buffer[j-1].obdTime;
							temp_buffer[j].priority = SEDistance(temp_buffer[j-1].lng,temp_buffer[j-1].lat,temp_buffer[j].lng,temp_buffer[j].lat,temp_buffer[j+1].lng,temp_buffer[j+1].lat,t1,t2) + temp_buffer[j].add_priority;//�㵽�ߵľ���
						}
						//�ڶ������Ǵ���һ��ʣ�����ģ���Ҫ�����Լ���add_priority
						//temp_buffer[1].priority += temp_buffer[1].add_priority;
					}

					while(temp_buffer.size() == need_buffer_size)
					{
						//cout<<"in while "<<temp_buffer.size()<<endl;
						//����Ѱ����С���ȼ���
						int weizhi = 1;
						double youxianji = temp_buffer[1].priority;
						for(int j = 1;j < temp_buffer.size()-1;j++)
						{
							if(temp_buffer[j].priority < youxianji)
							{
								youxianji = temp_buffer[j].priority;
								weizhi = j;
							}
						}

						//ɾ����С���ȼ���
						const vector<sItem>::iterator it = temp_buffer.begin() + weizhi;
						temp_buffer.erase(it);

						//����������ǰ��
						if(weizhi >= 2 && temp_buffer.size() - weizhi >= 2)
						{
							double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
							double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
							if(youxianji >= temp_buffer[weizhi-1].add_priority)
							{
								temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//���ȼ����¸���
								temp_buffer[weizhi-1].add_priority = youxianji;
							}
							else
								temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//���ȼ����¸���


							double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
							double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
							if(youxianji >= temp_buffer[weizhi].add_priority)
							{
								temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//���ȼ����¸���
								temp_buffer[weizhi].add_priority = youxianji;
							}
							else
								temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//���ȼ����¸���
						}

						if(weizhi == 1 && temp_buffer.size() - weizhi >= 2)//���ŵ�һ���㣬��ǰ��ľͲ��ü�����
						{
							if(youxianji >= temp_buffer[weizhi-1].add_priority)
							{
								//temp_buffer[weizhi-1].priority = youxianji;//���ȼ����¸���//����ط��д����Ͻ���д����
								temp_buffer[weizhi-1].priority = temp_buffer[weizhi-1].priority + youxianji - temp_buffer[weizhi-1].add_priority;
								temp_buffer[weizhi-1].add_priority = youxianji;
							}

							double t3 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-1].obdTime;
							double t4 = temp_buffer[weizhi+1].obdTime - temp_buffer[weizhi-1].obdTime;
							if(youxianji >= temp_buffer[weizhi].add_priority)
							{
								temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + youxianji;//���ȼ����¸���
								temp_buffer[weizhi].add_priority = youxianji;
							}
							else
								temp_buffer[weizhi].priority = SEDistance(temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,temp_buffer[weizhi+1].lng,temp_buffer[weizhi+1].lat,t3,t4) + temp_buffer[weizhi].add_priority;//���ȼ����¸���
						}
					
						if(weizhi >= 2 && temp_buffer.size() - weizhi == 1)//�������һ���㣬�Ǻ���ľͲ��ü�����
						{
							double t1 = temp_buffer[weizhi-1].obdTime - temp_buffer[weizhi-2].obdTime;
							double t2 = temp_buffer[weizhi].obdTime - temp_buffer[weizhi-2].obdTime;
							if(youxianji >= temp_buffer[weizhi-1].add_priority)
							{
								temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + youxianji;//���ȼ����¸���
								temp_buffer[weizhi-1].add_priority = youxianji;
							}
							else
								temp_buffer[weizhi-1].priority = SEDistance(temp_buffer[weizhi-2].lng,temp_buffer[weizhi-2].lat,temp_buffer[weizhi-1].lng,temp_buffer[weizhi-1].lat,temp_buffer[weizhi].lng,temp_buffer[weizhi].lat,t1,t2) + temp_buffer[weizhi-1].add_priority;//���ȼ����¸���


							if(youxianji >= temp_buffer[weizhi].add_priority)
							{
								//temp_buffer[weizhi].priority = youxianji;//���ȼ����¸���
								temp_buffer[weizhi].priority = temp_buffer[weizhi].priority + youxianji - temp_buffer[weizhi].add_priority;
								temp_buffer[weizhi].add_priority = youxianji;
							}
						}
					}
					//yuanben
				}
				//if(semi_buffer.size() > need_buffer_size && need_buffer_size == 2)
				{
				//	store.push_back(temp_buffer[0]);
				//	store.push_back(semi_buffer[semi_buffer.size()-1]);
				}
			}
			flag = false;
		}
	}
	//������͵õ�һ���������¹켣
}

AdaptiveAccumulation::~AdaptiveAccumulation(void)
{
}

pair<double,double> AdaptiveAccumulation::getLocationL(double l)
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

pair<double,double> AdaptiveAccumulation::getLocationT(double t)
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

void AdaptiveAccumulation::calc()
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

void AdaptiveAccumulation::calcArea()
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

void AdaptiveAccumulation::calcDistance()
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