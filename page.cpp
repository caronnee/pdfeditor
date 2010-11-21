#include "page.h"
#include "globalfunctions.h"
#include "debug.h"
#include <QPainter>

DisplayPage::DisplayPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

DisplayPage::~DisplayPage()
{

}
//slot
void DisplayPage::zoom(int zoomscale)//later woth how much pages, if all or not
{
	this->ui.label->resize(zoomscale * _size);
	//TODO adjust scrollbar?
}

void DisplayPage::setImage( const QImage & image )
{
	_image = image.copy();
	unsetImg();
}
void DisplayPage::fillRect( int x1, int y1, int x2, int y2, const QColor color)
{
	QPainter painter(&_copyImg);
	y1 = ui.label->size().height() - y1;
	y2 = ui.label->size().height() - y2;
	int sx = (x1 > x2) ? x1 - x2:x2 - x1;
	int sy = (y1 > y2) ? y1 - y2:y2 - y1;
	QRect r(min(x1,x2),min(y1,y2),sx,sy);
//	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(r, color);
	setImg();

	//needrepaint?
}
void DisplayPage::setImg() //again st from image, for removing highligh and so
{
	this->ui.label->setPixmap(QPixmap::fromImage(_copyImg));
	this->ui.label->adjustSize();
	_size = this->ui.label->pixmap()->size();
}
void DisplayPage::unsetImg() //agai st from image, for removing highligh and so
{
	_copyImg = _image.copy();
	setImg();
}
void DisplayPage::mousePressEvent(QMouseEvent * event)
{
	//pass parent the coordinates
	DEBUG("clicked on" <<event->x() << " " <<event->y());
	emit MouseClicked(event->x(), this->ui.label->size().height() - event->y()); //opacne kvoli pdfku
}
