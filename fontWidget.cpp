//TODO on close close everythign else
#include <QTableWidgetItem>
#include "fontWidget.h"
#include "typedefs.h"
#include "globalfunctions.h"
#include <QEvent>
#include <QToolTip>

using namespace pdfobjects;
using namespace boost;

//TODO doplnit a spravne vyrazy
std::string fontShapes[] ={"Fill","Stroke","Fill&Stroke","Invisible"};
void FontWidget::setChange()
{
	_change = true;
	show();
}
void FontWidget::sliderChanged(int value)
{
	QVariant v(value);
	QString message = "Value:";
	message.append(v.toString());
	QToolTip::showText(QCursor::pos(), message);
	//this->ui.rotation->setToolTip(message);
}
QString FontWidget::getText()
{
	return this->ui.text->toPlainText();
}
void FontWidget::setInsert()
{
	_change = false;
	show();
}
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
	for ( int i = 10; i<=20; i++)
	{
		QVariant v(i);
		ui.fontsize->addItem(v.toString(),v);
	}
	for (int i = 0;i<NumberOfShapes;i++)
	{
		QVariant v(i);
		ui.shape->addItem(QString(fontShapes[i].c_str()),v);
	}
	connect(ui.rotation, SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
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
void FontWidget::addFont(std::string name, std::string val)
{
	QVariant q(val.c_str()); 
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?
	_fonts.push_back(name);
}
FontWidget::~FontWidget() {}
void FontWidget::createBT()
{
	_q = PdfComp(new pdfobjects::UnknownCompositePdfOperator( "q", "Q"));
	//ak chcemem vytvarat cm, tu je na to vhodna doba
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
	int angle = ui.rotation->value();
	double cs1 = cos(toRadians(angle));
	double sn1 = sin(toRadians(angle));
	QVariant var = QVariant(cs1);
	float f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(f)));
	var = QVariant(sn1);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(sn1*-1);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(cs1);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(_pdfPosX);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(_pdfPosY);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(f)));
	return createOperator(op, posOperands);
}
void FontWidget::setText(QString s)
{
	ui.text->setText(s);
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
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorS->getR()/255)) );
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorS->getG()/255)) );
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorS->getB()/255)) );
		_BT->push_back( createOperator("RG", operands ), getLastOperator(_BT));
	}
	{
		PdfOperator::Operands operands;
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorN->getR()/255)) );
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorN->getG()/255)) );
		operands.push_back(shared_ptr<IProperty>(new CReal((float)this->ui.colorN->getB()/255)) );

		//non-stroking operands
		_BT->push_back( createOperator("rg", operands ), getLastOperator(_BT));
	}
	if (ui.lastFont->checkState()==Qt::Checked)
	{
		PdfOp p = emit(getLastFontSignal(libs::Point(_pdfPosX, _pdfPosY)));
		if (p==NULL)//TODO warning
			assert(false);
		_BT->push_back( p, getLastOperator(_BT));
	}
	else
	{
		QVariant v = this->ui.fontsize->itemData(this->ui.fontsize->currentIndex());
		std::string id = emit fontInPage(_fonts[this->ui.fonts->currentIndex()].getName());
		assert(!id.empty());
		_fonts[this->ui.fonts->currentIndex()].setId(id);
		_BT->push_back( _fonts[this->ui.fonts->currentIndex()].getFontOper(v.toInt()), getLastOperator(_BT));
	}
	_BT->push_back( createMatrix("Tm"), getLastOperator(_BT));
}
PdfOp FontWidget::createTranslationTd(double x, double y)
{
	PdfOperator::Operands ops;
	ops.push_back(boost::shared_ptr<IProperty>(new CReal(x)));
	ops.push_back(boost::shared_ptr<IProperty>(new CReal(y)));
	return createOperator("Td",ops);
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
	if (_change)
	{
		emit changeTextSignal();
		return;
	}
	QString s = this->ui.text->toPlainText();
	std::string txt(s.toAscii().data());
	emit text(addText(txt));
	this->close();
}
void FontWidget::setPosition(float pdfx, float pdfy)
{
	_pdfPosX = pdfx; 
	_pdfPosY = pdfy;
	ui.rotation->setValue(0);
}

void FontWidget::createFromMulti( std::vector<PdfOp>& operators )
{
	createBT();
	addParameters();
	for (int i =0; i< operators.size(); i++)
		_BT->push_back(operators[i],getLastOperator(_BT));
	createET();
	emit text(_q);
}
