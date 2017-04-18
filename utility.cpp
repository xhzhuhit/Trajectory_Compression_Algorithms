#include "utility.h"
Time::Time(int iyear,int imonth, int iday,int ihour,int iminute,double isecond)
{
	year = iyear;
	month = imonth;
	day = iday;
	hour = ihour;
	minute = iminute;
	second = isecond;
}
Time operator+(Time t,double gap)
{
	double tmp = t.getSecond() + gap;
	return t.getTime(tmp);
}
Time operator-(Time t,double gap)
{
	double tmp = t.getSecond() - gap;
	return t.getTime(tmp);
}

double operator-(Time t1,Time t2)
{
	return t1.getSecond() - t2.getSecond();
}

bool operator<(Time t1,Time t2)
{
	return t1.getSecond() < t2.getSecond();
}
bool operator>(Time t1,Time t2)
{
	return t1.getSecond() > t2.getSecond();
}
bool operator==(Time t1,Time t2)
{
	return fabs(t1.getSecond() - t2.getSecond()) < 0.01;
}

bool Time::isLeap()
{
	return year%4==0&&year%100!=0||year%400==0;
}
void Time::showTime()
{
	cout<<"Time: "<<year<<"-"<<month<<"-"<<day<<" "<<hour<<":"<<minute<<":"<<setprecision(2)<<second<<endl;
}
double Time::getSecond()
{
	int iyear = year - 1970;
	int addDay = (iyear + 1)/4;
	int iday = iyear * 365 + addDay;
	int dayPerMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	for(int i = 0; i< month - 1; i ++)
		iday += (dayPerMonth[i] + (this->isLeap()&&i==1?1:0));
	iday += (day - 1);
	return iday * 3600 *24 + hour * 3600 + minute * 60 + second;
}
Time Time::getTime(double g)
{
	Time tmp;
	int gap = g;
	int iday = gap/(3600*24);
	int leftSec = gap - 3600 * 24 * iday;
	tmp.hour = leftSec/3600;
	tmp.minute = (leftSec - tmp.hour * 3600)/60;
	tmp.second = leftSec%60;
	tmp.second += g - gap;

	int iyear = (int)(iday/365.25);
	tmp.year = 1970 + iyear;
	int addDay = (iyear + 1)/4;
	iday = iday - iyear * 365 -addDay;
	int dayPerMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	for( i = 0;i < 12; i++)
	{
		if(iday - (dayPerMonth[i] + (this->isLeap()&&i==1?1:0)) > 0)
			iday -= (dayPerMonth[i] + (this->isLeap()&&i==1?1:0));
		else
			break;
	}
	tmp.month = i + 1;
	tmp.day = iday + 1;
	return tmp;
}

void Time::setTime(double gap)
{
	(*this) = this->getTime(gap);
}



vector<string> splitEx(const string& src, string separate_character)
{
    vector<string> strs;

    int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
    int lastPosition = 0,index = -1;
	while (-1 != (index = src.find_first_of(separate_character,lastPosition)))
    {
        strs.push_back(src.substr(lastPosition,index - lastPosition));
        lastPosition = index + 1;
    }
    string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
    if (!lastString.empty())
        strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
    return strs;
}
vector<string> splitEx_NoEmpty(const string& src, string separate_character)
{
    vector<string> strs;

    int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
    int lastPosition = 0,index = -1;
	while (-1 != (index = src.find_first_of(separate_character,lastPosition)))
    {
        if(!(src.substr(lastPosition,index - lastPosition)).empty())
			strs.push_back(src.substr(lastPosition,index - lastPosition));
        lastPosition = index + 1;
    }
    string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
    if (!lastString.empty())
        strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
    return strs;
}



double Distance(double lng1,double lat1,double lng2,double lat2)//count the distance between two points
{
	double x1 = R*cos(lat1*PI/180)*cos(lng1*PI/180);
	double y1 = R*cos(lat1*PI/180)*sin(lng1*PI/180);
	double z1 = R*sin(lat1*PI/180);

	double x2 = R*cos(lat2*PI/180)*cos(lng2*PI/180);
	double y2 = R*cos(lat2*PI/180)*sin(lng2*PI/180);
	double z2 = R*sin(lat2*PI/180);

	double L = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));

	return R*2*asin(0.5*L/R);
}

// 点的叉乘: AB * AC
double cross( point A,  point B, point C) {
    return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}

