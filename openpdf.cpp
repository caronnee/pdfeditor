#include "openpdf.h"
#include "TabPage.h"
#include <QFileDialog>
#include "insertpagerange.h"

OpenPdf::OpenPdf(QWidget * centralWidget) :QTabWidget(centralWidget)
{
	TabPage * page = new TabPage(this,"./zadani.pdf");
	this->addTab(page,"test");
}
void OpenPdf::search(QString s, bool v)
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->search(s,v);
}
void OpenPdf::changeSelectedImage()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->raiseChangeSelectedImage();
}
void OpenPdf::eraseSelectedText()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->eraseSelectedText();
	page->createList();
}
void OpenPdf::highlightSelected()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->highLightAnnSelected();
}

void OpenPdf::changeSelectedText()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->raiseChangeSelectedText();
}
void OpenPdf::deleteSelectedImage()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->deleteSelectedImage();
}
void OpenPdf::deleteSelectedText()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->deleteSelectedText();
}
void OpenPdf::setModeInsertImage()
{
	setMode(ModeInsertImage);
}
void OpenPdf::setModeSelectImage()
{
	setMode(ModeSelectImage);
}
void OpenPdf::setModeInsertText()
{
	setMode(ModeInsertText);
}
void OpenPdf::setModeSelectText()
{
	setMode(ModeSelectText);
}
OpenPdf::~OpenPdf(void){}

void OpenPdf::getText()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->exportText();
}
void OpenPdf::rotate()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->rotate();
}
void OpenPdf::closeAndRemoveTab(int i)
{
	TabPage * page = (TabPage *)this->widget(i); //how to get exact tab?
	//this->focusNextChild();
	this->removeTab(i);
	delete page;
	///But ist should! be deleted
}
void OpenPdf::saveEncoded()
{
	TabPage * page = (TabPage *)this->widget(this->currentIndex());
	page->saveEncoded();
}
void OpenPdf::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),".", tr("PDF files (*.pdf)"));
	if (fileName == "")
		return;
	TabPage * page = (TabPage *)this->widget(this->currentIndex());
	page->savePdf(fileName.toAscii().data());
}

void OpenPdf::open(QString name)
{
	TabPage * page = new TabPage(this, name);
	this->addTab(page,name);
}
void OpenPdf::openAnotherPdf()
{
	QFileDialog d(this);
	d.setFilter("All PDF files (*.pdf)");
	if (!d.exec())
		return;
	QStringList fileNames = d.selectedFiles();
	for ( int i =0; i < fileNames.size(); i++)
		open(fileNames[i]);
	setCurrentIndex(count() -1);
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
void OpenPdf::insertRange()
{
	//calls insert range specific to active tab
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->insertRange();
}
void OpenPdf::insertEmpty()
{
	//calls insert range specific to active tab
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->addEmptyPage();
}
void OpenPdf::print()
{
	//calls insert range specific to active tab
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->print();
}
void OpenPdf::deletePage()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->deletePage();
}
