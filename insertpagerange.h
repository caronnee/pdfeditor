#ifndef INSERTPAGERANGE_H
#define INSERTPAGERANGE_H

#define NO_CMAP
#include "xpdf/GlobalParams.h"
#undef NO_CMAP
#include <kernel/pdfedit-core-dev.h>
#include <kernel/cpdf.h>
#include <kernel/cpage.h>
#include <splash/Splash.h>
#include <splash/SplashBitmap.h>	
#include <xpdf/SplashOutputDev.h>

#include <QDialog>
#include <QString>
#include "ui_insertpagerange.h"

/** \brief this class is not used */
class InsertPageRange : public QDialog
{
	Q_OBJECT

public:
	InsertPageRange(boost::shared_ptr<pdfobjects::CPdf> pdf, size_t pos = 0, QWidget *parent = 0, QString name = NULL);
	~InsertPageRange();

	boost::shared_ptr<pdfobjects::CPdf> pdf;
	boost::shared_ptr<pdfobjects::CPdf> reference;
	size_t _pos;

private:
	Ui::InsertPageRangeClass ui;

public:
	int getBegin()const;
	int getEnd()const;

public slots:
	void setPreviewToPage(int i);
	void insertPageRange();
};

#endif // INSERTPAGERANGE_H
