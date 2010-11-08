#include "page.h"

Page::Page(QWidget *parent)
	: QScrollArea(parent)
{
	ui.setupUi(this);
}

Page::~Page()
{

}
