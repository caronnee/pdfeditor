//TODO on close close everythign else
#include "kernel/cpage.h"
#include <QTableWidgetItem>
#include "fontWidget.h"
#include "typedefs.h"
#include "globalfunctions.h"
#include <QEvent>
#include <QToolTip>
#include <QPixmap>
#include <QBitmap>

using namespace pdfobjects;
using namespace boost;

#define FINDFONT_INDEX 0

//TODO doplnit a spravne vyrazy
QString fontShapes[] ={"Fill","Stroke","Fill&Stroke","Invisible"};

void FontWidget::setChange()
{
	_scale[0] = _scale[1] = 1; //TODO dat do initu, vsade tm, kde sa ukazuje show
	_change = true;
	show();
	this->ui.fonts->setCurrentIndex(1);
	this->ui.fonts->setCurrentIndex(0);
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
	return this->ui.text->text();
}
void FontWidget::setInsert()
{
	_scale[0] = _scale[1] = 1;
	_change = false;
	show();
	this->ui.fonts->setCurrentIndex(1);
	this->ui.fonts->setCurrentIndex(0);
}
void FontWidget::change()
{
	emit changeSelected();
}
void FontWidget::reset() //nastavit rotaciu na nulu a podobne
{
	memset(set,0x0,sizeof(bool)*NumberOfOptions);
}
void FontWidget::paintEvent( QPaintEvent * event )
{
	//QVector<QPoint> points;
	//points.append(QPoint(0.0, 0.0)); //inverted
	//points.append(QPoint(width(), 0));
	//points.append(QPoint(width(), height()));
	//points.append(QPoint(0.0, height()));
	//points.append(QPoint(0.0, 0.0));

	//points.append(QPoint(ui.propFrame->x(), ui.propFrame->y()+10));
	//points.append(QPoint(ui.propFrame->x(), ui.propFrame->y()+ui.propFrame->height()));
	//points.append(QPoint(ui.propFrame->x()+ ui.propFrame->width(), ui.propFrame->y()+ui.propFrame->height()));
	//points.append(QPoint(ui.propFrame->x()+ ui.propFrame->width(), ui.propFrame->y()));
	//points.append(QPoint(ui.propFrame->x()+10, ui.propFrame->y()));

	//QPixmap image(this->size());
	//QPainter painter(&image);
	//QPainterPath path;
	//path.addPolygon(QPolygon(points));
	//path.closeSubpath();
	//painter.fillPath(path,QColor(0,0,0));
	//painter.end();
	//image.setAlphaChannel(image);
	//setFixedSize(image.size());
	//setMask(image.mask());//TODO nie v repainte

}
FontWidget::FontWidget(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint),_embededFont(false)
{
	_scale[0] = _scale[1] = 1;
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
		ui.shape->addItem(fontShapes[i],v);
	}
	//pridaj posledny polozku na zistovanie fontu z pdf operatora
	this->ui.fonts->addItem("<Select font from text>");
	connect(ui.rotation, SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
	connect(ui.fonts, SIGNAL(currentIndexChanged(int)), this, SLOT(fontIndexChanged(int)));
}

FontWidget::FontWidget(const FontWidget & font) : QWidget(font.parentWidget()),_embededFont(false)
{
	for ( int i = 10; i<= 40; i+=2)
	{
		QVariant q(i);
		this->ui.fontsize->insertItem(0, q.toString(),q);
	}
	//copy TODO
	throw "Not now";
}
void FontWidget::addFont(PdfOp op)
{
	std::string val;
#if _DEBUG
	op->getOperatorName(val);
	assert(val=="Tf");
#endif
	PdfOperator::Operands operands;
	//get size
	op->getParameters(operands);
	std::string fontId = utils::getValueFromSimple<CName>(operands[0]);
	addFont(fontId,fontId);
	float size = utils::getValueFromSimple<CReal>(operands[1]);
	QVariant q(size);
	this->ui.fonts->setItemText(0, QString("Selected Text(") + fontId.c_str() + ")");
	this->ui.fontsize->setItemData(0,q); //custom, bude na vrchu
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
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(_scale[0]*var.value<float>())));
	var = QVariant(sn1*-1);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(cs1);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = QVariant(_pdfPosX);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(_scale[1]*var.value<float>())));
	var = QVariant(_pdfPosY);
	f = var.value<float>();
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(f)));
	return createOperator(op, posOperands);
}
void FontWidget::setText(QString s)
{
	ui.text->setText(s);
}
std::string FontWidget::addParameters() //TODO nie s jedine parametre
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
	std::string fontName="";
	PdfOp fontOp;
	if (ui.fonts->currentIndex() == FINDFONT_INDEX )//posledny je vyber z fontu
	{
		fontOp = emit(getLastFontSignal(libs::Point(_pdfPosX, _pdfPosY)));
		emit getLastTm(libs::Point(_pdfPosX, _pdfPosY),_scale);
#ifdef _DEBUG
		std::string m;
		fontOp->getStringRepresentation(m);
#endif // _DEBUG
	}
	else
	{
		QVariant v = this->ui.fontsize->itemData(this->ui.fontsize->currentIndex());
		std::string id = emit fontInPage(_fonts[this->ui.fonts->currentIndex()].getName());
		assert(!id.empty());
		_fonts[this->ui.fonts->currentIndex()].setId(id);
		fontOp = _fonts[this->ui.fonts->currentIndex()].getFontOper(v.toInt());
	}
	PdfOperator::Operands pars;
	fontOp->getParameters(pars); //font uz bude v PDF - otazne, co bude aj fontData -> osertit!
	fontName = utils::getValueFromSimple<CName>(pars[0]);
	_BT->push_back(fontOp, getLastOperator(_BT));
	_BT->push_back( createMatrix("Tm"), getLastOperator(_BT));
	return fontName;
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

PdfOp FontWidget::addText( QString s )
{
	createBT();
	std::string name = addParameters();
	PdfOperator::Operands textOperands;
	std::string e = emit convertTextFromUnicode(s,name);
	textOperands.push_back(shared_ptr<IProperty>(CStringFactory::getInstance(e)));
	addToBT(createOperator("Tj", textOperands));
	return createET();
}

void FontWidget::apply()
{	
	QString s = this->ui.text->text();
	if (_change)
		emit changeTextSignal(addText(s));
	else
	{
		_scale[0] = _scale[1] =1;
		emit text(addText(s));
	}
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


void FontWidget::clearTempFonts()
{
	ui.fonts->clear();
	QVariant q("Select font from operator"); 
	ui.fonts->insertItem(ui.fonts->count(),q.toString(),q); //Qvariant?

	CPage::FontList fontList;
	CPageFonts::SystemFontList flist = CPageFonts::getSystemFonts();
	for ( CPageFonts::SystemFontList::iterator i = flist.begin(); i != flist.end(); i++ )
	{
		addFont(*i,*i); //TODO zrusit a pridat do fontu, ktory si to moze naloadovat sam
	}
	//_fonts.push_back(name);
}

void FontWidget::fontIndexChanged( int index )
{
	if (!this->isVisible())
		return;
	if (index != 0 )
	{
		if (this->ui.fontsize->itemText(0).mid(0,strlen("Generic")) == "Generic")
			this->ui.fontsize->removeItem(0);
		return;
	}
	//add "Generic"
	QVariant q(10);
	this->ui.fontsize->insertItem(0, "Generic",q);
	this->ui.fontsize->setCurrentIndex(0);
	emit FindLastFontSignal();
}

void FontWidget::addTm( float w, float h )
{
	_scale[0] = w;
	_scale[1] = h;
}
