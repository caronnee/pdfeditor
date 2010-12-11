#include "fontWidget.h"

FontWidget::FontWidget() : QWidget(this)
{
	ui.setupUi(this);
	//set 10-40 fontsize
	//
}

FontWidget::FontWidget(const FontWidget & font) : QWidget(NULL)
{
	for ( int i = 10; i<= 40; i+=2)
	{
		QVariant q(i);
		this->ui.fontsize->insertItem(0, q.toString(),q);
	}
	//copy 
}
void FontWidget::addFont(std::string name)
{
	QVariant q(name.c_str());
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
}
FontWidget::~FontWidget() { }
