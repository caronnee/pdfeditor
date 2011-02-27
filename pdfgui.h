#ifndef PDFGUI_H
#define PDFGUI_H

#include <QtGui/QMainWindow>
 
#include "openpdf.h"
#include "ui_pdfgui.h"
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

class pdfGui : public QMainWindow
{
	Q_OBJECT

	struct pdfedit_core_dev_init init;

public:
	pdfGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~pdfGui();

protected:

	//void keyPressEvent(QKeyEvent *event);

private:
	Ui::pdfGuiClass ui;

};

#endif // PDFGUI_H
