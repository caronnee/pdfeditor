#ifndef ___COLORPICK__
#define ___COLORPICK__

#include <QWidget>
#include <QPainter>
#include <QColor>
#include "ui_colorPicker.h"
#include <vector>

//TODO CMYK a podobne veci?
class ColorPicker : public QWidget
{
	Q_OBJECT
		
private:
	Ui::colorPick ui;	 //tri spinboxy + 1 na vykreslovanie farby
public:
	ColorPicker(QWidget * parent);
	int getR(); 
	int getG(); 
	int getB(); 
signals:
	void ValueChangedSignal(QColor);
	public slots:
		//void update()
		//{
		//	QColor color(getR(),getG(),getB());
		//	//ui.preview->set
		//	QPalette pal(QColor(255,255,255));
		//	pal.setColor(QPalette::Background, color);
		//	ui.preview->setAutoFillBackground(true);
		//	ui.preview->setPalette(pal);
		//	emit ValueChangedSignal(color);
		//}
		void setColor(QColor color);
		QColor getColor();
		void valueChanged(int);
};
#endif