point intersection(point p1,point p2,point p3,point p4)// return the intersection of two points
{
	point p;
    double temp1=cross(p1,p3,p4),temp2=cross(p2,p3,p4);
    p.x=(temp1*p2.x-temp2*p1.x)/(temp1-temp2);
    p.y=(temp1*p2.y-temp2*p1.y)/(temp1-temp2);
	return p;
}
double integral(point p1,point p2,point p3,point p4,double x,double y)//几分
{
	double a = 0;

	double x1 = p1.x,x2 = p2.x,x3 = p3.x,x4 = p4.x;
	double y1 = p1.y,y2 = p2.y,y3 = p3.y,y4 = p4.y;
	if(fabs(x2-x1) < 0.0001 || fabs(x4 - x3) < 0.0001)
		return 0;
	a += 0.5* ((y4-y3)/(x4-x3) - (y2-y1)/(x2-x1)) * (y*y - x*x);
	a += ((x1*y2 - x1*y1)/(x2-x1) - (x3*y4 - x3*y3)/(x4-x3) + y3 - y1) * (y-x);
	return a;
}
double countArea_old(pair<double,double> start,pair<double,double> end, vector <pair<double,double> > middle)
// old method for calculating area
{
	double a = 0;
	int dirX = (end.first - start.first) >= 0 ? 1:-1;
	int dirY = (end.second - start.second) >= 0 ? 1:-1;//used to adjust the coordintes in the first quadrant

	double LPerD = 111319.55;//length per degree
	point startP,endP;
	startP.x = startP.y = 0;
	double tempEndX = dirX*LPerD*(end.first - start.first)* cos(start.second);
	double tempEndY = dirY*LPerD*(end.second - start.second);
	double rotateDegree;
	if(fabs(tempEndX) < 0.0001)
		rotateDegree = -PI/4;
	else
		rotateDegree = PI/4 - atan(tempEndY/tempEndX);
	endP.x = tempEndX*cos(rotateDegree) + tempEndY*sin(rotateDegree);
	endP.y = -tempEndX*sin(rotateDegree) + tempEndY*cos(rotateDegree);
	point *middleP = new point[middle.size()];
	for(int i = 0; i < middle.size(); i ++)
	{
		double tempX = dirX*LPerD*(middle[i].first - start.first)* cos(start.second);
		double tempY = dirY*LPerD*(middle[i].second - start.second);

		middleP[i].x = tempX*cos(rotateDegree) + tempY*sin(rotateDegree);
		middleP[i].y = -tempX*sin(rotateDegree) + tempY*cos(rotateDegree);
	}

	//check meaningful
	/*if(middlep[0].x <= startp.x || middlep[0].y <= startp.y)
		return 0;
	if(endp.x <= middlep[middle.size()-1].x || endp.y <= middlep[middle.size()-1].y)
		return 0;
	for(int i = 0; i < middle.size() - 1; i ++)
	{
		if(middlep[i+1].x <= middlep[i].x || middlep[i+1].y <= middlep[i].y)
			return 0;
	}*/

	//count

	a += fabs(integral(startP,endP,startP,middleP[0],startP.x,middleP[0].x));

	for(int i = 0; i < middle.size() - 1; i ++)
	{
		if(fabs(middleP[i].x-middleP[i+1].x) < 0.0001 || fabs(middleP[i].y - middleP[i+1].y) < 0.0001)
			continue;
		if((middleP[i].y/middleP[i].x - endP.y/endP.x)*(middleP[i+1].y/middleP[i+1].x - endP.y/endP.x) > 0)
			a += fabs(integral(startP,endP,middleP[i],middleP[i+1],middleP[i].x,middleP[i+1].x));
		else
		{
			point intersec;
			intersec = intersection(startP,endP,middleP[i],middleP[i+1]);
			a += fabs(integral(startP,endP,middleP[i],intersec,middleP[i].x,intersec.x));
			a += fabs(integral(startP,endP,intersec,middleP[i+1],intersec.x,middleP[i+1].x));
		}
	}

	a += fabs(integral(startP,endP,middleP[middle.size() - 1],endP,middleP[middle.size() - 1].x,endP.x));
	return a;
}

double polygon_areas( vector<point> p)//计算多边形面积公式
{
    double area = 0;
    int i;
	int n = p.size();
    point temp;

    temp.x = temp.y = 0;//原点
    for (i = 0; i < n - 1; ++i){
        area += cross(temp, p[i], p[i+1]);
    }
    //area += cross(temp, p[n-1], p[0]);//首尾相连
    area = area/2.0;        //注意要除以2
    return area > 0 ? area : -area;    //返回非负数
}

