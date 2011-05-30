#ifndef __GLOBAL_F__
#define __GLOBAL_F__

#include <cmath>
//TODO asi by t mohlo byt vacsie, na viac desatinnych miest
#define PI 3.14

#define min(a,b) (a) < (b)? (a):(b)
#define max(a,b) (a) > (b)? (a):(b) 

float toRadians(int angle);
int toDegree(float angle);

void rotate(int angle, double& x, double& y);

template<typename T>
void rotatePosition( T xin, T yin, T& xout, T &yout, int angle )
{
	float s = sqrt(static_cast<double>(xin*xin+yin*yin));
	xout = s*cos(toRadians(angle));
	yout = s*sin(toRadians(angle));
};
#define ANNOTS(XX) \
	XX("Text", Text) \
	XX("Link", Link) \
	XX("Highlight", Highlight) \
	XX("Underline", Underline) \
	XX("Strikeout", Strike) \
	XX("", Supported) \

#define ENUMS(a,b) A##b,
#define CREATE_ARRAY(a,b)	a,

enum SupportedAnnotation
{
	ANNOTS(ENUMS)
};

#endif // __GLOBAL_F__
