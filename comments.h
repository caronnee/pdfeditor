#ifndef __COMMENTS___
#define __COMMENTS___

#include <QWidget>
#include <QRect>

#include <vector>
#include "ui_comments.h"
//PDF
#include "kernel/cannotation.h"
#include "kernel/cdict.h"
#include "kernel/carray.h"

typedef boost::shared_ptr<pdfobjects::CAnnotation> Annot;

class Comments : public  QWidget
{
	Q_OBJECT

	typedef boost::shared_ptr<pdfobjects::CDict> ADictionary;

	boost::shared_ptr<pdfobjects::CArray> points;
	Ui::Comments ui;
	boost::shared_ptr<pdfobjects::CArray> arr;
	Annot _an;
	libs::Rectangle rect;
	typedef boost::shared_ptr<pdfobjects::utils::IAnnotInitializator> CAInit;
	std::vector<CAInit> _inits;
public:
	Comments();
signals:
	void annotationTextMarkup(Annot);
	void annotation(Annot);
	void WaitForPosition();

public slots:
	void setRectangle(libs::Rectangle rectangle);
	void setRectangle(float pdfx, float pdfy, int pdfwidth, int pdfheight);
	void setPoints(std::vector<float> flts);
	void setDestination(pdfobjects::IndiRef ref);
	void onChange(int index);
	void apply();
	void insertMarkup();
	void addLink( Annot an, pdfobjects::IndiRef ref, float x, float y );
	void fromDict( boost::shared_ptr<pdfobjects::CDict> annDict );
};

#endif
