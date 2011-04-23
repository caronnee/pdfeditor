#include "page.h"
#include "globalfunctions.h"
#include "debug.h"
#include <QPainter>

DisplayPage::DisplayPage(QWidget *parent)
	: QLabel(parent), _mousePressed(false)
{
	 setBackgroundRole(QPalette::Base);
     setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	 setContextMenuPolicy(Qt::CustomContextMenu);
     setScaledContents(true);
	 menu = new QMenu();
	 menu->addAction("Test");
	 menu->addAction("Test2");
	 menu->addAction("Test3");
}
void DisplayPage::contextMenuEvent( QContextMenuEvent* event )
{
	menu->exec(event->globalPos());
}
DisplayPage::~DisplayPage(){}


void DisplayPage::setImage( const QImage & image )
{
	_image = image.copy();
	unsetImg();
}
void DisplayPage::fillRect( QRegion region, const QColor color)
{
	QVector<QRect> r = region.rects();
	for ( int i= 0; i < r.size(); i++)
	{
		fillRect(r[i].x(), r[i].y(), r[i].width(), r[i].height(),color);
	}
}
	
void DisplayPage::fillRect( int x1, int y1, int x2, int y2, const QColor color)
{
	QPainter painter(&_copyImg); //mozno az na this?
	y1 = size().height() - y1; //TODO pixmap size! for displaying
	y2 = size().height() - y2;
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
	this->setPixmap(QPixmap::fromImage(_copyImg));
	this->adjustSize();
	_size = this->pixmap()->size();
}
void DisplayPage::unsetImg() //against from image, for removing highligh and so
{
	_copyImg = _image.copy();
	setImg();
}
void DisplayPage::mousePressEvent(QMouseEvent * event)
{
	//pass parent the coordinates
	//chceme suradnice vzhladom na label //TODO co ak budeme v continuous mode?
	QPoint point = event->pos();
	QSize s= pixmap()->size();
	_mousePressed = true;
	mouseMoveEvent(event);
}
void DisplayPage::mouseMoveEvent(QMouseEvent * event)
{
	if (_mousePressed)
		emit MouseClicked(event->x(), this->size().height() - event->y());
		//emit highlightText(event->x(), this->size().height() - event->y());
}

void DisplayPage::mouseReleaseEvent(QMouseEvent * event)
{
	_mousePressed = false;
//	event->ignore(); //posun to parentovi
	emit MouseReleased(); //:)
}
