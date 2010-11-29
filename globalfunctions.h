#ifndef __GLOBAL_F__
#define __GLOBAL_F__

#include <cmath>
//TODO asi by t mohlo byt vacsie, na viac desatinnych miest
#define PI 3.14
template<typename T>
T min( T a , T b) { return a < b? a:b; }

template<typename T>
T max( T a , T b) { return a > b? a:b; }

float toRadians(int angle)
{
	return PI * angle /180;
}
int toDegree(float angle)
{
	return 1800 * angle/PI;
}
#endif // __GLOBAL_F__
