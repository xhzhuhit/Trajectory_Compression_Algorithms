#include "arcRecovery.h"


arcRecovery::arcRecovery(pointSeq fixedRateSeq,pointSeq compressionSeq,double rate,double sed_limit,double set_angle_recovery,ofstream &debuout)
{
	
	ptSeq = fixedRateSeq;
	time.clear();
	length.clear();
	totalLength = 0;
	totalTime = 0;
	
	area = 0;
	
	//ͳ�ƴ���sed_limit�Ĳ��ֵĻ���
	/*double angle_sum = 0;
	int count_angle_sum = 0;
	for(int i = 1;i < compressionSeq.store.size()-1;i++)
	{
		double t1 = compressionSeq.store[i].obdTime - compressionSeq.store[i-1].obdTime;
		double t2 = compressionSeq.store[i+1].obdTime - compressionSeq.store[i-1].obdTime;
		//double check_sed = SEDistance(compressionSeq.store[i-1].lng,compressionSeq.store[i-1].lat,compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,t1,t2);
		//���ǿ�ѹ����֮���ƫ���������ֱ������ܻ��õ�
		double check_sed = LineDistance(compressionSeq.store[i-1].lng,compressionSeq.store[i-1].lat,compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat);
		if(check_sed >= sed_limit)
		{
			double angle = calcAngle(compressionSeq.store[i-1].lng,compressionSeq.store[i-1].lat,compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat);
			angle_sum += angle;
			count_angle_sum++;
		}
	}
	angle_recovery = angle_sum / count_angle_sum;//������Ƕ�����ԭ���
	

	int biaoji = 0;//������¼���һ�θ�ԭ��ֱ�������ԭ�����һ�㣬����ֱ�߸�ԭ�������ڶ���

	int size = compressionSeq.store.size();
	for(int i = 0;i < compressionSeq.store.size()-1;i++)
	{
		//store.push_back(compressionSeq.store[i]);
		//����һ�µڶ��㵽һ�������sed
		double t1 = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
		double t2 = compressionSeq.store[i+2].obdTime - compressionSeq.store[i].obdTime;
		//double check_sed = SEDistance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat,t1,t2);
		double check_sed = LineDistance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat);
		if(check_sed >= sed_limit)//�����޶�sed��һ���Իָ����Σ�i+1,���λָ�
		{
			store.push_back(compressionSeq.store[i]);
			double deta_time = compressionSeq.store[i+2].obdTime - compressionSeq.store[i].obdTime;
			int number = int(deta_time / rate);
			
			//�ؼ����ڼ���Բ�����꣬���Ҹ�ԭ�ɾ�γ��
			for(int j = 0;j < number-1;j++)
			{
				sItem tmp;
				tmp.order = 1;
				tmp.priority = 1;
				tmp.acceleration = 0;//ǰ�����޹ؽ�Ҫ
				//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
				tmp.obdTime = compressionSeq.store[i].obdTime + (j+1) * rate;

				//��һ���û���
				simple_point pint = compressionSeq.calcArcPoint(compressionSeq.store[i],compressionSeq.store[i+2],j+1,angle_recovery,number);
				tmp.lng = pint.lng;
				tmp.lat = pint.lat;
				tmp.speed = pint.speed;
				//��һ��ֱ����ֱ��
				//tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat) / (compressionSeq.store[i+2].obdTime - compressionSeq.store[i].obdTime);
				//tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+2].lng - compressionSeq.store[i].lng) / number * (j + 1);
				//tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+2].lat - compressionSeq.store[i].lat) / number * (j + 1);

				store.push_back(tmp);
			}
			
			i++;//������ǰ��һ����Ϊһ�λ��൱�ڴ���������ֱ��
		}
		else
		{
			store.push_back(compressionSeq.store[i]);

			double deta_time = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
			int number = int(deta_time / rate);
			if(number > 1)//����1˵���м�Ӧ�ò���㣬��˳�������������Ҵ浽�¹켣����
			{
				for(int j = 0;j < number-1;j++)
				{
					sItem tmp;
					tmp.order = 1;
					tmp.priority = 1;
					tmp.acceleration = 0;//ǰ�����޹ؽ�Ҫ
					//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
					tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat) / (compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime) ;
					tmp.obdTime = compressionSeq.store[i].obdTime + (j+1) * rate;
					//SED��ʽ
					tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+1].lng - compressionSeq.store[i].lng) / number * (j + 1);
					tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+1].lat - compressionSeq.store[i].lat) / number * (j + 1);

					store.push_back(tmp);
				}
			}
		}
		biaoji = i;
	}

	if(biaoji == compressionSeq.store.size()-3)//˵�����һ��û�л��ȸ�ԭ
	{
		store.push_back(compressionSeq.store[size-2]);//ѹ�뵹���ڶ���

		double deta_time = compressionSeq.store[size-1].obdTime - compressionSeq.store[size-2].obdTime;
		int number = int(deta_time / rate);
		if(number > 1)//����1˵���м�Ӧ�ò���㣬��˳�������������Ҵ浽�¹켣����
		{
			for(int j = 0;j < number-1;j++)
			{
				sItem tmp;
				tmp.order = 1;
				tmp.priority = 1;
				tmp.acceleration = 0;//ǰ�����޹ؽ�Ҫ
				//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
				tmp.speed = Distance(compressionSeq.store[size-2].lng,compressionSeq.store[size-2].lat,compressionSeq.store[size-1].lng,compressionSeq.store[size-1].lat) / (compressionSeq.store[size-1].obdTime - compressionSeq.store[size-2].obdTime) ;
				tmp.obdTime = compressionSeq.store[size-2].obdTime + (j+1) * rate;
				//SED��ʽ
				tmp.lng = compressionSeq.store[size-2].lng + (compressionSeq.store[size-1].lng - compressionSeq.store[size-2].lng) / number * (j + 1);
				tmp.lat = compressionSeq.store[size-2].lat + (compressionSeq.store[size-1].lat - compressionSeq.store[size-2].lat) / number * (j + 1);

				store.push_back(tmp);
			}
		}
		store.push_back(compressionSeq.store[size-1]);//ѹ�뵹����1��
	}

	if(biaoji == compressionSeq.store.size()-2)//���һ�λ��ȸ�ԭ
		store.push_back(compressionSeq.store[size-1]);//ѹ�뵹����1��
*/

	int size = compressionSeq.store.size();
	double max_sed = 0;
	int start = 0;
	for(int i = 0;i < compressionSeq.store.size()-1;i++)
	{
		//store.push_back(compressionSeq.store[i]);
		//����һ�µڶ��㵽һ�������sed
		//double t1 = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
		//double t2 = compressionSeq.store[i+2].obdTime - compressionSeq.store[i].obdTime;
		//double check_sed = SEDistance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat,t1,t2);
		//double check_sed = LineDistance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat);
		double t1 = compressionSeq.store[i].obdTime - compressionSeq.store[0].obdTime;
		double t2 = compressionSeq.store[i+1].obdTime - compressionSeq.store[0].obdTime;
		for(int j = start; j < ptSeq.number; j++)
		{
			if(ptSeq.time[j] > t1 + 0.0001 && ptSeq.time[j] < t2 - 0.0001)
			{
				//middlePoint.push_back(make_pair(ptSeq.store[j].lng,ptSeq.store[j].lat));
				//�����ӳ���
				//vector <double> tmmp = ptSeq.calcProjection(compressionSeq.store[i],ptSeq.store[j],compressionSeq.store[i+1]);
				double t3 = ptSeq.store[j].obdTime -compressionSeq.store[i].obdTime;
				double t4 = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
				double temp_sed = SEDistance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,ptSeq.store[j].lng,ptSeq.store[j].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat,t3,t4);
				if(temp_sed > max_sed)
					max_sed = temp_sed;

				continue;
			}
			if(ptSeq.time[j] >= t2 - 0.0001)
			{
				start = j;
				break;
			}
		}
	
		double check_sed = max_sed;
		//check_sedҪ������һ�ε�
		if(check_sed >= sed_limit)//�����޶�sed��һ���Իָ����Σ�i+1,���λָ�
		{
			store.push_back(compressionSeq.store[i]);
			double deta_time = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
			int number = int(deta_time / rate);
			
			//�ؼ����ڼ���Բ�����꣬���Ҹ�ԭ�ɾ�γ��
			for(int j = 0;j < number-1;j++)
			{
				sItem tmp;
				tmp.order = 1;
				tmp.priority = 1;
				tmp.acceleration = 0;//ǰ�����޹ؽ�Ҫ
				//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
				tmp.obdTime = compressionSeq.store[i].obdTime + (j+1) * rate;

				//��һ���û���
				simple_point pint = compressionSeq.calcArcPoint(compressionSeq.store[i],compressionSeq.store[i+1],j+1,set_angle_recovery,number);
				tmp.lng = pint.lng;
				tmp.lat = pint.lat;
				tmp.speed = pint.speed;
				//��һ��ֱ����ֱ��
				//tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+2].lng,compressionSeq.store[i+2].lat) / (compressionSeq.store[i+2].obdTime - compressionSeq.store[i].obdTime);
				//tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+2].lng - compressionSeq.store[i].lng) / number * (j + 1);
				//tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+2].lat - compressionSeq.store[i].lat) / number * (j + 1);

				store.push_back(tmp);
			}
		}
		else
		{
			store.push_back(compressionSeq.store[i]);

			double deta_time = compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime;
			int number = int(deta_time / rate);
			if(number > 1)//����1˵���м�Ӧ�ò���㣬��˳�������������Ҵ浽�¹켣����
			{
				for(int j = 0;j < number-1;j++)
				{
					sItem tmp;
					tmp.order = 1;
					tmp.priority = 1;
					tmp.acceleration = 0;//ǰ�����޹ؽ�Ҫ
					//tmp.speed = compressionSeq.store[i].speed + (compressionSeq.store[i+1].speed - compressionSeq.store[i].speed) / number * (j + 1);
					tmp.speed = Distance(compressionSeq.store[i].lng,compressionSeq.store[i].lat,compressionSeq.store[i+1].lng,compressionSeq.store[i+1].lat) / (compressionSeq.store[i+1].obdTime - compressionSeq.store[i].obdTime) ;
					tmp.obdTime = compressionSeq.store[i].obdTime + (j+1) * rate;
					//SED��ʽ
					tmp.lng = compressionSeq.store[i].lng + (compressionSeq.store[i+1].lng - compressionSeq.store[i].lng) / number * (j + 1);
					tmp.lat = compressionSeq.store[i].lat + (compressionSeq.store[i+1].lat - compressionSeq.store[i].lat) / number * (j + 1);

					store.push_back(tmp);
				}
			}
		}
	}
	store.push_back(compressionSeq.store[size-1]);//ѹ�뵹����1��
}

arcRecovery::~arcRecovery(void)
{
}

pair<double,double> arcRecovery::getLocationL(double l)
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

pair<double,double> arcRecovery::getLocationT(double t)
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

void arcRecovery::calc()
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

void arcRecovery::calcArea()
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

void arcRecovery::calcDistance()
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

void arcRecovery::calcSpeedError()
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

void arcRecovery::calcLengthError()
{
	double error = 0;
	for(int i = 0;i < store.size() && i < ptSeq.store.size();i++)
	{
		double err = Distance(store[i].lng,store[i].lat,ptSeq.store[i].lng,ptSeq.store[i].lat);
		error += err;
	}
	length_error = error;
}

void arcRecovery::calcError(pointSeq fixRateSeq,pointSeq compressionSeq)
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
				//�����ӳ���
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