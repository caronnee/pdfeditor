#include "fontWidget.h"

using namespace pdfobjects;
using namespace boost;

typedef shared_ptr< PdfOperator > PdfOp;

FontWidget::FontWidget(QWidget *parent) : QWidget(parent)
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
void FontWidget::addFont(std::string name, std::string val)
{
	QVariant q(name.c_str());
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
	_fonts.push_back(val);
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
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(1)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(1)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	BT->push_back(createOperator("Tm", posOperands), getLastOperator(BT));

	PdfOperator::Operands textOperands;
//	QString s =this->ui.text->toPlainText();
//	std::string text(s.toAscii().data());
	textOperands.push_back(shared_ptr<IProperty>(new CString ("aargh:)")));
	BT->push_back(createOperator("Tj", textOperands), getLastOperator(BT));
	PdfOperator::Operands emptyOperands;
	BT->push_back(createOperator("ET", emptyOperands), getLastOperator(BT));
	q->push_back(createOperator("Q", emptyOperands), getLastOperator(q));
	emit text(q);
}
