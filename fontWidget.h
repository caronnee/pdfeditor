#ifndef __FONTE_WIDGET__
#define __FONTE_WIDGET__

#include "ui_properties.h"
#include <QWidget>
#include <QString>
#include <vector>
//PDF
#include <kernel/pdfedit-core-dev.h>
#include <kernel/factories.h>
#include <kernel/pdfoperators.h>
#include "typedefs.h"
//misc

class TextFont
{
	std::string _name;
	std::string _fontId;
public:
	TextFont(std::string name) : _name(_name),_fontId("") {}
	//vrati pdfoperator TF, s nastavenim fontu a velkosti
	boost::shared_ptr<pdfobjects::PdfOperator> getFontOper(int size)
	{
		assert(!_fontId.empty());
		pdfobjects::PdfOperator::Operands fontOperands;//TODO check poradie
		fontOperands.push_back(boost::shared_ptr<pdfobjects::IProperty>(new pdfobjects::CName (_fontId)) );
		fontOperands.push_back(boost::shared_ptr<pdfobjects::IProperty>(pdfobjects::CRealFactory::getInstance(size)));//velkost pismeno
		return createOperator("Tf", fontOperands);
	}
	std::string getName() const{ return _name; }
	void setId(std::string id) { _fontId = id; } 
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
	bool _change;
	bool set[NumberOfOptions];
	Ui::Properties ui;
	std::vector<TextFont> _fonts;
	PdfComp _q;
	PdfComp _BT;
	float _pdfPosX,_pdfPosY;
	PdfOp createMatrix(std::string op);
public:
	void setHeight(float h)
	{
		QVariant data(h);
		data=data.toInt();
		this->ui.fontsize->setCurrentIndex(this->ui.fontsize->findData(data.toInt()));
	}
	QString getText();
	static PdfOp createTranslationTd(double x, double y);
	void setText(QString s);
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
	void changeTextSignal();
	void text(PdfOp op);
	void changeSelected();
	PdfOp getLastFontSignal(libs::Point);
	std::string fontInPage(std::string id);

public slots:
	void setInsert();
	void setChange();
	void setPosition(float pdfx, float pdfy);
	void setTm() { set[OptionTm] = true; }
	void setFont() { set[OptionFont] = true; }
	void setShape() { set[OptionShape] = true; }
	void setGray() { set[OptionGray] = true; } //TODO macro

	void apply(); //on clicked
	void createFromMulti( std::vector<PdfOp>& operators );
	void sliderChanged(int value);
	//void setAngle(int angle);
};

#endif
