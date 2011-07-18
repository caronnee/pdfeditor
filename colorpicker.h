#ifndef ___COLORPICK__
#define ___COLORPICK__

#include <QWidget>
#include <QPainter>
#include <QColor>
#include "ui_colorPicker.h"

//TODO CMYK a podobne veci?
class ColorPicker : public QWidget
{
	Q_OBJECT

private:
	Ui::colorPick ui;	 //tri spinboxy + 1 na vykreslovanie farby
public:
	ColorPicker(QWidget * parent) : QWidget(parent) { this->ui.setupUi(this);update(); }
	int getR() { return ui.r->value(); }
	int getG() { return ui.g->value(); }
	int getB() { return ui.b->value(); }
signals:
	void ValueChangedSignal(QColor);
	public slots:
		void update()
		{
			QColor color(getR(),getG(),getB());
			//ui.preview->set
			QPalette pal(QColor(255,255,255));
			pal.setColor(QPalette::Background, color);
			ui.preview->setAutoFillBackground(true);
			ui.preview->setPalette(pal);
			emit ValueChangedSignal(color);
		}
		void setColor(QColor color)
		{
			ui.r->setValue(color.red());
			ui.g->setValue(color.green());
			ui.b->setValue(color.blue());
		}
		QColor getColor()
		{
			return QColor (getR(),getG(),getB(),50);
		}
};
#endif
