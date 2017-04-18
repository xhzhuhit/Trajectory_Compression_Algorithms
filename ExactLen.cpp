#include "ExactLen.h"


ExactLen::ExactLen(pointSeq pS,double r,double l,double al,int isAdap)
{
	//initial
	setTimeUnit(r);
	setL(l);
	setAlpha(al);
	isAdaptive = isAdap;
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
	AL = 0;
	SD = 0;
	SDL = 0;

	//ofstream everystate;
	//everystate.open("everystate.txt",fstream::out);

	//int isAdaptive = 1;
	double adaptivePram = 1;

	//generate the Markov sequence

	double t = 0; 
	//first point at time 0
	sItem tmp;
	tmp.order = ptSeq.store[0].order;
	tmp.obdTime = ptSeq.store[0].obdTime + t;
	pair<double,double> temp = ptSeq.getLocationT(t);
	tmp.lng = temp.first;
	tmp.lat = temp.second;
	tmp.acceleration = ptSeq.getAcc(t);
	tmp.speed = ptSeq.getSpeed(t);
	store.push_back(tmp);

	//decide adaptive parameter
	if(isAdaptive)
	{
		double presentV = pS.getSpeed(t);
		double presentA = pS.getAcc(t);
		int presentLevel = getLevel(presentV,presentA );
		//adaptivePram = (presentLevel/10 + 3)/3;
		adaptivePram = (presentLevel/12 > 6)?1.2:1;
	}

	double needLength = adaptivePram * L;
	for(int i = 0; i < pS.number - 1; i ++ )
	{
		double segLength = pS.length[i+1] - pS.length[i];
		int sampleNumber = floor((segLength - needLength)/(adaptivePram * L)) + 1;
		if(sampleNumber == 0)
		{
			needLength =needLength - (pS.length[i+1] - pS.length[i]);
			continue;
		}

		int alreadySampleNumber = 0;
		for(t = pS.time[i]; t <= pS.time[i+1]; t += 0.01 )
		{
			double le = 1/3.0*pS.a[i]*pow(t - pS.time[i],3) + 0.5*pS.b[i]*pow(t - pS.time[i],2) + pS.c[i] * (t - pS.time[i]);
			if(le >= needLength)
			{
				//generate a sample point
				sItem tmp;
				tmp.order = ptSeq.store[0].order;
				tmp.obdTime = ptSeq.store[0].obdTime + t;
				pair<double,double> temp = ptSeq.getLocationT(t);
				tmp.lng = temp.first;
				tmp.lat = temp.second;
				tmp.acceleration = ptSeq.getAcc(t);
				tmp.speed = ptSeq.getSpeed(t);
				store.push_back(tmp);

				alreadySampleNumber ++;
				needLength += adaptivePram * L;
			}
			if(alreadySampleNumber == sampleNumber || needLength > adaptivePram * L)
			{
				
				//decide adaptive parameter
				if(isAdaptive)
				{
					double presentV = pS.getSpeed(t);
					double presentA = pS.getAcc(t);
					int presentLevel = getLevel(presentV,presentA );
					//adaptivePram = (presentLevel/10 + 3)/3;
					adaptivePram = (presentLevel/12 > 6)?1.2:1;
				}
				double leength =  pS.length[i+1] - pS.length[i] - le;//guanjian
				needLength =adaptivePram * L - (leength>0?leength:0);
				break;
			}
		}
	}

	
}

void ExactLen::setTimeUnit(double r)
{
	timeUnit = r;
}
void ExactLen::setL(double l)
{
	L = l;
}
void ExactLen::setAlpha(double al)
{
	alpha = al;
}

ExactLen::~ExactLen(void)
{
}

pair<double,double> ExactLen::getLocationL(double l)
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

pair<double,double> ExactLen::getLocationT(double t)
{
	int i;
	for(i = 0; i < number - 1; i ++)
	{
		if(time[i] <= t && time[i+1] >= t)
			break;
	}
	double t0 = t - time[i];
	double lng,lat;
	lng = store[i].lng+t0/(time[i+1] - time[i])*(store[i+1].lng - store[i].lng);
	lat = store[i].lat+t0/(time[i+1] - time[i])*(store[i+1].lat - store[i].lat);
	return make_pair(lng,lat);
}


void ExactLen::calc()
{
	number = store.size();
	time.push_back(0);
	length.push_back(0);

	double presentV = ptSeq.getSpeed(0);
	double presentA = ptSeq.getAcc(0);
	int presentLevel = getLevel(presentV,presentA );
	out_level.push_back(presentLevel);

	for(int i = 0;i < number - 1; i ++ )
	{
		double ti = store[i+1].obdTime - store[i].obdTime;
		totalTime += ti;
		time.push_back(totalTime);

		presentV = ptSeq.getSpeed(totalTime);
		presentA = ptSeq.getAcc(totalTime);
		presentLevel = getLevel(presentV,presentA);
		out_level.push_back(presentLevel);

		out_increase_time.push_back(ti);//存储采样点增加时间
		out_speed.push_back(store[i].speed);//存储采样点速度,速度为什么会出现负值？
		out_acceleration.push_back(store[i].acceleration);

		double s = Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		totalLength += s;
		length.push_back(totalLength);
	}
	out_speed.push_back(store[number-1].speed);//终点速度
	out_acceleration.push_back(store[number-1].acceleration);
}

void ExactLen::calcRR()
{
	int count = 0;
	for(int i = 0;i < store.size()-1; i++)
	{
		count += Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat)< alpha*L;
	}
	nRR = count;
	RR = (double)count/store.size();
}

void ExactLen::calcAD()
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

void ExactLen::calcArea()
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
			out_aera.push_back(0);//面积存储

		if(middlePoint.size() != 0 )
		{
			c++;
			if(c == 77)
				cout<<"";
			double temp = countArea_new(make_pair(store[i].lng,store[i].lat),make_pair(store[i+1].lng,store[i+1].lat),middlePoint);
			
				//cout<< temp <<"("<<c<<")  ";
			//if(store[i].obdTime - store[0].obdTime > 50 && store[store.size()-1].obdTime - store[i].obdTime > 50)
			a +=temp;

			out_aera.push_back(temp);
		}
	}
	area = a;
}

void ExactLen::calcAL()
{
	AL = length[number - 1]/(number - 1);
}

void ExactLen::calcSD()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SD += (length[i] - length[i-1] - AL)*(length[i] - length[i-1] - AL);
	}
	SD /= number - 1;
}
void ExactLen::calcSDL()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SDL += (length[i] - length[i-1] - L)*(length[i] - length[i-1] - L);
	}
	SDL /= number - 1;
}

void ExactLen::calcDistance()
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
				dist += Distance(middlePoint[size-1].first,middlePoint[size-1].second,store[i+1].lng,store[i+1].lat);
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