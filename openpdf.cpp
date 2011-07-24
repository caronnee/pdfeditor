#include "openpdf.h"
#include "TabPage.h"
#include <QFileDialog>
#include <QTabWidget>
#include "insertpagerange.h"
#include "kernel/exceptions.h"
#include <QMessageBox>

void OpenPdf::initAnalyze()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->initAnalyze();
}
void OpenPdf::pdfChanged()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->_changed = true;
	this->setTabText( currentIndex(), QString("*") + this->tabText(currentIndex() )); //TODO zmenit na coot?
}
void OpenPdf::setModeChangeAnnotation()
{
	setMode(ModeChangeAnntation);
}
void OpenPdf::setModeDeleteAnnotation()
{
	setMode(ModeDeleteAnnotation);
}
QColor OpenPdf::getHColor()
{
	return _highlightColor;
}
OpenPdf::OpenPdf(QWidget * centralWidget) :QTabWidget(centralWidget),_mode(ModeDoNothing),_previous(ModeDoNothing), _color(255,0,0,50), _highlightColor(0,255,0),_author("Unknown")
{
#if _DEBUG
	TabPage * page = new TabPage(this,"./zadani.pdf");
	this->addTab(page,"test");
#endif
	setModeDefault(); //default mode
}
void OpenPdf::search(QString s, bool v)
{
	if (s.isEmpty())
	{
		QMessageBox::warning(this, "Empty string","No string to search", QMessageBox::Ok,QMessageBox::Ok); 
		return;
	}
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
	page->redraw();
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
void OpenPdf::setModeInsertLinkAnotation()
{
	setMode(ModeInsertLinkAnnotation);
	setMode(ModeInsertLinkAnnotation);
}
void OpenPdf::setModeInsertAnotation()
{
	setMode(ModeInsertAnnotation);
	setMode(ModeInsertAnnotation);
}
void OpenPdf::setModeExtractImage()
{
	setMode(ModeExtractImage);
	setMode(ModeExtractImage);
}
void OpenPdf::setModeImagePart()
{
	setMode(ModeImagePart);
	setMode(ModeImagePart);
}
void OpenPdf::setModeInsertImage()
{
	setMode(ModeInsertImage);
	setMode(ModeInsertImage);
}
void OpenPdf::setModeDefault()
{
	setMode(ModeDoNothing);
	setMode(ModeDoNothing);
}
void OpenPdf::setModeSelectImage()
{
	setMode(ModeSelectImage);
}
void OpenPdf::setModeInsertText()
{
	setMode(ModeInsertText);
	setMode(ModeInsertText);
}
void OpenPdf::setModeSelectText()
{
	setMode(ModeSelectText);
	setMode(ModeSelectText); //prepisanie previous
}
OpenPdf::~OpenPdf(void){}

void OpenPdf::getText()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->exportText();
}
void OpenPdf::derotate()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->rotate(-90);
}
void OpenPdf::rotate()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->rotate(90);
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
	try
	{
		TabPage * page = (TabPage *)this->widget(this->currentIndex());
		page->saveEncoded();
	}
	catch (...)
	{
		QMessageBox::warning(this, "Unable to save",QString("Unable to save to decoded file"), QMessageBox::Ok, QMessageBox::Ok);
	}
}
void OpenPdf::saveAs()
{
	try
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),".", tr("PDF files (*.pdf)"));
		if (fileName == "")
			return;
		TabPage * page = (TabPage *)this->widget(this->currentIndex());
		page->savePdf(fileName.toAscii().data());
	}
	catch (...)
	{
		QMessageBox::warning(this, "Error occured",QString("Unable to save file"), QMessageBox::Ok, QMessageBox::Ok);
	}
}

#include "kernel/delinearizator.h"

void OpenPdf::open(QString name)
{
	try
	{
		TabPage * page = new TabPage(this, name);
		this->addTab(page,name);
		emit OpenSuccess(name);
		setCurrentIndex(count() -1);
		//pri kazdon otvoreni sa spytaj, ci je to potreba delinerizovat(co noveho suboru), inak sa nebude dat savovat
		if (page->checkLinearization())
			return;
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save pdf file"),".",tr("PdfFiles (*.pdf)"));
		if (fileName == "")
			return;
		page->delinearize(fileName);
		
		//close the old file & open delinearized version
		this->deletePage();
		page = new TabPage(this,fileName);
		this->addTab(page,fileName);
		assert(page->checkLinearization());
		setCurrentIndex(count() -1);
	}
	catch (PdfOpenException e)
	{
		QMessageBox::warning(this, "Pdf library unable to perform action",QString("Reason") + QString(e.what()), QMessageBox::Ok, QMessageBox::Ok);

	}
	catch (PdfException e)
	{
		QMessageBox::warning(this, "Pdf library unable to perform action",QString("Reason") + QString(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	//	return;
	}
	catch (PermissionException)
	{
		QMessageBox::warning(this, "Encrypted document",QString("Unable to open file ( it is ecnrypted") , QMessageBox::Ok, QMessageBox::Ok);
	}
	catch (std::exception e)
	{
		QMessageBox::warning(this, "Unexpected exception",QString("Reason") + QString(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	}
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

}
void OpenPdf::save()
{
	TabPage * t = (TabPage *)this->widget(this->currentIndex());
	if (!t->CanBeSavedChanges())
		return;
	t->savePdf(NULL);
	this->setTabText(currentIndex(), this->tabText(currentIndex()).mid(1));
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
void OpenPdf::redraw()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->redraw();
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
QColor OpenPdf::getColor()
{
	return _color;
}
void OpenPdf::deletePage()
{
	TabPage * page = (TabPage *)this->widget(currentIndex());
	page->deletePage();
}
static const char * helper[] = { MODES(EARRAY) };

bool PermanentMode(Mode mode)
{
	return mode == ModeChangeAnntation ||
		mode == ModeDeleteAnnotation ||
		mode == ModeDoNothing ||
		mode == ModeExtractImage ||
		mode == ModeImagePart ||
		mode == ModeImagePartCopied ||
		mode == ModeImageSelected ||
		mode == ModeInsertAnnotation ||
		mode == ModeInsertLinkAnnotation ||
		mode == ModeInsertText ||
		mode == ModeSelectText ||
		mode == ModeSelectImage ||
		mode == ModeOperatorSelect ;
}
void OpenPdf::setMode( Mode mode )
{
	_previous = _mode;
	if (PermanentMode(mode))
		_previous = mode;
	_mode =  mode;
	emit ModeChangedSignal(helper[mode]);
}

void OpenPdf::setPreviousMode()
{
	setMode(_previous);
}

void OpenPdf::setColor( QColor color )
{
	_color = color;
	_color.setAlpha(50);
}

void OpenPdf::setHColor( QColor color)
{
	_highlightColor =color;
}

void OpenPdf::setHighlighCommentText()
{
	setMode(ModeHighlighComment);
}

std::string OpenPdf::Author()
{
	return _author;
}
