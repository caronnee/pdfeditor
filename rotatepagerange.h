#ifndef ROTATEPAGERANGE_H
#define ROTATEPAGERANGE_H

#include <QWidget>
#include "ui_rotatepagerange.h"

class rotatePageRange : public QWidget
{
	Q_OBJECT

public:
	rotatePageRange(QWidget *parent = 0);
	~rotatePageRange();

private:
	Ui::rotatePageRangeClass ui;
};

#endif // ROTATEPAGERANGE_H
