#include  <QImage>
#include <QColor>
#include <QKeyEvent>
#include <iostream>
#include "pdfgui.h"

pdfGui::pdfGui(QWidget *parent, Qt::WFlags flags) 
	: QMainWindow(parent, flags), init()
{
	init.fontDir = ".";
	int argc = 0;
	char ** argv;
	if ( 0 != pdfedit_core_dev_init(&argc, &argv, &init))
		throw "Not going to happen..pliiis";

	
	GlobalParams::initGlobalParams(NULL)->setEnableT1lib("no");
	GlobalParams::initGlobalParams(NULL)->setEnableFreeType("yes");
	GlobalParams::initGlobalParams(NULL)->setErrQuiet(gFalse);
	GlobalParams::initGlobalParams(NULL)->setAntialias("yes");
	GlobalParams::initGlobalParams(NULL)->setupBaseFonts("D:\\Work\\winPdfEdit\\winPdfEdit\\pdfedit-0.4.5\\projects\\output"); //? Where are my fonts?

	ui.setupUi(this);
	//pridame inicializocanie kniznice
	// pdf lib init & work

	//connections - no connections are made because i shoul connect and disconnect frequently to match actually choosed label
	
}

pdfGui::~pdfGui()
{
	pdfedit_core_dev_destroy();
}
