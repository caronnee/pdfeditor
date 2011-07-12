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

	void drawRectangle( QRect rect);
	void setImage( const QImage & image);

	void fillRect(QList<QRect> rects,QColor color);

	const QImage & getImage()const
	{
		return _image;
	}
	void setImg();
	QPoint convertCoord(QPoint point);
private:
	bool _mousePressed;
	//annotations
	QList<QRect> _interactive;

	//change handling
	QImage _image;
	QImage _copyImg;

	//we have to remeber original size
	QSize _size;

public slots:
	void markPosition(QPoint point);
	void changeImage();
	void annotation();
	//void fillRect( QVector<QRect>& reg, const QColor color);
	void mousePressEvent(QMouseEvent * event);
	void fillRect(int x, int y, int x2, int y2, const QColor color);
	void insertText();
	void deleteText();
	void eraseText();
	void changeText();
	void addPlace(QRect r);
	void insertImage();
	void deleteImage();
signals:
	void HandleLink(int i);
	void ShowAnnotation(int i);
	void ChangeImageSignal(QPoint p);
	void DeleteAnnotationSignal(QPoint);
	void AnnotationSignal(QPoint);
	void DeleteImageSignal(QPoint);
	void InsertImageSignal(QPoint);
	void ChangeTextSignal();
	void EraseTextSignal();
	void DeleteTextSignal();
	void InsertTextSignal(QPoint point);
	void MouseClicked(QPoint p); //bolo na mna kliknute, robte s tym nieco!
	void highlightText(int, int);
	void MouseReleased(QPoint);
public:
	int deleteAnnotation(QPoint p);
	void unsetImg();
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent * event);
	void clearLabels();

};

#endif // PAGE_H
