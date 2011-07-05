#include "page.h"
#include "globalfunctions.h"
#include "debug.h"
#include <QPainter>

void DisplayPage::drawRectangle(QRect rect)
{
//TODO
}
DisplayPage::DisplayPage(QWidget *parent)
	: QLabel(parent), _mousePressed(false)
{
	 setBackgroundRole(QPalette::Base);
     setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	 setContextMenuPolicy(Qt::CustomContextMenu);
     setScaledContents(true);
	 menu = new QMenu();
	 menu->addAction("InsertText",this,SLOT(insertText()));
	 menu->addAction("Delete",this,SLOT(deleteText()));
	 menu->addAction("Erase", this, SLOT(eraseText()));
	 menu->addAction("Change",this,SLOT(changeText()));
	 menu->addAction("InsertImage",this,SLOT(insertImage()));
	 menu->addAction("DeleteImage",this,SLOT(deleteImage()));
	 menu->addAction("Annotate",this,SLOT(annotation()));
	 menu->addAction("Delete Annotation",this,SLOT(deleteAnnotation()));
	 menu->addAction("Change image",this,SLOT(changeImage()));
}

QPoint DisplayPage::convertCoord(QPoint point)
{
	return (QPoint(point.x(),this->size().height() - _point.y()));
}
void DisplayPage::changeImage()
{
	QPoint p(_point.x(), this->size().height() - _point.y());
	emit ChangeImageSignal(p);//vieme o ktory obrazok ide
}
void DisplayPage::deleteAnnotation()
{
	emit DeleteAnnotationSignal(_point);
}
void DisplayPage::annotation()
{
	emit AnnotationSignal(_point); //TODO emit also rectangle
}
void DisplayPage::deleteImage()
{
	QPoint p(_point.x(), this->size().height() - _point.y());
	emit DeleteImageSignal(p);
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
void DisplayPage::markPosition(QPoint point)
{
	QImage resultImage(_copyImg.size(),QImage::Format_ARGB32_Premultiplied);
	QImage image("images/spot.gif");

	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.drawImage(0, 0, _copyImg);
	painter.translate(point);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawImage(0,0,image);
	painter.end();
	this->setPixmap(QPixmap::fromImage(resultImage));
}
void DisplayPage::fillRect(QRect rect,QColor color)
{
	QImage resultImage(_copyImg.size(),QImage::Format_ARGB32_Premultiplied);

	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.fillRect(rect, color);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.drawImage(0, 0, _copyImg);
	painter.end();

	this->setPixmap(QPixmap::fromImage(resultImage));
}
void DisplayPage::fillRect( QVector<QRect>& r, const QColor color)
{
	QImage resultImage(_copyImg.size(),QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	for ( int i= 0; i < r.size(); i++)
	{
		painter.fillRect(r[i], color);
	}
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.drawImage(0, 0, _copyImg);

	painter.end();
	this->setPixmap(QPixmap::fromImage(resultImage));
}
	
void DisplayPage::fillRect( int x1, int y1, int x2, int y2, const QColor color)
{
//	QPainter painter(&_copyImg); //mozno az na this?
	int sx = (x1 > x2) ? x1 - x2:x2 - x1;
	int sy = (y1 > y2) ? y1 - y2:y2 - y1;
	QRect r(min(x1,x2),min(y1,y2),sx,sy);
////	painter.setCompositionMode(QPainter::CompositionMode_Source);
//	painter.fillRect(r, color);
//	setImg();
	fillRect(r,color);
	//needrepaint? TODO
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
		emit MouseClicked(event->pos());
		//emit highlightText(event->x(), this->size().height() - event->y());
}

void DisplayPage::mouseReleaseEvent(QMouseEvent * event)
{
	_mousePressed = false;
//	event->ignore(); //posun to parentovi
	emit MouseReleased(event->pos()); //:)
}
