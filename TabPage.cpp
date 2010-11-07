#include "TabPage.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>

TabPage::TabPage(QString name) : _name(name)
{
	ui.setupUi(this);

	//connections()
	connect (ui.previous,SIGNAL(clicked()),this,SLOT(previousPage()));
	connect (ui.next,SIGNAL(clicked()),this,SLOT(nextPage()));
	//connect (ui.commit, SIGNAL(clicked()), this, SLOT(createRevision()));
//	connect (ui.branchRevision, SIGNAL(clicked()), this, SLOT(branchRevision()));

	//end of connections

	pdf = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance (name.toAscii().data(), pdfobjects::CPdf::ReadWrite));

	page = boost::shared_ptr<pdfobjects::CPage> (pdf->getPage(1)); //or set from last
	//page->addText("myText...XXXX",libs::Point(1,1),"Helvetica");
	//pdf->save();
	// init splash bitmap
	QStringList list;
	std::stringstream ss;
	std::string s;
	for ( size_t i =0; i< pdf->getRevisionsCount(); i++)
	{
		ss << i;
		ss >> s;
		list.append(("Revision " + s).c_str());
	}
	ui.Revision->addItems(list);
	ui.Revision->setCurrentIndex(list.count()-1);
	setFromSplash();
}
void TabPage::updatePageInfoBar()
{
	std::stringstream ss;
	ss << page->getPagePosition();
	std::string s1;
	ss >> s1;
	ss << pdf->getPageCount();
	std::string s2;
	ss >> s2;
	//,f1,10) + "/" + itoa(pdf->getPageCount(),f2,10);
	this->ui.pageInfo->setText( (s1 + " / " +s2).c_str() );
}
void TabPage::pageUp()
{
	int pos = pdf->getPagePosition(page);
	this->pdf->removePage(pos);
	this->pdf->insertPage(page,pos-2);
	updatePageInfoBar();
}
void TabPage::pageDown()
{
	int pos = pdf->getPagePosition(page);
	this->pdf->removePage(pos);
	this->pdf->insertPage(page,pos);
	updatePageInfoBar();
}
void TabPage::previousPage()
{
	if (pdf->getPagePosition(page) == 1)
		return;
	page = pdf->getPrevPage(page);
	this->setFromSplash();
}
void TabPage::nextPage()
{
	if (pdf->getPagePosition(page) == pdf->getPageCount())
		return;
	page = pdf->getNextPage(page);
	this->setFromSplash();
}

void TabPage::insertPageFromExisting()
{ 
	QString s = getFile(QFileDialog::ExistingFile);
	if (s == NULL)
		return;
	//set rotation, set rotation for pages, insertpages
	boost::shared_ptr<pdfobjects::CPdf> pdf2 = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance (s.toAscii().data(), pdfobjects::CPdf::ReadOnly));
//	RangeDialog
	page = pdf->insertPage(pdf2->getPage(1),1);
}
void TabPage::deletePage()
{
	//removes actual page and displays the one after
	if (pdf->getPageCount() <= 1)
		return;
	size_t i = page->getPagePosition();
	pdf->removePage(i);
	if ( i == pdf->getPageCount() ) //if removing last page..
		i-=2;
	i++;
	page = pdf->getPage(i);
	setFromSplash();
}
void TabPage::setFromSplash()
{
	SplashColor paperColor;
	paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
	SplashOutputDev splash (splashModeBGR8, 4, gFalse, paperColor);

	// display it = create internal splash bitmap
	page->displayPage (splash, displayparams);
	splash.clearModRegion();

	QImage image(splash.getBitmap()->getWidth(), splash.getBitmap()->getHeight(),QImage::Format_RGB32);
	Guchar * p = new Guchar[3];
	for ( int i =0; i< image.width(); i++)
	{
		for ( int j =0; j < image.height(); j++)
		{
			splash.getBitmap()->getPixel(i,j,p);
			image.setPixel(i,j, qRgb(p[0],p[1],p[2]));
		}
	}
	delete[] p;

	this->ui.label->setPixmap(QPixmap::fromImage(image));
	//image.save("mytest.bmp","BMP");
	this->ui.label->adjustSize();
	updatePageInfoBar();
}
void TabPage::savePdf(char * name)
{
	if (name == NULL)
	{
		pdf->save();
		return;
	}
	FILE * f;
	f = fopen(name,"w");
	pdf->clone(f);
	fclose(f);
}
TabPage::~TabPage(void)	{}

///---private--------
void TabPage::addRevision( size_t i )
{
	std::stringstream ss;
	assert( i < pdf->getRevisionsCount() );
	ss << i;
	std::string s;
	ss >> s;
	ui.Revision->addItem(QString(("Revision " + s).c_str()));
}

///--------------------------------PRIVATE SLOTS----------------


void TabPage::commitRevision()
{
	//save revision to pdf
	this->pdf->save(true);
	//update revicionscout
	addRevision(this->ui.Revision->count());	
}

void TabPage::exportRevision()
{
	QString name = getFile();
	//if exists, save it here
	FILE * f = fopen(name.toAscii().data(),"r");
	if (f) //we are sure we caanot create file
	{
		if ( QMessageBox::warning(this, "File exists","You are sure to overwrite?", QMessageBox::Ok | QMessageBox::Discard,QMessageBox::Discard) == QMessageBox::Discard)
		{
			fclose(f);
			return;
		}
		fclose(f);
	}
	//saving to new file
	f = fopen(name.toAscii().data(),"w");
	if (!f)
	{
		QMessageBox::warning(this,"Cannot save","Unknown error", QMessageBox::Ok);
		return;
	}
	pdf->clone(f);
	fclose(f);
}

QString TabPage::getFile(QFileDialog::FileMode flags)
{
	QFileDialog d(this);
	d.setFileMode(flags);
	d.setFilter("All PDF files (*.pdf)");
	if (!d.exec())
		return NULL;
	return d.selectedFiles()[0] ;
}
void TabPage::revertRevision()
{
	//save to file that is tmpdte
	FILE * f;
	std::string s(_name.toAscii().data());
	s+= ".tmp";
	f = fopen(s.c_str(), "w");
	if (!f)
	{
		QMessageBox::warning(this, tr("Not able to revert"),tr("There was problem to open file for writing"), QMessageBox::Ok);
	}
	pdf->clone(f);
	fclose(f);
	rename(s.c_str(), _name.toAscii().data());
	pdf = boost::shared_ptr<pdfobjects::CPdf> ( pdfobjects::CPdf::getInstance (_name.toAscii().data(), pdfobjects::CPdf::ReadWrite));
	//TODO skontrolovat, ci sa zmenia zamky
}
