#ifndef __COMMENTS___
#define __COMMENTS___

#include <QWidget>
#include <QRect>

#include "ui_comments.h"
//PDF
#include "kernel/cannotation.h"
#include "kernel/cdict.h"

typedef boost::shared_ptr<pdfobjects::CAnnotation> Annot;

class Comments : public  QWidget
{
	Q_OBJECT

		typedef boost::shared_ptr<pdfobjects::CDict> ADictionary;

	Ui::Comments ui;
	Annot _an;
	libs::Rectangle rect;
public:
	Comments(float x, float y);
	
signals:
	//addAnnotation, where
	void annotation(Annot);

public slots:
	void onChange(int index);
	void apply();
};

#endif
