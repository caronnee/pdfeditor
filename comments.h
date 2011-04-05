#ifndef __COMMENTS___
#define __COMMENTS___

#include <QWidget>
#include <QRect>

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
public:
	Comments(float x, float y);
	
signals:
	//addAnnotation, where
	void annotation(Annot);

public slots:
	void setPoints(float * flts, int size);
	void setDestination(pdfobjects::IndiRef ref);
	void onChange(int index);
	void apply();
};

#endif
