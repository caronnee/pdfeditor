#include <iostream>
#include <QImage>
#include <QColor>
#include <QKeyEvent>

#include "pdfgui.h"
#include "typedefs.h"

pdfGui::pdfGui(QWidget *parent, Qt::WFlags flags) 
	: QMainWindow(parent, flags), init()
{
	init.fontDir = ".";
	int argc = 0;
	char ** argv;
	if ( 0 != pdfedit_core_dev_init(&argc, &argv, &init))
		throw "Unable to init PdfEdit library";

	
	GlobalParams::initGlobalParams(NULL)->setEnableT1lib("no");
	GlobalParams::initGlobalParams(NULL)->setEnableFreeType("yes");
	GlobalParams::initGlobalParams(NULL)->setErrQuiet(gFalse);
	GlobalParams::initGlobalParams(NULL)->setAntialias("yes");
	//GlobalParams::initGlobalParams(NULL)->setupBaseFonts("D:\\Work\\winPdfEdit\\winPdfEdit\\pdfedit-0.4.5\\projects\\output"); //? Where are my fonts?

	ui.setupUi(this);
	ui.contentFrame->hide();
	ui.textFrame->hide();
	ui.annotationFrame->hide();
	ui.imageFrame->hide();
//TODO dat do uicka
	connect( this->ui.opSelect, SIGNAL(clicked()), this->ui.openedPdfs,SLOT(setModeDefault()));
	connect( this->ui.derotateButton, SIGNAL(clicked()), this->ui.openedPdfs,SLOT(derotate()));
	connect( this->ui.deleteButton,SIGNAL(clicked()),this->ui.openedPdfs,SLOT(deleteSelectedText()));
	connect( this->ui.changeButton,SIGNAL(clicked()),this->ui.openedPdfs,SLOT(changeSelectedText()));
	connect( this->ui.eraseButton,SIGNAL(clicked()),this->ui.openedPdfs,SLOT(eraseSelectedText()));
	connect( this->ui.highlightButton, SIGNAL(clicked()), this->ui.openedPdfs, SLOT(highlightSelected()));
	connect( this->ui.search, SIGNAL(search(QString,bool)),this->ui.openedPdfs, SLOT(search(QString, bool)));
	connect( this->ui.selectImageButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(setModeSelectImage()));
	connect( this->ui.deleteImageButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(deleteSelectedImage()));
	connect( this->ui.changeImageButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(changeSelectedImage()));
	connect( this->ui.imagePartButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(setModeImagePart()));
	connect( this->ui.extractButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(setModeExtractImage()));
	connect( this->ui.insertAnotation, SIGNAL(clicked()), this->ui.openedPdfs, SLOT(setModeInsertAnotation()));
	connect( this->ui.delAnnotation,SIGNAL(clicked()), this->ui.openedPdfs, SLOT(setModeDeleteAnnotation()));
	//connect( this->ui.highlightButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(highlightSelected()));
	//connect( this->ui.insertAnotation,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(setModeInsertComment()));
}
pdfGui::~pdfGui()
{
	pdfedit_core_dev_destroy();
}
