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
	//rect sa potom este upravi podla to, ci je to link a ako velky je
	{
		char b[] = { 'N','I','O','P', '\0' };
		for ( int i = 0; b[i]!= '\0'; i++)
		{
			ui.annotLinkStyle->addItem(QString(b[i]));
		}
	}
}
void Comments::setPoints(float * flts, int size)
{
	for ( int i = 0; i< size; i++)
		points->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(flts[i])));
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
	ADictionary d = _an->getDictionary();
	switch (ui.annotType->currentIndex())
	{
	case AText: //text, obycajny
		{//mame spravne KDE ma byt, zostava zistit obsah
			ADictionary d = _an->getDictionary();
			d->addProperty("Contents", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string(ui.text->toPlainText().toAscii().data()))));
			break;
		} 
	case ALink: //co ked je to URL? - linkova anotacia, vyriesit v texte. Tot sa odkazuje oba  raci textu
		{
			d->addProperty("Subtype", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance("Link")));
			//d->addProprty
			d->addProperty("H",*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string(ui.annotLinkStyle->currentText().toAscii().data()))));
			//optional -> border
			//kam to ma skocit, zatial neratam s actions
			pdfobjects::CArray arr;
			//page, fitR, fitH..., 3 cisla. Cislo stranky dostane z PDF dokumentu
			d->addProperty("Dest",arr); //array bude teraz platne, bolo nastavene pocas dialogu
			break;
		}
	case AHighlight:
	case AUnderline:
	case AStrike:
		{
			d->addProperty("Subtype", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string(ui.annotType->currentText().toAscii().data()))));

			d->addProperty("QuadPoints",*boost::shared_ptr<pdfobjects::IProperty>(points));
			break;
		}
	default:
		throw "unimplemented";
	}
	emit (annotation (_an));
}
void Comments::setDestination(pdfobjects::IndiRef ref)
{
	arr = boost::shared_ptr<pdfobjects::CArray>(new pdfobjects::CArray());
	//cislo spranky,resp refereci na bject
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRefFactory::getInstance(ref)));
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string("Fit"))));
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CIntFactory::getInstance(0)));
}