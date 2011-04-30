//TODO on close close everythign else
#include <QTableWidgetItem>
#include "fontWidget.h"

using namespace pdfobjects;
using namespace boost;

typedef shared_ptr< PdfOperator > PdfOp;

//TODO doplnit a spravne vyrazy
std::string fontShapes[] ={"Fill","Stroke","Fill&Stroke","Invisible"};

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
void FontWidget::addFont(std::string name, std::string val)//TODO
{
	QVariant q(val.c_str()); //TODO convert to more understable font name
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
	_fonts.push_back(name);
}
FontWidget::~FontWidget() {}
void FontWidget::createBT()
{
	_q = PdfComp(new pdfobjects::UnknownCompositePdfOperator( "q", "Q"));
	//ak chcemem vytvarat cm, tu je na to vhodna doba
	if (this->ui.generateCm->isChecked())
		_q->push_back( createMatrix("cm"), getLastOperator(_q));
	//TODO farba  podobne prkotiny
	_BT = PdfComp(new pdfobjects::UnknownCompositePdfOperator( "BT", "ET"));
}
PdfOp FontWidget::createET()
{
	PdfOperator::Operands emptyOperands;
	_BT->push_back(createOperator("ET", emptyOperands), getLastOperator(_BT));
	_q->push_back(_BT, getLastOperator(_q));
	_q->push_back(createOperator("Q", emptyOperands), getLastOperator(_q));
	return _q;
}
PdfOp FontWidget::createMatrix(std::string op)
{
	//text matrix 
	PdfOperator::Operands posOperands;
	QVariant var = ui.tm->item(0,0)->data(0);
	float f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(f)));
	var = ui.tm->item(0,1)->data(0);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(1,0)->data(0);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(1,1)->data(0);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(2,0)->data(0);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.tm->item(2,1)->data(0);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(f)));
	return createOperator(op, posOperands);
}
void FontWidget::addParameters() //TODO nie s jedine parametre
{
	///rendering mode
	{
		PdfOperator::Operands operands;
		operands.push_back(shared_ptr<IProperty>(new CInt(ui.shape->currentIndex())));
		_BT->push_back( createOperator("Tr", operands ), getLastOperator(_BT));
	}
	//nastal cas na farby a podobne uchylnosti, este predtym, ako pojdm td
	//RGB
	{
		//stroking operands
		PdfOperator::Operands operands;
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorS->getR())) );
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorS->getG())) );
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorS->getB())) );
		_BT->push_back( createOperator("RG", operands ), getLastOperator(_BT));
	}
	{
		PdfOperator::Operands operands;
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorN->getR())) );
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorN->getG())) );
		operands.push_back(shared_ptr<IProperty>(new CReal(this->ui.colorN->getB())) );

		//non-stroking operands
		_BT->push_back( createOperator("rg", operands ), getLastOperator(_BT));
	}

	QVariant v = this->ui.fontsize->itemData(this->ui.fontsize->currentIndex());
	_BT->push_back( _fonts[this->ui.fonts->currentIndex()].getFontOper(v.toInt()), getLastOperator(_BT));

	//if (this->ui.generateCm->isChecked())
	//	return;
	_BT->push_back( createMatrix("Tm"), getLastOperator(_BT));
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
	textOperands.push_back(shared_ptr<IProperty>(new CString(txt.c_str())));
	addToBT(createOperator("Tj", textOperands));
	return createET();
}

void FontWidget::apply()
{	
	QString s = this->ui.text->toPlainText();
	std::string txt(s.toAscii().data());
	emit text(addText(txt));
}
void FontWidget::setPosition(float pdfx, float pdfy)
{
	_pdfPosX = pdfx; 
	_pdfPosY = pdfy;
	setAngle(0);
}
void FontWidget::setAngle(int angle)
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

	QVariant x(_pdfPosX);
	QVariant y(_pdfPosY);
	ui.tm->setItem(2,0,new QTableWidgetItem(x.toString()));
	ui.tm->setItem(2,1,new QTableWidgetItem(y.toString()));
}
