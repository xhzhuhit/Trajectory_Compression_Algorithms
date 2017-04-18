#include "pointSeq.h"
#include "utility.h"
pointSeq::pointSeq(void)
{
	number = 0;
	totalTime = 0;
	totalLength = 0;
}


pointSeq::~pointSeq(void)
{
	clear();
}

void pointSeq::clear()
{
	store.clear();
	time.clear();
	number = 0;
	totalTime = 0;
	a.clear();
	b.clear();
	c.clear();
	length.clear();
	totalLength = 0;
}
void pointSeq::calc()
{
	//bool flag = true;
	number = store.size();
	time.push_back(0);
	length.push_back(0);
	for(int i = 0;i < number - 1; i ++ )
	{
		double ti = store[i+1].obdTime - store[i].obdTime;
		totalTime += ti;
		time.push_back(totalTime);
		raw_deta_time.push_back(ti);//输出原始时间jiange

		double s = Distance(store[i].lng,store[i].lat,store[i+1].lng,store[i+1].lat);
		//if(s > 100000)
			//return false;//验证是否留下这条轨迹
		totalLength += s;
		length.push_back(totalLength);
		raw_deta_length.push_back(s);//输出原始距离jiange

		double v1 = store[i].speed;
		double v2 = store[i+1].speed;


		//v = a2 * t^2 + a1 * t + a0
		double a0 = v1;
		double a1 = -4 * v1 /ti - 2* v2 / ti + 6 * s /(ti * ti);
		double a2 = 3*v1/(ti * ti) + 3 * v2 / (ti * ti ) - 6 * s / (ti * ti * ti);
		//xianxing
		//double a0 = v1;
		//double a1 = (v2 - v1) / ti;
		//double a2 = 0;

		a.push_back(a2);
		b.push_back(a1);
		c.push_back(a0);
	}
	//return true;
}

double pointSeq::getAcc(double t)
{
	int i;
	for(i = 0; i < number; i ++)
	{
		if(time[i] <= t && time[i+1] >= t)
			break;
	}
	return (2*a[i]*(t - time[i]) + b[i]);
}

pair<double,double> pointSeq::getLocationT(double t)
{
	int i;
	for(i = 0; i < number; i ++)
	{
		if(time[i] <= t && time[i+1] >= t)
			break;
	}
	double s0 = 1.0/3*a[i]*pow((t - time[i]),3) + 1.0/2*b[i]*pow((t - time[i]),2) + c[i]*(t - time[i]);
	double s = length[i+1] - length[i];
	double lng,lat;
	if(fabs((double)s) < 0.1)
	{
		lng = store[i].lng;
		lat = store[i].lat;
	}
	else
	{
		lng = store[i].lng+s0/s*(store[i+1].lng - store[i].lng);
		lat = store[i].lat+s0/s*(store[i+1].lat - store[i].lat);
	}
	return make_pair(lng,lat);
}

pair<double,double> pointSeq::getLocationL(double l)
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
	if(fabs(s) < 0.1)
	{
		lng = store[i].lng;
		lat = store[i].lat;
	}
	else
	{
		lng = store[i].lng+s0/s*(store[i+1].lng - store[i].lng);
		lat = store[i].lat+s0/s*(store[i+1].lat - store[i].lat);
	}
	return make_pair(lng,lat);
}

double pointSeq::l2t(double l)
{
	int i;
	for(i = 0; i < number - 1; i ++)
	{
		if(length[i] <= l && length[i+1] >= l)
			break;
	}
	double t = time[i];
	while(1/3.0*a[i]*pow((t-time[i]),3)+1/2.0*b[i]*pow((t-time[i]),2)+c[i]*(t-time[i]) < l - length[i])
		t += 0.05;
	return t;
}

double pointSeq::getSpeed(double t)
{
	int i;
	for(i = 0; i < number; i ++)
	{
		if(time[i] <= t && time[i+1] >= t)
			break;
	}
	//return (a[i]*(t - time[i])*(t - time[i]) + b[i]*(t - time[i]) + c[i]);//直接返回可能有过大或者负值
	double spee = (a[i]*(t - time[i])*(t - time[i]) + b[i]*(t - time[i]) + c[i]);
	if(spee > 35)
		spee = 35;
	if(spee < 0)
		spee = 0.5;
	return spee;
}

