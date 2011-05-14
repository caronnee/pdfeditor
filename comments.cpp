#include "comments.h"
#include "kernel/factories.h"
#include "globalfunctions.h"

void Comments::setRectangle(float x, float y, int width, int height)
{
	//konvert do nasich suradnic
	rect.yleft = y; //big line
	rect.xleft = x; //big line
	rect.yright = y+width; //big line
	rect.xright = x+height; //big line
	pdfobjects::CArray arr;
	arr.addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(rect.xleft)));
	arr.addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(rect.yleft)));
	arr.addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(rect.xright)));
	arr.addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(rect.yright)));
	_an->getDictionary()->setProperty("Rect",arr);
}
void Comments::setRectangle(libs::Rectangle rectangle)
{
	rect = rectangle;
}
Comments::Comments()
{
	typedef boost::shared_ptr<pdfobjects::utils::IAnnotInitializator> CAInit;
	pdfobjects::CAnnotation::setAnnotInitializator(CAInit(new pdfobjects::utils::TextAnnotInitializer()));
	ui.setupUi(this);
	//rect sa potom este upravi podla to, ci je to link a ako velky je
	{
		char b[] = { 'N','I','O','P', '\0' };
		for ( int i = 0; b[i]!= '\0'; i++)
		{
			ui.annotLinkStyle->addItem(QString(b[i]));
		}
	}
	this->ui.annotType->addItem(QString("Comment"),QVariant("Text"));
	this->ui.annotType->addItem(QString("Link"),QVariant("Link"));
	this->ui.annotType->addItem(QString("Highlight"),QVariant("Highlight"));
	this->ui.annotType->addItem(QString("Strikeout"),QVariant("StrikeOut"));
}
void Comments::setPoints(std::vector<float> flts)
{
	for ( int i = 0; i< flts.size(); i++)
		points->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(flts[i])));
}
void Comments::onChange(int index)
{
	//prida ten typ na urcity rect
	QString strong = ui.annotType->itemData(index).toString();
	_an = pdfobjects::CAnnotation::createAnnotation(rect, "Text");
	//cela annotacia sa bude diat v page
}
void Comments::apply()
{
	//ak je to highlight, potom parsni
	//zisti, ci je to text
	ADictionary d = _an->getDictionary();
	switch (ui.annotType->currentIndex())
	{
	case AText: //text, obycajny
		{//mame spravne KDE ma byt, zostava zistit obsah
			d->setProperty("Contents", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CStringFactory::getInstance(std::string(ui.text->toPlainText().toAscii().data()))));
			d->addProperty("T",*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CStringFactory::getInstance("Muf!")));
			d->addProperty("Subj", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CStringFactory::getInstance("pdftest")));
			emit (annotation (_an));
			return;
		} 
	case ALink: //co ked je to URL? - linkova anotacia, vyriesit v texte. Tot sa odkazuje oba ramci textu
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
			emit parseToRows(rect); //potom to budeme mat setle, TODO netreba cakat?
			d->addProperty("Subtype", *boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string(ui.annotType->currentText().toAscii().data()))));

			d->addProperty("QuadPoints",*boost::shared_ptr<pdfobjects::IProperty>(points));
			break;
		}
	default:
		throw "unimplemented";
	}
	emit (annotationTextMarkup (_an));
}
void Comments::setDestination(pdfobjects::IndiRef ref)
{
	arr = boost::shared_ptr<pdfobjects::CArray>(new pdfobjects::CArray());
	//cislo spranky,resp refereci na bject
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRefFactory::getInstance(ref)));
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CNameFactory::getInstance(std::string("Fit"))));
	arr->addProperty(*boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CIntFactory::getInstance(0)));
}//TODO pre zaciatok, potom bude aj na konkretnu cast dokumentu
