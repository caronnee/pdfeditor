#include "page.h"

DisplayPage::DisplayPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

DisplayPage::~DisplayPage()
{

}
void DisplayPage::setImage( const QImage & image )const
{
	this->ui.label->setPixmap(QPixmap::fromImage(image));
	this->ui.label->adjustSize();
}