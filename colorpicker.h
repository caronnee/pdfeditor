#ifndef ___COLORPICK__
#define ___COLORPICK__

#include <QWidget>
#include "ui_colorPicker.h"

//TODO CMYK a podobne veci?
class ColorPicker : public QWidget
{
	Q_OBJECT

private:
	Ui::colorPick ui;	 //tri spinboxy + 1 na vykreslovanie farby
public:
	ColorPicker(QWidget * parent) : QWidget(parent) { }
	int getR() { return ui.r->value(); }
	int getG() { return ui.g->value(); }
	int getB() { return ui.b->value(); }
};
#endif
