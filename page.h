#ifndef DISPLAY_PAGE_H
#define DISPLAY_PAGE_H

#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include "ui_page.h"

class DisplayPage : public QLabel
{
	Q_OBJECT

private:
	/** kontext menu  */
	QMenu * menu;
	QPoint _point;
public:
	DisplayPage(QWidget *parent = 0);
	~DisplayPage();

	void setImage( const QImage & image);

	void fillRect(QRect rect,QColor color);
	//adds interactive rectagle (annotation)
	void addPlace(QRect r)
	{
		_interactive.push_back(r);
		//FIXME zotriedit pre lepsi pristup
		//ukaz
		QColor col(0,255,0);
		fillRect(r,col);
	}
	const QImage & getImage()const
	{
		return _image;
	}
	void setImg();
private:
	bool _mousePressed;
	//annotations
	std::vector<QRect> _interactive;

	//change handling
	QImage _image;
	QImage _copyImg;

	//we have to remeber original size
	QSize _size;

public slots:
	void annotation();
	void fillRect( QRegion reg, const QColor color);
	void mousePressEvent(QMouseEvent * event);
	void fillRect(int x, int y, int x2, int y2, const QColor color);
	void insertText();
	void deleteText();
	void eraseText();
	void changeText();
	void insertImage();
	void deleteImage();
	void deleteAnnotation();
signals:
	void DeleteAnnotationSignal(QPoint);
	void AnnotationSignal(QPoint);
	void DeleteImageSignal(QPoint);
	void InsertImageSignal(QPoint);
	void ChangeTextSignal();
	void EraseTextSignal();
	void DeleteTextSignal();
	void InsertTextSignal(QPoint point);
	void MouseClicked(int, int); //bolo na mna kliknute, robte s tym nieco!
	void highlightText(int, int);
	void MouseReleased();
public:
	void unsetImg();
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent * event);
	
};

#endif // PAGE_H
