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
	QImage _image;
	Ui::page ui;

	//we have to remeber original size
	QSize _size;

public slots:
	//scales the image according to factor
	void zoom(int zoomscale);
	void mousePressEvent(QMouseEvent * event);
	void fillRect(const QRect rect, const QColor color);
signals:
	void MouseClicked(int, int); //bolo na mna kliknute, robte s tym nieco!
};

#endif // PAGE_H
