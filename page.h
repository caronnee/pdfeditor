#ifndef DISPLAY_PAGE_H
#define DISPLAY_PAGE_H

#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include "ui_page.h"

class DisplayPage : public QWidget
{
	Q_OBJECT

	std::vector<QRect> _interactive;
public:
	DisplayPage(QWidget *parent = 0);
	~DisplayPage();
	void setImage( const QImage & image);
	//adds interactive rectagle (annotation)
	void addPlace(QRect r)
	{
		_interactive.push_back(r);
		//FIXME zotriedit pre lepsi pristup
	}
private:
	void setImg();
private:
	QImage _image;
	QImage _copyImg;
	Ui::page ui;

	//we have to remeber original size
	QSize _size;

public slots:
	void fillRect( QRegion reg, const QColor color);
	void zoom(int zoomscale);
	void mousePressEvent(QMouseEvent * event);
	void fillRect(int x, int y, int x2, int y2, const QColor color);
signals:
	void MouseClicked(int, int); //bolo na mna kliknute, robte s tym nieco!
	void highlightText(int x, int y);
public:
	void unsetImg();
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleased(QMouseEvent * event);
	
};

#endif // PAGE_H
