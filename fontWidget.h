#ifndef __FONTE_WIDGET__
#define __FONTE_WIDGET__

#include <QWidget>
#include <vector>


//PDF
#include <kernel/pdfedit-core-dev.h>
#include <kernel/factories.h>
#include <kernel/pdfoperators.h>
#include "typedefs.h"
//misc
//kvoli xpdf, kde je to definovane...pch!:)
#undef fontItalic 
#include "ui_properties.h"

typedef boost::shared_ptr<pdfobjects::UnknownCompositePdfOperator> PdfComp;

class TextFont
{
	std::string fontId;
public:
	TextFont(std::string id) : fontId(id) {}
	//vrati pdfoperator TF, s nastavenim fontu a velkosti
	boost::shared_ptr<pdfobjects::PdfOperator> getFontOper(int size)
	{	
		pdfobjects::PdfOperator::Operands fontOperands;//TODO check poradie
		fontOperands.push_back(boost::shared_ptr<pdfobjects::IProperty>(new pdfobjects::CName (fontId)) );
		fontOperands.push_back(boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(size)));//velkost pismeno
		return createOperator("Tf", fontOperands);
	}
};

class FontWidget : public QWidget
{
	Q_OBJECT

	enum Options
	{
		OptionTm,
		OptionShape,
		OptionColor,
		OptionGray,
		OptionFont,
		NumberOfOptions
	};
	enum Shapes
	{
		a,s,d,f,g,h,NumberOfShapes
	};
	bool set[NumberOfOptions];
	Ui::Properties ui;
	std::vector<TextFont> _fonts;
	PdfComp _q;
	PdfComp _BT;
public:
	void reset();
	FontWidget(QWidget * parent);
	FontWidget(const FontWidget & font);
	~FontWidget();
	PdfOp addText(std::string s);
	void createBT();
	void addParameters();
	void addToBT(PdfOp op);
	PdfOp createET();

	// set
	void addFont(std::string name, std::string value);
	//gets
	int getRotation();
	int getScale();
	QColor getColor();
	int getX();
	int getY();
	void change();
signals:
	void text(PdfOp op);
	void changeSelected();

public slots:

	void setTm() { set[OptionTm] = true; }
	void setFont() { set[OptionFont] = true; }
	void setShape() { set[OptionShape] = true; }
	void setGray() { set[OptionGray] = true; } //TODO macro

	void apply(); //on clicked
	void setValue(int angle);
};

#endif
