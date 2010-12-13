//TODO on close close evrythign else
#include "fontWidget.h"
#include <QTableWidgetItem>

using namespace pdfobjects;
using namespace boost;

typedef shared_ptr< PdfOperator > PdfOp;

enum FontShapes
{
	q,w,e,r,t,y,NumberOfShapes
};

std::string fontShapes[] ={"a","s","d","f","g","h"};

FontWidget::FontWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	//set 10-40 fontsize
	for ( int i = 10; i<=40; i+=2)
	{
		QVariant v(i);
		ui.fontsize->addItem(v.toString(),v);
	}
	for (int i = 0;i<NumberOfShapes;i++)
	{
		QVariant v(i);
		ui.shape->addItem(QString(fontShapes[i].c_str()),v);
	}
}

FontWidget::FontWidget(const FontWidget & font) : QWidget(font.parentWidget())
{
	for ( int i = 10; i<= 40; i+=2)
	{
		QVariant q(i);
		this->ui.fontsize->insertItem(0, q.toString(),q);
	}
	//copy TODO
}
void FontWidget::addFont(std::string name, std::string val)
{
	QVariant q(name.c_str());
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
	_fonts.push_back(name);
}
FontWidget::~FontWidget() { }
void FontWidget::apply()
{
	//vytvor BT, ET 
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));
	shared_ptr<UnknownCompositePdfOperator> BT(new UnknownCompositePdfOperator("BT", "ET"));

	q->push_back(BT,q);
	QVariant v = this->ui.fontsize->itemData(this->ui.fontsize->currentIndex());
	BT->push_back( _fonts[this->ui.fonts->currentIndex()].getFontOper(v.toInt()), getLastOperator(BT));
	//text matrix 

	PdfOperator::Operands posOperands;
	QVariant var = ui.tm->item(0,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(0,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(1,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(1,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(2,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(2,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	BT->push_back(createOperator("Tm", posOperands), getLastOperator(BT));

	PdfOperator::Operands textOperands;
	QString s =this->ui.text->toPlainText();
	std::string txt(s.toAscii().data());
	textOperands.push_back(shared_ptr<IProperty>(new CString (txt.c_str())));
	BT->push_back(createOperator("Tj", textOperands), getLastOperator(BT));
	PdfOperator::Operands emptyOperands;
	BT->push_back(createOperator("ET", emptyOperands), getLastOperator(BT));
	q->push_back(createOperator("Q", emptyOperands), getLastOperator(q));
	emit text(q);
}
void FontWidget::setValue(int angle)
{
	//set cosie, sine
	double cs = cos(angle);
	QVariant cs1(cs);
	double sn = sin(angle);
	QVariant sn1(sn);
	QVariant sn2(sn*-1);
	ui.tm->setItem(0,0,new QTableWidgetItem(cs1.toString()));
	ui.tm->setItem(0,1,new QTableWidgetItem(sn1.toString()));
	ui.tm->setItem(1,0,new QTableWidgetItem(sn1.toString()));
	ui.tm->setItem(1,1,new QTableWidgetItem(cs1.toString()));
}
