#include <iostream>
#include <QImage>
#include <QColor>
#include <QKeyEvent>
#include <QMenu>
#include <QIcon>
#include <QShortcut>
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
	aboutDialogUI.setupUi(&aboutDialog);

	_search = new Search(NULL);//neptraia k tomuto mimiokienku
	_searchShortCut = new QShortcut(QKeySequence(tr("Ctrl+F", "Find texts")),this);

	QObject::connect(_search, SIGNAL(search(QString,bool)),this->ui.openedPdfs,SLOT(search(QString,bool)));
	//QObject::connect(_search, SIGNAL(replaceTextSignal(QString,QString)),this,SLOT(replaceText(QString,QString)));
	connect(_searchShortCut, SIGNAL(activated()), _search, SLOT(show()));

//TODO dat do uicka
	connect(this->ui.aboutButton, SIGNAL(pressed()), &aboutDialog, SLOT(open()) );
	connect( this->ui.openButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(openAnotherPdf()));
	connect( this->ui.saveButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(save()));
	connect( this->ui.saveAsButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(saveAs()));
	connect( this->ui.saveEncodedButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(saveEncoded()));
	connect( this->ui.viewButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeView()));
	connect( this->ui.exportButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(getText()));

	connect( this->ui.analyzeButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(initAnalyze()));
	connect( this->ui.lastOpenedButton, SIGNAL(pressed()), this, SLOT(lastOpenedPdfs()));
	connect( this->ui.opSelect, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(setModeOperator()));
	connect( this->ui.rotateButton, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(rotate()));
	connect( this->ui.derotateButton, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(derotate()));
	connect( this->ui.deleteButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(deleteSelectedText()));
	connect( this->ui.changeButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(changeSelectedText()));
	connect( this->ui.eraseButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(eraseSelectedText()));
	connect( this->ui.highlightButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(highlightSelected()));
	connect( this->ui.search, SIGNAL(search(QString,bool)),this->ui.openedPdfs, SLOT(search(QString, bool)));
	connect( this->ui.selectImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeSelectImage()));
	connect( this->ui.deleteImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(deleteSelectedImage()));
	connect( this->ui.changeImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(changeSelectedImage()));
	connect( this->ui.insertTextButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeInsertText()));

	connect( this->ui.selectTextButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeSelectText()));
	connect( this->ui.imagePartButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeImagePart()));
	connect( this->ui.extractButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeExtractImage()));
	connect( this->ui.insertAnotation, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeInsertAnotation()));
	connect( this->ui.linkAnnotation, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeInsertLinkAnotation()));
	connect( this->ui.changeAnnotationButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeChangeAnnotation()) );
	connect( this->ui.delAnnotation,SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeDeleteAnnotation()));
	connect( this->ui.openedPdfs, SIGNAL(ModeChangedSignal(HelptextIcon)), this,SLOT(handleModeChange(HelptextIcon)));

	connect( this->ui.repeter, SIGNAL(pressed()), ui.openedPdfs, SLOT(redraw()));
	connect( this->ui.pageUp, SIGNAL(pressed()), ui.openedPdfs, SLOT(pageUp()));
	connect( this->ui.pageDown, SIGNAL(pressed()), ui.openedPdfs, SLOT(pageDown()));
	connect( this->ui.insertEmpty, SIGNAL(pressed()), ui.openedPdfs, SLOT(insertEmpty()));
	connect( this->ui.deletePage, SIGNAL(pressed()), ui.openedPdfs, SLOT(deletePage()));

	connect( this->ui.hcolor, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setHColor(QColor)));
	connect( this->ui.color, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setColor(QColor)));
	connect( this->ui.openedPdfs, SIGNAL(OpenSuccess(QString)), this, SLOT(appendToLast(QString)));
	connect( this->ui.highlightButton_2, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setHighlighCommentText()));
	this->ui.openedPdfs->setMode(ModeDoNothing);
	//connect( this->ui.highlightButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(highlightSelected()));
	//connect( this->ui.insertAnotation,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeInsertComment()));
	//load settings
	ui.color->setColor(QColor(255,0,0,50));
	ui.hcolor->setColor(QColor(0,255,0));
	_lastOpenedButtonMenu = new QMenu();

	FILE * f = fopen("config","r");
	if (!f)
		return;
	char buffer[256];
	int i=0;
	while (!feof(f))
	{
		//dostan riadok
		char c = getc(f);
		if ( c != '\n')
		{
			buffer[i] = c;
			i++;
			continue;
		}

		buffer[i] ='\0';
		char * s = strchr(buffer,'=');
		*s='\0';
		if (!stricmp(buffer,"hcolor"))
		{
			s++;//budu to vzdy len 3 hodnoty
			QVariant str(s);
			ui.hcolor->setColor((QRgb)str.toUInt());
		}
		if(!stricmp(buffer,"color"))
		{
			s++;//budu to vzdy len 3 hodnoty
			QVariant str(s);
			ui.color->setColor((QRgb)str.toUInt());
		}
		if(!stricmp(buffer,"file"))
		{
			s++;
			_lastOpenedButtonMenu->addAction(s,this,SLOT(openLastFile()));
		}
		i=0;

	}
	//--------------------
	fclose(f);
}
void pdfGui::closeEvent( QCloseEvent *event )
{
	_search->close();
	FILE * f = fopen("config","w");
	if (!f)
		return;
	//napis hcolor a color + 3 cislice wo forma te XX bez medzery za tym
	char buffer[256];
	sprintf(buffer,"color=%u\n",ui.openedPdfs->getColor().rgb());
	fwrite(buffer,sizeof(char),strlen(buffer),f);
	sprintf(buffer,"hcolor=%u\n",ui.hcolor->getColor().rgb());
	fwrite(buffer,sizeof(char),strlen(buffer),f);
	QList<QAction *> actions = _lastOpenedButtonMenu->actions();
	for (int i =0; i< actions.size(); i++)
	{
		sprintf(buffer,"file=%s\n",actions[i]->text().toAscii().data());
		fwrite(buffer,sizeof(char),strlen(buffer),f);
	}
	fclose(f);
}
pdfGui::~pdfGui()
{
	delete _lastOpenedButtonMenu;
	pdfedit_core_dev_destroy();
}

void pdfGui::lastOpenedPdfs()//pressed
{
	if (_lastOpenedButtonMenu->actions().size()==0)
		return;
	_lastOpenedButtonMenu->exec(QCursor::pos());
}

void pdfGui::openLastFile()
{
	QAction * action = (QAction *)sender();
	this->ui.openedPdfs->open(action->text());
}

void pdfGui::appendToLast( QString s )
{
// check if this is opened
	QList<QAction *> actions = _lastOpenedButtonMenu->actions();
	for ( int i =0; i < actions.size(); i++)
		if (actions[i]->text() == s)
			return;
	_lastOpenedButtonMenu->addAction(s,this,SLOT(openLastFile()));
#ifdef _DEBUG
	QString name = _lastOpenedButtonMenu->actions().back()->text();
	assert(name == s);
#endif // _DEBUG
}

void pdfGui::handleModeChange( HelptextIcon str )
{
	this->ui.Help->setText(str.helpText);
	this->ui.actualButton->setIcon(QIcon(QString(":images/")+str.icon));
}
