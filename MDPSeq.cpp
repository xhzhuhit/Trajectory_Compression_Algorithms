#include "MDPSeq.h"


MDPSeq::MDPSeq(pointSeq pS,double r,double l,double al,int matrix[][144],int D[],map<int,int> stayTime[][144],int totalHappen[][144],int isAdap,map<int,int> singleStayTime[144])
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

	for(int i = 0; i < 20;i++)
	{
		aera_segment[i] = 0;
		count_segment[i] = 0;
	}
	//ofstream everystate;
	//everystate.open("everystate.txt",fstream::out);



	//int isAdaptive = 1;
	double adaptivePram = 1;
	//generate the Markov sequence

	double t1 = 0, t2 = 0;
	bool isLastOne = false;

	sItem tmp;
	tmp.order = ptSeq.store[0].order;
	tmp.obdTime = ptSeq.store[0].obdTime + t2;
	pair<double,double> temp = ptSeq.getLocationT(t2);
	tmp.lng = temp.first;
	tmp.lat = temp.second;
	tmp.acceleration = ptSeq.getAcc(t2);
	tmp.speed = ptSeq.getSpeed(t2);
	store.push_back(tmp);
	//double nextLevel = 0;//因为staytime预测要在presentlevel之后

	while(!isLastOne)
	{//stayTime 应该是当前的等待时间
		double presentV = ptSeq.getSpeed(t1);
		double presentA = ptSeq.getAcc(t1);
		int presentLevel = getLevel(presentV,presentA );
		presentLevel = checkIfExist(presentLevel,D);
		//以下第一个点的staytime获取有点不一样
		double theStayTime = getSingleStayTime(presentLevel,singleStayTime,totalHappen,timeUnit);
		//cout<<"single : "<<theStayTime<<endl;

		//这时可以预测下一点速度
		double nextLevel = getNextLevel(presentLevel,matrix,D);
		double nextV = getVA(nextLevel).first;
		double nextA = getVA(nextLevel).second;

		if(isAdaptive)
		{
			//adaptivePram = (presentLevel/10 + 5)/5;
			//adaptivePram = (presentLevel/10 + 5)/5;
			adaptivePram = (presentLevel/12 > 6)?1.2:1;
		}
		//double adaptivePram = 1;
		//if(nextV > 10)
			//adaptivePram = nextV/10;

		double len = 0;
		t2 += theStayTime;
		while((len += presentV * theStayTime) < adaptivePram * L && (t2 < ptSeq.totalTime))
		{
			//counter ++;
			//renew matrix
			/*double actualV = (pS.getSpeed(t1 + (counter - 1) * timeUnit) + pS.getSpeed(t1 + (counter) * timeUnit) )/2;
			double actualA = (pS.getAcc(t1 + (counter - 1) * timeUnit) + pS.getAcc(t1 + (counter) * timeUnit) )/2;
			int actualNextLevel = getLevel(actualV,actualA);
			matrix[presentLevel][actualNextLevel] ++;
			D[presentLevel] ++;*/

			//presentV = pS.getSpeed(t2);
			//presentA = pS.getAcc(t2);
			//presentLevel = getLevel(presentV,presentA);
			theStayTime = getStayTime(presentLevel,nextLevel,stayTime,totalHappen,timeUnit);//首先确定nextLevel的持续时间
			//cout<<"stay : "<<theStayTime<<endl;

			presentLevel = nextLevel;
			presentV = nextV;//代换保存

			nextLevel = getNextLevel(presentLevel,matrix,D);//预测下一个
			nextV = getVA(nextLevel).first;
			nextA = getVA(nextLevel).second;

			t2 += theStayTime;
		}
		//cout<<"jiesu"<<endl;
		len -= presentV * theStayTime;
		//if(t2>903)
		//	cout<<endl;
		if(t2 >= ptSeq.totalTime && t2 - t1 <= L/8)
		{
			t2 = ptSeq.totalTime;
			isLastOne = true;
			out_increase_time.push_back(t2-t1);//最后一段时间
		}
		else
		{
			t2 = t2 - theStayTime + (adaptivePram * L - len)/(presentV > 0?presentV:1);
			//if timeUnit = 0.1,keep one decimal place
			//if timeUnit = 1,keep integer
			t2 = floor(t2*(1/timeUnit)+0.5)/(1/timeUnit);

			double increase = t2 - t1;
			//int low = L/10 - 5;
			//int high = L/10 + 5;
			int low = L/20;//¸Ä³É15£¬Ð§¹ûºÜ²î
			int high = L/8;//´Ó5¸Ä³É10£¬ÕâÀïÒª¶à´Î²âÊÔ
			/*
			if(L > 0 && L <= 400)
			{
				low = L/20;
				high = L/8;
			}
			else if(L > 400 && L <= 800)
			{
				low = L/25;
				high = L/14;
			}
			else
			{
				low = L/30;
				high = L/18;
			}
			*/
			if(increase > high)
				increase = high;
			if(increase < low)
				increase = low;
			t2 = t1 + increase;
			out_increase_time.push_back(increase);//存储增加时间
			if(t2 >= ptSeq.totalTime)
			{
				t2 = ptSeq.totalTime;
			}
		}

		//renew matrix
		/*double actualV = (pS.getSpeed(t1 + (counter - 1) * timeUnit) + pS.getSpeed(t2))/2;
		double actualA = (pS.getAcc(t1 + (counter - 1) * timeUnit) + pS.getAcc(t2) )/2;
		int actualNextLevel = getLevel(actualV,actualA);
		matrix[presentLevel][actualNextLevel] ++;
		D[presentLevel] ++;*/

		sItem tmp;
		tmp.order = ptSeq.store[0].order;
		tmp.obdTime = ptSeq.store[0].obdTime + t2;
		pair<double,double> temp = ptSeq.getLocationT(t2);
		tmp.lng = temp.first;
		tmp.lat = temp.second;
		tmp.acceleration = ptSeq.getAcc(t2);
		tmp.speed = ptSeq.getSpeed(t2);
		store.push_back(tmp);

		t1 = t2;
	}



}

