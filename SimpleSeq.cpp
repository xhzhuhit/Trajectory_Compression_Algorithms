#include "SimpleSeq.h"


SimpleSeq::SimpleSeq(pointSeq pS,double r,double l,double al,int isAdap)
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

	//generate the Markov sequence

	double t1 = 0, t2 = 0;
	bool isLastOne = false;

	double adaptivePram = 1;

	sItem tmp;
	tmp.order = ptSeq.store[0].order;
	tmp.obdTime = ptSeq.store[0].obdTime + t2;
	pair<double,double> temp = ptSeq.getLocationT(t2);
	tmp.lng = temp.first;
	tmp.lat = temp.second;
	tmp.speed = ptSeq.getSpeed(t2);
	store.push_back(tmp);
	

	while(!isLastOne)
	{
		double presentV = pS.getSpeed(t1);
		double	nextV = presentV;
		if(nextV < V_SEG[0])
			nextV = V_SEG[0]/2;
		if(nextV > V_SEG[8])
			nextV = V_SEG[8] + 3;

		if(isAdaptive)
		{
			//adaptivePram = (presentLevel/10 + 5)/5;
			//adaptivePram = (presentLevel/10 + 5)/5;
			adaptivePram = (nextV > 10 )?1.2:1;
		}
		

		double increase = adaptivePram*L/nextV;
		int low = L/20;
		int high = L/5;
		if(increase > high)
			increase = high;
		if(increase < low)
			increase = low;
		t2 += increase;
			
		//if(t2>903)
		//	cout<<endl;
		if(t2 >= pS.totalTime)
		{
			t2 = pS.totalTime;
			isLastOne = true;
		}

		sItem tmp;
		tmp.order = ptSeq.store[0].order;
		tmp.obdTime = ptSeq.store[0].obdTime + t2;
		pair<double,double> temp = ptSeq.getLocationT(t2);
		tmp.lng = temp.first;
		tmp.lat = temp.second;
		tmp.speed = ptSeq.getSpeed(t2);
		store.push_back(tmp);

		t1 = t2;
	}


	
}

void SimpleSeq::setTimeUnit(double r)
{
	timeUnit = r;
}
void SimpleSeq::setL(double l)
{
	L = l;
}
void SimpleSeq::setAlpha(double al)
{
	alpha = al;
}

SimpleSeq::~SimpleSeq(void)
{
}

pair<double,double> SimpleSeq::getLocationL(double l)
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

pair<double,double> SimpleSeq::getLocationT(double t)
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


void SimpleSeq::calc()
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

void SimpleSeq::calcRR()
{
	int count = 0;
	for(int i = 0;i < store.size()-1; i++)
	{
		count += Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat)< alpha*L;
	}
	nRR = count;
	RR = (double)count/store.size();
}

void SimpleSeq::calcAD()
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

void SimpleSeq::calcArea()
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
		
		if(middlePoint.size() != 0 )
		{
			c++;
			if(c == 77)
				cout<<"";
			double temp = countArea_new(make_pair(store[i].lng,store[i].lat),make_pair(store[i+1].lng,store[i+1].lat),middlePoint);
			
				//cout<< temp <<"("<<c<<")  ";
			a +=temp;
		}
	}
	area = a;
}

void SimpleSeq::calcAL()
{
	AL = length[number - 1]/(number - 1);
}

void SimpleSeq::calcSD()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SD += (length[i] - length[i-1] - AL)*(length[i] - length[i-1] - AL);
	}
	SD /= number - 1;
}
void SimpleSeq::calcSDL()
{
	for(int i = 1;i <= number - 1; i++ )
	{
		SDL += (length[i] - length[i-1] - L)*(length[i] - length[i-1] - L);
	}
	SDL /= number - 1;
}