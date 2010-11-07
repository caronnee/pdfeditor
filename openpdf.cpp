#include "openpdf.h"
#include "TabPage.h"
#include <QFileDialog>

OpenPdf::OpenPdf(QWidget * centralWidget) :QTabWidget(centralWidget)
{
}

OpenPdf::~OpenPdf(void)
{
}

void OpenPdf::closeAndRemoveTab(int i)
{
	TabPage * page = (TabPage *)this->widget(i); //how to get exact tab?
	//this->focusNextChild();
	this->removeTab(i);
	delete page;
	///But ist should! be deleted
}
void OpenPdf::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),".", tr("PDF files (*.pdf)"));
	TabPage * page = (TabPage *)this->widget(this->currentIndex());
	page->savePdf(fileName.toAscii().data());
}
void OpenPdf::openAnotherPdf()
{
	QFileDialog d(this);
	d.setFilter("All PDF files (*.pdf)");
	if (!d.exec())
		return;
	QStringList fileNames = d.selectedFiles();
	for ( int i =0; i < fileNames.size(); i++)
	{
		TabPage * page = new TabPage(fileNames[i]);
		this->addTab(page,fileNames[i]);
	}
	setCurrentIndex(count() -1);

	/*pdf = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance ("D:\\Work\\winPdfEdit\\winPdfEdit\\pdfedit-0.4.5\\projects\\output\\test.pdf", pdfobjects::CPdf::ReadWrite));

	page = boost::shared_ptr<pdfobjects::CPage> (pdf->getPage(1));

	//page->addText("myText...XXXX",libs::Point(1,1),"Helvetica");
	//pdf->save();
	// init splash bitmap
	setFromSplash();*/
}
void OpenPdf::save()
{
	TabPage * t = (TabPage *)this->widget(this->currentIndex());
	t->savePdf(NULL);
}

void OpenPdf::pageUp()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->pageUp();
}
void OpenPdf::pageDown()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->pageDown();
}
