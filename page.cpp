#include "page.h"
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
	_image = image;
	this->ui.label->setPixmap(QPixmap::fromImage(image));
	this->ui.label->adjustSize();
	_size = this->ui.label->pixmap()->size();
}
void DisplayPage::fillRect( const QRect rect, const QColor color)
{
	QPainter painter(&_image);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(rect, color);
	//needrepaint
}
void DisplayPage::mousePressEvent(QMouseEvent * event)
{
	//pass parent the coordinates
	emit MouseClicked(event->x(), event->y());
}
