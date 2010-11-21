#ifndef DISPLAY_PAGE_H
#define DISPLAY_PAGE_H

#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include "ui_page.h"

class DisplayPage : public QWidget
{
	Q_OBJECT

public:
	DisplayPage(QWidget *parent = 0);
	~DisplayPage();
	void setImage( const QImage & image);
private:
	void unsetImg();
	void setImg();
private:
	QImage _image;
	QImage _copyImg;
	Ui::page ui;

	//we have to remeber original size
	QSize _size;

public slots:
	void zoom(int zoomscale);
	void mousePressEvent(QMouseEvent * event);
	void fillRect(int x, int y, int x2, int y2, const QColor color);
signals:
	void MouseClicked(int, int); //bolo na mna kliknute, robte s tym nieco!
};

#endif // PAGE_H
