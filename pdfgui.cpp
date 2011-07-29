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
	ui.fileFrame->hide();
	_textFrameUI.setupUi(&_textFrame);
	_textFrame.hide();
	
	_annotationFrameUI.setupUi(&_annotationFrame);
	_annotationFrame.hide();

	_imageFrameUI.setupUi(&_imageFrame);
	_imageFrame.hide();

	_debugFrame.setupUi(&_debugWidget);

	ui.settingFrame->hide();
	aboutDialogUI.setupUi(&aboutDialog);

	_search = new Search(NULL);//neptraia k tomuto mimiokienku
	_searchShortCut = new QShortcut(QKeySequence(tr("Ctrl+F", "Find texts")),this);

	connect(_search, SIGNAL(search(QString,bool)),this->ui.openedPdfs,SLOT(search(QString,bool)));
	//QObject::connect(_search, SIGNAL(replaceTextSignal(QString,QString)),this,SLOT(replaceText(QString,QString)));
	connect(_searchShortCut, SIGNAL(activated()), _search, SLOT(show()));

//////////////////////////////////MENU////////////////////////////////////////

	connect( this->ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect( this->ui.actionFullScreen, SIGNAL(triggered()), this, SLOT(showFullScreened()) );
	connect( this->ui.actionOpen, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(openAnotherPdf()));
	connect( this->ui.actionOpen, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(show()));

	connect( this->ui.actionAbout, SIGNAL(triggered()), &aboutDialog, SLOT(open()));
	connect( this->ui.actionPage_down, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(pageDown()));
	connect( this->ui.actionPage_up, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(pageUp()));
	connect( this->ui.actionRotate_left, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(rotate()));
	connect( this->ui.actionRotate_right, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(derotate()));
	connect( this->ui.actionDelete_Page, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(deletePage()));
	connect( this->ui.actionAnalyze, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(initAnalyze()));
	connect( this->ui.actionAdd_page, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(insertEmpty()));
	connect( this->ui.actionExport_text, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(getText()));
	connect( this->ui.actionSave, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(save()));
	connect( this->ui.actionSaveCopy, SIGNAL(triggered()), this->ui.openedPdfs, SLOT(saveAs()));

//////////////////////////////////////////////////////////////////////////

	connect( this->ui.debugButton, SIGNAL(pressed()), &_debugWidget, SLOT(show()));
	connect( _textFrameUI.searchButton, SIGNAL(pressed()), _search, SLOT(show()));
//	connect( this->ui.openButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(openAnotherPdf()));
//	connect( this->ui.saveButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(save()));
//	connect( this->ui.saveAsButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(saveAs()));
	connect( _debugFrame.saveEncodedButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(saveEncoded()));
	connect( this->ui.viewButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeView()));
//	connect( this->ui.exportButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(getText()));
	connect( _imageFrameUI.insertImageButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeInsertImage()));
	connect( _debugFrame.analyzeButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(initAnalyze()));
//	connect( this->ui.lastOpenedButton, SIGNAL(pressed()), this, SLOT(lastOpenedPdfs()));
	connect( _debugFrame.opSelect, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(setModeOperator()));
	//connect( this->ui.rotateButton, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(rotate()));
	//connect( this->ui.derotateButton, SIGNAL(pressed()), this->ui.openedPdfs,SLOT(derotate()));
	connect( _textFrameUI.insertTextButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeInsertText()));
	connect( _textFrameUI.deleteButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(deleteSelectedText()));
	connect( _textFrameUI.changeButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(changeSelectedText()));
	connect( _textFrameUI.eraseButton,SIGNAL(pressed()),this->ui.openedPdfs,SLOT(eraseSelectedText()));
	connect( _textFrameUI.highlightButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(highlightSelected()));
	connect( _textFrameUI.selectTextButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeSelectText()));
	
	connect( _search, SIGNAL(search(QString,bool)),this->ui.openedPdfs, SLOT(search(QString, bool)));
	connect( _imageFrameUI.selectImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeSelectImage()));
	connect( _imageFrameUI.deleteImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(deleteSelectedImage()));
	connect( _imageFrameUI.changeImageButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(changeSelectedImage()));
	connect( _imageFrameUI.imagePartButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeImagePart()));
	connect( _imageFrameUI.extractButton,SIGNAL(pressed()),this->ui.openedPdfs, SLOT(setModeExtractImage()));
	
	connect( _annotationFrameUI.insertAnotation, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeInsertAnotation()));
	connect( _annotationFrameUI.linkAnnotation, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeInsertLinkAnotation()));
	connect( _annotationFrameUI.changeAnnotationButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeChangeAnnotation()) );
	connect( _annotationFrameUI.delAnnotation,SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setModeDeleteAnnotation()));
	connect( this->ui.openedPdfs, SIGNAL(ModeChangedSignal(HelptextIcon)), this,SLOT(handleModeChange(HelptextIcon)));
	connect( _annotationFrameUI.highlightButton, SIGNAL(pressed()), this->ui.openedPdfs, SLOT(setHighlighCommentText()));

	connect( _debugFrame.refreshButton, SIGNAL(pressed()), ui.openedPdfs, SLOT(redraw()));
//	connect( this->ui.pageUp, SIGNAL(pressed()), ui.openedPdfs, SLOT(pageUp()));
//	connect( this->ui.pageDown, SIGNAL(pressed()), ui.openedPdfs, SLOT(pageDown()));
//	connect( this->ui.insertEmpty, SIGNAL(pressed()), ui.openedPdfs, SLOT(insertEmpty()));
//	connect( this->ui.deletePage, SIGNAL(pressed()), ui.openedPdfs, SLOT(deletePage()));

	connect( this->ui.hcolor, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setHColor(QColor)));
	connect( this->ui.color, SIGNAL(ValueChangedSignal(QColor)), ui.openedPdfs, SLOT(setColor(QColor)));
	connect( this->ui.openedPdfs, SIGNAL(OpenSuccess(QString)), this, SLOT(appendToLast(QString)));
	
	this->ui.openedPdfs->setMode(ModeDoNothing);
	//load settings
	ui.color->setColor(QColor(255,0,0,50));
	ui.hcolor->setColor(QColor(0,255,0));
	_lastOpenedButtonMenu = this->ui.menuOpen_recent;

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
#ifdef _DEBUG
	ui.fileFrame->show();
#endif // _DEBUG
}
void pdfGui::showFullScreened()
{
	if (isFullScreen())
		showNormal();
	else
		showFullScreen();
}
void pdfGui::closeEvent( QCloseEvent *event )
{
	_search->close();
	_debugWidget.close();

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
		if(actions[i]->text().isEmpty())
			continue; //hidden "nothing"?
		sprintf(buffer,"file=%s\n",actions[i]->text().toAscii().data());
		fwrite(buffer,sizeof(char),strlen(buffer),f);
	}
	fclose(f);
}
pdfGui::~pdfGui()
{
	delete _search;
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