double pointSeq::caDistance(sItem p1,sItem p2)
{
	double a = 0;
	int start = 0;
	int c =0;
	
	double dist = 0;

	double t1 = p1.obdTime - store[0].obdTime;
	double t2 = p2.obdTime - store[0].obdTime;
	vector <pair<double,double> > middlePoint;
	for(int j = start; j < number; j ++ )
	{
		if(time[j] > t1 + 0.0001 && time[j] < t2 - 0.0001)
		{
			middlePoint.push_back(make_pair(store[j].lng,store[j].lat));
			continue;
		}
		if(time[j] >= t2 - 0.0001)
		{
			start = j;
			break;
		}
	}

	if(middlePoint.size() != 0 )
	{
		int size = middlePoint.size();
		if(middlePoint.size() == 1)
			dist = Distance(p1.lng,p1.lat,middlePoint[0].first,middlePoint[0].second) + Distance(middlePoint[0].first,middlePoint[0].second,p2.lng,p2.lat);
		else
		{
			dist += Distance(p1.lng,p1.lat,middlePoint[0].first,middlePoint[0].second);
			dist += Distance(middlePoint[0].first,middlePoint[0].second,p2.lng,p2.lat);
			for(int m = 0;m < size-1;m++)
			{
				dist += Distance(middlePoint[m].first,middlePoint[m].second,middlePoint[m+1].first,middlePoint[m+1].second);
			}
		}
	}
	else
		dist += Distance(p1.lng,p1.lat,p1.lng,p1.lat);
	
	return dist;
}

vector <double> pointSeq::calcProjection(sItem p1,sItem p2,sItem p3)//p1,p3为保留点
{
	vector <double> xyt;
	double LPerD = 111319.55;
	//以p1为原点，求出第二点坐标
	double change_angl2 = p2.lat / 180 * PI;
	double x2 = LPerD * (p2.lng - p1.lng) * cos(change_angl2);
	double y2 = LPerD * (p2.lat - p1.lat);
	double t2 = p2.obdTime - p1.obdTime;
	//以p1为原点，求出第三点坐标
	double change_angl3 = p3.lat / 180 * PI;
	double x3 = LPerD * (p3.lng - p1.lng) * cos(change_angl3);
	double y3 = LPerD * (p3.lat - p1.lat);
	double t3 = p3.obdTime - p1.obdTime;

	double rate_k = (x2 * x3 + y2 * y3 + t2 * t3) / (x3 * x3 + y3 * y3 + t3 * t3);//按垂直计算
	double x2_re = rate_k * x3;
	double y2_re = rate_k * y3;
	double t2_re = rate_k * t3;

	//double speed2_re = p1.speed + rate_k * (p3.speed - p1.speed);
	double speed2_re = sqrt(x3 * x3 + y3 * y3) / t3;

	//从已压缩的点考虑，若要恢复，就要基于压缩后的点制定规则，才能从压缩后的点的基础上恢复出其它点
/*	double dist = p1.speed * t2 + 0.5 * (p3.speed - p1.speed) / t3 * pow(t2,2);
	double rate_k = dist / Distance(p1.lng,p1.lat,p3.lng,p3.lat);//按垂直计算
	double x2_re = rate_k * x3;
	double y2_re = rate_k * y3;
	double t2_re = rate_k * t3;
*/
	xyt.push_back(abs(x2 - x2_re));
	xyt.push_back(abs(y2 - y2_re));
	//xyt.push_back(abs(t2 - t2_re));//要求速度指标，故第三项暂时改为speed
	xyt.push_back(abs(p2.speed - speed2_re));

	return xyt;
}

simple_point pointSeq::calcArcPoint(sItem p1,sItem p2,int time,double angle_recovery,int number)
{
	double LPerD = 111319.55;
	//以p1为原点，求出第二点坐标
	double change_angl2 = p2.lat / 180 * PI;
	double x2 = LPerD * (p2.lng - p1.lng) * cos(change_angl2);
	double y2 = LPerD * (p2.lat - p1.lat);
	
	//计算三角形两个角
	double angle1 = ((PI - angle_recovery) / (2 * number)) * time;
	double angle2 = (PI - angle_recovery) / 2 - angle1;
	double p1_p2_distance = sqrt(x2 * x2 + y2 * y2);

	double fenmu = 1 + pow(sin(angle2) / sin(angle1),2) - 2 * cos(angle_recovery) * sin(angle2) / sin(angle1);//fen mu
	double d = sqrt(pow(p1_p2_distance,2) / fenmu);

	double true_angle = 0;
	if(x2 > 0)
	{
		true_angle = angle2 + atan(y2 / x2);//真实角度要加上旋转角度
	}
	if(x2 < 0)
	{
		true_angle = angle2 + atan(y2 / x2) + PI;//真实角度要加上旋转角度
	}
	//计算投影点x，y
	double x = d * cos(true_angle);
	double y = d * sin(true_angle);
	//把x，y转换成经纬度
	double lng = x / (LPerD * cos(change_angl2)) + p1.lng;//姑且认为在同一纬度，所以用同一个change_angle2
	double lat = y / LPerD + p1.lat;

	//接下来要求除弧长，计算速度
	double radius = p1_p2_distance / (2 * cos(angle_recovery / 2));
	double arc_length = (PI - angle_recovery) * radius;
	double speed = arc_length / number;

	return simple_point(lng,lat,speed);
}