void MDPSeq::setTimeUnit(double r)
{
	timeUnit = r;
}
void MDPSeq::setL(double l)
{
	L = l;
}
void MDPSeq::setAlpha(double al)
{
	alpha = al;
}

MDPSeq::~MDPSeq(void)
{
}

pair<double,double> MDPSeq::getLocationL(double l)
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

pair<double,double> MDPSeq::getLocationT(double t)
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


void MDPSeq::calc()
{
	number = store.size();
	time.push_back(0);
	length.push_back(0);
	for(int i = 0;i < number - 1; i ++ )
	{
		double ti = store[i+1].obdTime - store[i].obdTime;
		totalTime += ti;
		time.push_back(totalTime);

		//out_increase_time.push_back(ti);//存储采样点增加时间
		out_speed.push_back(store[i].speed);//存储采样点速度,速度为什么会出现负值？

		double s = Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		totalLength += s;
		length.push_back(totalLength);
	}
	out_speed.push_back(store[number-1].speed);//终点速度
}

void MDPSeq::calcRR()
{
	int count = 0;
	for(int i = 0;i < store.size()-1; i++)
	{
		count += Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat)< alpha*L;
	}
	nRR = count;
	RR = (double)count/store.size();
}

void MDPSeq::calcAD()
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

void MDPSeq::calcArea()
{
	double a = 0;
	int start = 0;
	int c =0;
	for(int i = 0; i < store.size() - 1; i ++)
	{
		double t1 = store[i].obdTime - ptSeq.store[0].obdTime;
		double t2 = store[i + 1].obdTime - ptSeq.store[0].obdTime;
		vector <pair<double,double> > middlePoint;
		for(int j = start; j < ptSeq.number; j++ )
		{
			double time_raw = ptSeq.store[j].obdTime - ptSeq.store[0].obdTime;
			if(time_raw > t1 + 0.0001 && time_raw < t2 - 0.0001)
			{
				middlePoint.push_back(make_pair(ptSeq.store[j].lng,ptSeq.store[j].lat));
				continue;
			}
			if(time_raw >= t2 - 0.0001)
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
			//if(store[i].obdTime - store[0].obdTime < 50 || store[store.size()-1].obdTime - store[i].obdTime < 50)//fen jie tongji,fixedrateseq also take into count
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
		}
	}
	area = a;
}

void MDPSeq::calcAL()
{
	AL = length[number - 1]/(number - 1);
}

void MDPSeq::calcSD()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SD += (length[i] - length[i-1] - AL)*(length[i] - length[i-1] - AL);
	}
	SD /= number - 1;
}
void MDPSeq::calcSDL()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SDL += (length[i] - length[i-1] - L)*(length[i] - length[i-1] - L);
	}
	SDL /= number - 1;
}

void MDPSeq::calcDistance()
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

