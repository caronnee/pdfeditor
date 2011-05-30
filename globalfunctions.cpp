#include "globalfunctions.h"

float toRadians(int angle)
{
	return PI* angle/180;
}
int toDegree(float angle)
{
	return 180*angle/PI;
}
void rotate(int angle, double& x, double& y)
{
	float an = toRadians(angle);
	double xx = x*cos(an) + y*sin(an);  //po smere hodinych ruciciek
	double yy = -x*sin(an) + y*cos(an);
	x = xx;
	y = yy;
}
