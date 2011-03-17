#include "comments.h"

Comments::Comments(float x, float y)
{
	ui.setupUi(this);
	rect.yleft = y; //big line
	rect.xleft = x; //big line
	rect.yright = y+50; //big line
	rect.xright = x+50; //big line
}
void Comments::onChange(int index)
{
	//prida ten typ na urcity rect
	_an =  pdfobjects::CAnnotation::createAnnotation(rect, ui.annotType->currentText().toAscii().data());
	//cela annotacia sa bude diat v page
}
void Comments::apply()
{
	//ziti, ci je to text
	emit (annotation (_an));
}
