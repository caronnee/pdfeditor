#include "comments.h"
#include "kernel\factories.h"
#include "globalfunctions.h"

Comments::Comments(float x, float y)
{
	ui.setupUi(this);
	rect.yleft = y; //big line
	rect.xleft = x; //big line
	rect.yright = y+50; //big line
	rect.xright = x+50; //big line
}
void Comments::onChange(int index)
{
	//prida ten typ na urcity rect
	_an =  pdfobjects::CAnnotation::createAnnotation(rect, ui.annotType->currentText().toAscii().data());
	//cela annotacia sa bude diat v page
}
void Comments::apply()
{
	//zisti, ci je to text
	switch (ui.annotType->currentIndex())
	{
	case AText: //text, obycajny
		{
			ADictionary d = _an->getDictionary();
			d->addProperty("Contents", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string(ui.text->toPlainText().toAscii().data()))));
			break;
		} //TODO dokoncit
	default:
		throw "unimplemented";
	}
	emit (annotation (_an));
}
