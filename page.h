#ifndef DISPLAY_PAGE_H
#define DISPLAY_PAGE_H

#include <QScrollArea>
#include <QWheelEvent>
#include "ui_page.h"

class DisplayPage : public QWidget
{
	Q_OBJECT

public:
	DisplayPage(QWidget *parent = 0);
	~DisplayPage();
	void DisplayPage::setImage( const QImage & image)const;

private:
	Ui::page ui;

};

#endif // PAGE_H
