#pragma once
//xpdf, pdfedit -> ktovie ci to nema ist do cppka
#define NO_CMAP
#include "xpdf/GlobalParams.h"
#undef NO_CMAP
#include <kernel/pdfedit-core-dev.h>
#include <kernel/cpdf.h>
#include <kernel/cpage.h>
#include <splash/Splash.h>
#include <splash/SplashBitmap.h>	
#include <xpdf/SplashOutputDev.h>

//END of PDF
#include <qwidget.h>
#include <QString>
#include <QFileDialog>
#include "ui_showPage.h"


class TabPage :
	public QWidget
{
	Q_OBJECT

private:
	Ui::TabUI ui;
	QString _name; //name of the file to be opened
	SplashColorPtr m_image;
	pdfobjects::DisplayParams displayparams;	
	boost::shared_ptr<pdfobjects::CPdf> pdf;
	boost::shared_ptr<pdfobjects::CPage> page;
public:
	TabPage(QString name);
	~TabPage(void);

private:
	//private methods
	void addRevision( size_t i);
	void setFromSplash();

	void updatePageInfoBar();
	// gets file, name is name of dialog
	QString getFile(QFileDialog::FileMode flags = QFileDialog::AnyFile);

public:	
	void deletePage();
	void pageUp();
	void pageDown();
	void savePdf(char * name);

public slots:
	///Sets image to previous page
	void previousPage();
	///Sets image to next page
	void nextPage();

	///insert range
	void insertRange();
	
	/// Adds empty page
	void addEmptyPage();

	//prints pdf
	void print();
private slots:

	/// Inserts range of file from existing PDF
	void insertPageRangeFromExisting();

	/// Saves actual made changes to new revision
	/** Nothign else happens, no need to  
	 */
	void commitRevision();

	/// Saves revision-specific pdf to new pdf
	void exportRevision();

	/// Revert revision
	void revertRevision();

};
