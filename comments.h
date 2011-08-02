#ifndef __COMMENTS___
#define __COMMENTS___

#include <QWidget>
#include <QRect>
#include <QColor>
#include <vector>
#include "ui_comments.h"
//PDF
#include "typedefs.h"
#include "kernel/cannotation.h"
#include "kernel/cdict.h"
#include "kernel/carray.h"

class Comments : public  QWidget
{
	Q_OBJECT

	typedef boost::shared_ptr<pdfobjects::CDict> ADictionary;

	boost::shared_ptr<pdfobjects::CArray> points;
	Ui::Comments ui;
	boost::shared_ptr<pdfobjects::CArray> arr;
	PdfAnnot _an;
	libs::Rectangle rect;
	typedef boost::shared_ptr<pdfobjects::utils::IAnnotInitializator> CAInit;
	struct InitName
	{
		std::string name;
		CAInit init;
		InitName(std::string n,CAInit i):name(n),init(i){};
	};
	std::vector<InitName> _inits;
	int _index;
	bool _change;
	std::string _name;
public:
	Comments(std::string name6);
signals:
	void textAnnotation (PdfAnnot);
	void annotationTextMarkup(PdfAnnot);
	void annotation(PdfAnnot);
	void WaitForPosition(PdfAnnot);
protected:
	void showEvent ( QShowEvent * event );

public slots:
	void setIndex(int i);
	void setRectangle(libs::Rectangle rectangle);
	void setRectangle(float pdfx, float pdfy, int pdfwidth, int pdfheight);
	void setPoints(std::vector<float> flts);
	void setDestination(pdfobjects::IndiRef ref);
	void onChange(int index);
	void apply();
	void insertMarkup();
	void addLink( PdfAnnot an, pdfobjects::IndiRef ref, float x, float y );
	void fromDict( boost::shared_ptr<pdfobjects::CDict> annDict );
	void loadAnnotation( PdfAnnot _annots );
	QColor getHColor();
	void setHColor( QColor colo );
};

#endif
