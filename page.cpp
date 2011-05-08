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
	 menu->addAction("InsertText",this,SLOT(insertText()));
	 //connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	 menu->addAction("Delete",this,SLOT(deleteText()));
	 menu->addAction("Erase", this, SLOT(eraseText()));
	 menu->addAction("Change",this,SLOT(changeText()));
	 menu->addAction("InsertImage",this,SLOT(insertImage()));
	 menu->addAction("DeleteImage",this,SLOT(deleteImage()));
}
void DisplayPage::deleteImage()
{
	emit DeleteImageSignal(_point);
}
void DisplayPage::insertImage()
{
	emit InsertImageSignal(_point);
}
void DisplayPage::changeText()
{
	emit ChangeTextSignal();
}
void DisplayPage::eraseText()
{
	emit EraseTextSignal();
}
void DisplayPage::deleteText()
{
	emit DeleteTextSignal();
}
void DisplayPage::insertText()
{	
	emit InsertTextSignal(_point);
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
	_point = event->pos();
	//ak to bol lavy button, nerob nic)
	switch(event->button())
	{
	case Qt::RightButton:
		{
			menu->exec(event->globalPos());
			return;
		}
	case Qt::LeftButton:
		{
			//pass parent the coordinates
			//chceme suradnice vzhladom na label //TODO co ak budeme v continuous mode?
		
			_mousePressed = true;
			mouseMoveEvent(event);
			break;
		}
	default:
		{
			assert(false);
			break; //not implemented
		}
	}
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
