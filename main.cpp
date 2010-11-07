#include "pdfgui.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	pdfGui w;
	w.show();
	bool ret = a.exec();
	return ret;
}
