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
	ui.debugFrame->hide();
	ui.settingFrame->hide();
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
	connect( this->ui.linkAnnotation, SIGNAL(clicked()), this->ui.openedPdfs, SLOT(setModeInsertLinkAnotation()));
	connect( this->ui.delAnnotation,SIGNAL(clicked()), this->ui.openedPdfs, SLOT(setModeDeleteAnnotation()));
	connect( this->ui.openedPdfs, SIGNAL(ModeChangedSignal(QString)), this->ui.Help,SLOT(setText(QString)));
	connect( this->ui.repeter, SIGNAL(clicked()), ui.openedPdfs, SLOT(redraw()));
	connect( this->ui.pageUp, SIGNAL(clicked()), ui.openedPdfs, SLOT(pageUp()));
	connect( this->ui.pageDown, SIGNAL(clicked()), ui.openedPdfs, SLOT(pageDown()));

	connect( this->ui.hcolor, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setHColor(QColor)));
	connect( this->ui.color, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setColor(QColor)));
	
	this->ui.openedPdfs->setMode(ModeSelectText);
	//connect( this->ui.highlightButton,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(highlightSelected()));
	//connect( this->ui.insertAnotation,SIGNAL(clicked()),this->ui.openedPdfs, SLOT(setModeInsertComment()));
	//load settings
	ui.color->setColor(QColor(255,0,0,50));
	ui.hcolor->setColor(QColor(0,255,0));
	FILE * f = fopen("config","r");
	if (!f)
		return;
	char buffer[256];
	int i=0;
	while (!feof(f))
	{
		//dostan riadok
		char c = getc(f);
		if ( c == '\n')
		{
			buffer[i] ='\0';
			char * s = strchr(buffer,'=');
			*s='\0';
			if (!stricmp(buffer,"hcolor"))
			{
				s++;//budu to vzdy len 3 hodnoty
				QVariant str(s);
				ui.hcolor->setColor((QRgb)str.toUInt());
				i=0;
				continue;
			}
			if(!stricmp(buffer,"color"))
			{
				s++;//budu to vzdy len 3 hodnoty
				QVariant str(s);
				ui.color->setColor((QRgb)str.toUInt());
				i=0;
				continue;
			}
		}
		buffer[i] = c;
		i++;
	}
	fclose(f);
}
void pdfGui::closeEvent( QCloseEvent *event )
{
	FILE * f = fopen("config","w");
	if (!f)
		return;
	//napis hcolor a color + 3 cislice wo forma te XX bez medzery za tym
	char buffer[256];
	sprintf(buffer,"color=%u\n",ui.openedPdfs->getColor().rgb());
	fwrite(buffer,sizeof(char),strlen(buffer),f);
	sprintf(buffer,"hcolor=%u\n",ui.hcolor->getColor().rgb());
	fwrite(buffer,sizeof(char),strlen(buffer),f);
	fclose(f);
	//savni to do configu
}
pdfGui::~pdfGui()
{
	pdfedit_core_dev_destroy();
}
