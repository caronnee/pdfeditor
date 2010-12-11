#ifndef __FONTE_WIDGET__
#define __FONTE_WIDGET__

#include <QWidget>
#include "ui_properties.h"

class FontWidget : QWidget
{
	Q_OBJECT

	Ui::Properties ui;
public:
	FontWidget();
	FontWidget(const FontWidget & font);
	~FontWidget();
	// set
	void addFont(std::string name);
	//gets
	int getRotation();
	int getScale();
	QColor getColor();
	int getX();
	int getY();
};
#endif
