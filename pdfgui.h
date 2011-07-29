#ifndef PDFGUI_H
#define PDFGUI_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>
#include <QDialog>
#include "Search.h"

#include "openpdf.h"
#include "ui_pdfgui.h"
//xpdf, pdfedit -> ktovie ci to nema ist do cppka

#include <kernel/pdfedit-core-dev.h>
#include <kernel/cpdf.h>
#include <kernel/cpage.h>
#include <splash/Splash.h>
#include <splash/SplashBitmap.h>	
#include <xpdf/SplashOutputDev.h>
#define NO_CMAP
#include "xpdf/GlobalParams.h"
#undef NO_CMAP

#include "ui_aboutDialog.h"
#include "ui_debugFrame.h"
#include "ui_textFrame.h"
#include "ui_imageFrame.h"
#include "ui_annotationFrame.h"

class pdfGui : public QMainWindow
{
	Q_OBJECT

	struct pdfedit_core_dev_init init;

	QShortcut * _searchShortCut;
	QDialog aboutDialog;
	Ui::AboutDialog aboutDialogUI;

	QWidget _debugWidget;
	Ui::DebugFrame _debugFrame;

	QWidget _annotationFrame;
	Ui::AnnotationFrame _annotationFrameUI;

	QWidget _imageFrame;
	Ui::ImageFrame _imageFrameUI;

	QWidget _textFrame;
	Ui::TextFrame _textFrameUI;
public:
	pdfGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~pdfGui();
	void closeEvent( QCloseEvent *event );

private:
	Search * _search;
	QMenu * _lastOpenedButtonMenu;
	Ui::pdfGuiClass ui;
public slots:
	void openLastFile( );
	void lastOpenedPdfs();
	void appendToLast( QString s );
	void handleModeChange( HelptextIcon s );
	void showFullScreened();
};

#endif // PDFGUI_H