double countArea_new(pair<double,double> start,pair<double,double> end, vector <pair<double,double> > middle)
{
	double a = 0;
	int dirX = (end.first - start.first) >= 0 ? 1:-1;
	int dirY = (end.second - start.second) >= 0 ? 1:-1;//used to adjust the coordintes in the first quadrant

	double LPerD = 111319.55;//length per degree
	point startP,endP;
	startP.x = startP.y = 0;

	//double pp = cos(start.second);//这里出大问题了！！！！
	//double tempEndX = dirX*LPerD*(end.first - start.first)* cos(start.second);//大问题！！！
	double change_angle = start.second / 180 * 3.1415926;
	double tempEndX = dirX*LPerD*(end.first - start.first)* cos(change_angle);

	double tempEndY = dirY*LPerD*(end.second - start.second);//juli
	double rotateDegree;
	if(fabs(tempEndX) < 0.0001)
		rotateDegree = -PI/4;
	else
		rotateDegree = PI/4 - atan(tempEndY/tempEndX);
	endP.x = tempEndX*cos(rotateDegree) - tempEndY*sin(rotateDegree);
	endP.y = tempEndX*sin(rotateDegree) + tempEndY*cos(rotateDegree);
	vector<point> middleP;

	double preX = startP.x;
	double preY = startP.y;
	for(int i = 0; i < middle.size(); i ++)
	{
		double change_angl = start.second / 180 * 3.1415926;
		double tempX = dirX*LPerD*(middle[i].first - start.first)* cos(change_angl);//问题所在

		double tempY = dirY*LPerD*(middle[i].second - start.second);

		double x = tempX*cos(rotateDegree) - tempY*sin(rotateDegree);
		double y = tempX*sin(rotateDegree) + tempY*cos(rotateDegree);
		if(fabs(x-preX)>0.0001 && fabs(y-preY)>0.0001)
		{
			point tmp;
			tmp.x = x;
			tmp.y = y;
			middleP.push_back(tmp);
			preX = x;
			preY = y;
		}
	}
	if( middleP.size() > 0 && fabs(middleP.back().x - endP.x) < 0.0001 && fabs(middleP.back().y - endP.y) < 0.0001 )
		middleP.pop_back();

	//middleP.push_back(endP);//添加部分！！

	//count
	if(middleP.size() < 1)//<= 1 ??是不是有问题？
		return 0;
	vector<point> p;
	p.push_back(startP);
	p.push_back(middleP[0]);
	int preDir,curDir;
	preDir = (middleP[0].y > middleP[0].x)? 1: -1;
	if(middleP.size() > 1)
	{
		for(int i = 1; i < middleP.size() ; i ++)
		{
			curDir = (middleP[i].y > middleP[i].x)? 1: -1;
			if(preDir*curDir == 1)
				p.push_back(middleP[i]);
			else
			{
				point intersec;
				if(fabs(middleP[i-1].x - middleP[i-1].y)<0.0001 && fabs(middleP[i].x - middleP[i].y)<0.0001)
					return 0;
				intersec = intersection(startP,endP,middleP[i-1],middleP[i]);
				p.push_back(intersec);
				a += polygon_areas(p);
				preDir = curDir;
				p.clear();
				p.push_back(intersec);
				p.push_back(middleP[i]);
			}
		}
	}
	p.push_back(endP);
	a += polygon_areas(p);
	return a;
}



	

double LineDistance(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3)
{
	double LPerD = 111319.55;
	//以lng1，lat1为原点，求出第二点坐标
	double change_angl2 = lat2 / 180 * 3.1415926;
	double X2 = LPerD*(lng2 - lng1)* cos(change_angl2);
	double Y2 = LPerD*(lat2 - lat1);
	//以lng1，lat1为原点，求出第三点坐标
	double change_angl3 = lat3 / 180 * 3.1415926;
	double X3 = LPerD*(lng3 - lng1)* cos(change_angl3);
	double Y3 = LPerD*(lat3 - lat1);

	//求出第二点到一、三点连线的距离
	double a = abs(X3 * Y2 - X2 * Y3);
	double b = sqrt(X3 * X3 + Y3 * Y3);
	return a/b;
}

double SEDistance(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3,double t1,double t2)
{
	double LPerD = 111319.55;
	//以lng1，lat1为原点，求出第二点坐标
	double change_angl2 = lat2 / 180 * 3.1415926;
	double X2 = LPerD*(lng2 - lng1)* cos(change_angl2);
	double Y2 = LPerD*(lat2 - lat1);
	//以lng1，lat1为原点，求出第三点坐标
	double change_angl3 = lat3 / 180 * 3.1415926;
	double X3 = LPerD*(lng3 - lng1)* cos(change_angl3);
	double Y3 = LPerD*(lat3 - lat1);

	//求出复原点
	double x_re = X3 * t1 / t2;
	double y_re = Y3 * t1 / t2;
	
	double distan = sqrt((X2 - x_re) * (X2 - x_re) + (Y2 - y_re) * (Y2 - y_re));
	return distan;
}

double calcAngle(double lng1,double lat1,double lng2,double lat2,double lng3,double lat3)//lng2,lat2代表顶点
{
	double LPerD = 111319.55;
	//以lng1，lat1为原点，求出第二点坐标
	double change_angl2 = lat2 / 180 * 3.1415926;
	double X2 = LPerD*(lng2 - lng1)* cos(change_angl2);
	double Y2 = LPerD*(lat2 - lat1);
	//以lng2，lat2为原点，求出第三点坐标
	double change_angl3 = lat3 / 180 * 3.1415926;
	double X3 = LPerD*(lng3 - lng2)* cos(change_angl3);
	double Y3 = LPerD*(lat3 - lat2);

	//求出第二点到一、三点连线的距离
	double a = X2 * X2 + Y2 * Y2 - X2 * X3 - Y2 * Y3;
	double b = sqrt(X2 * X2 + Y2 * Y2) * sqrt((X3 - X2) * (X3 - X2) + (Y3 - Y2) * (Y3 - Y2));

	return acos(a/b);
}