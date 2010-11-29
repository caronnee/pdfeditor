#ifndef __GLOBAL_F__
#define __GLOBAL_F__

#include <cmath>
//TODO asi by t mohlo byt vacsie, na viac desatinnych miest
#define PI 3.14

#define min(a,b) (a) < (b)? (a):(b)
#define max(a,b) (a) > (b)? (a):(b) 

float toRadians(int angle);
int toDegree(float angle);

template<typename T>
void rotatePosition( T xin, T yin, T& xout, T &yout, int angle )
{
	float s = sqrt(xin*xin+yin*yin);
	xout = s*cos(toRadians(angle));
	yout = s*sin(toRadians(angle));
};
#endif // __GLOBAL_F__
