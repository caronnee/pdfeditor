//TODO on close close evrythign else
#include "fontWidget.h"
#include <QTableWidgetItem>

using namespace pdfobjects;
using namespace boost;

typedef shared_ptr< PdfOperator > PdfOp;

//TODO doplnit
std::string fontShapes[] ={"a","s","d","f","g","h"};

void FontWidget::change()
{
	emit changeSelected();
}
void FontWidget::reset() //nastavit rotaciu na nulu a podobne
{
	memset(set,0x0,sizeof(bool)*NumberOfOptions);
}
FontWidget::FontWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	reset();
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
	throw "Not now";
}
void FontWidget::addFont(std::string name, std::string val)//TOFO
{
	QVariant q(name.c_str()); //TODO convert to more understable font name
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
	_fonts.push_back(val);
}
FontWidget::~FontWidget() { }
void FontWidget::createBT()
{
	_q = PdfComp(new pdfobjects::UnknownCompositePdfOperator( "q", "Q"));
	_BT = PdfComp(new pdfobjects::UnknownCompositePdfOperator( "BT", "ET"));
}
PdfOp FontWidget::createET()
{
	PdfOperator::Operands emptyOperands;
	_BT->push_back(createOperator("ET", emptyOperands), getLastOperator(_BT));
	_q->push_back(createOperator("Q", emptyOperands), getLastOperator(_q));
	return _q;
}
void FontWidget::addParameters() //TODO nie s jedine parametre
{
	QVariant v = this->ui.fontsize->itemData(this->ui.fontsize->currentIndex());
	_BT->push_back( _fonts[this->ui.fonts->currentIndex()].getFontOper(v.toInt()), getLastOperator(_BT));

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
	_BT->push_back(createOperator("Tm", posOperands), getLastOperator(_BT));
}

void FontWidget::addToBT(PdfOp o)
{
	_BT->push_back(o,getLastOperator(_BT));
}

PdfOp FontWidget::addText(std::string txt)
{
	createBT();
	addParameters();
	PdfOperator::Operands textOperands;
	textOperands.push_back(shared_ptr<IProperty>(new CString (txt.c_str())));
	addToBT(createOperator("Tj", textOperands));
	return createET();
}

void FontWidget::apply()
{	
	QString s =this->ui.text->toPlainText();
	std::string txt(s.toAscii().data());
	emit text(addText(txt));
}
void FontWidget::setValue(int angle)
{
	//set cosie, sine
	double cs = cos(static_cast<double>(angle));
	QVariant cs1(cs);
	double sn = sin(static_cast<double>(angle));
	QVariant sn1(sn);
	QVariant sn2(sn*-1);
	ui.tm->setItem(0,0,new QTableWidgetItem(cs1.toString()));
	ui.tm->setItem(0,1,new QTableWidgetItem(sn1.toString()));
	ui.tm->setItem(1,0,new QTableWidgetItem(sn1.toString()));
	ui.tm->setItem(1,1,new QTableWidgetItem(cs1.toString()));
}
