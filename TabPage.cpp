#include "TabPage.h"
//QT$
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QScrollBar>
//PDF
#include <kernel/factories.h>
//created files
#include "insertpagerange.h"

typedef std::vector<shared_ptr<PdfOperator> > Ops;

TabPage::TabPage(QString name) : _name(name)
{
	ui.setupUi(this);

	//connections()
	connect (ui.previous,SIGNAL(clicked()),this,SLOT(previousPage()));
	connect (ui.next,SIGNAL(clicked()),this,SLOT(nextPage()));
	connect (ui.content,SIGNAL(MouseChanged(int, int)),this, SLOT(showClicked(int, int))); //pri selecte sa to disconnectne a nahrasi inym modom


	//end of connections

	pdf = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance (name.toAscii().data(), pdfobjects::CPdf::ReadWrite));

	page = boost::shared_ptr<pdfobjects::CPage> (pdf->getPage(1)); //or set from last
	//page->addText("myText...XXXX",libs::Point(1,1),"Helvetica");
	//pdf->save();
	// init splash bitmap
	QStringList list;

	std::string s;
	for ( size_t i =0; i< pdf->getRevisionsCount(); i++)
	{
		std::stringstream ss;
		ss << i;
		ss >> s;
		list.append(("Revision " + s).c_str());
	}
	ui.Revision->addItems(list);
	ui.Revision->setCurrentIndex(list.count()-1);
	setFromSplash();
}
void TabPage::showClicked(int x, int y)
{
	double px, py;
	displayparams.convertPixmapPosToPdfPos(x, y, px, py);
	//find operattors
	std::vector< shared_ptr < PdfOperator> > ops;
	page->getObjectsAtPosition(ops, libs::Point(px,py));
	//vsetky tieto objekty vymalujeme
	for ( int i =0; i < ops.size(); i++)
	{
		QRect rect = getRectangle(ops[i]);
		QColor color(266, 255, 0, 50);
		this->ui.content->fillRect(rect, color );
	}
}
QRect TabPage::getRectangle(shared_ptr < PdfOperator> ops)
{
	QRect r;
	libs::Rectangle b = ops->getBBox();
	double x1,y1,x2,y2;
	double top_x = b.xleft;
	double top_y = max(b.yleft,b.yright);
	double bottom_x = b.xright;
	double bottom_y = min(b.yright,b.yleft);
	
	displayparams.convertPdfPosToPixmapPos(top_x,top_y,x1,y1);
	displayparams.convertPdfPosToPixmapPos(bottom_x, bottom_y, x2, y2);

	r.setBottom(y2);
	r.setLeft(x1);
	r.setRight(x2);
	r.setTop(y1);

	return r;
}
void TabPage::updatePageInfoBar()
{
	std::stringstream ss;
	ss << pdf->getPageCount();
	std::string s2;
	ss >> s2;
	ss.clear();
	ss << page->getPagePosition();
	std::string s1;
	ss >> s1;
	
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
bool TabPage::previousPage()
{
	if (pdf->getPagePosition(page) == 1)
		return false;
	page = pdf->getPrevPage(page);
	this->setFromSplash();
	return true;
}
bool TabPage::nextPage()
{
	if (pdf->getPagePosition(page) == pdf->getPageCount())
		return false;
	page = pdf->getNextPage(page);
	this->setFromSplash();
	return true;
}

void TabPage::insertPageRangeFromExisting()
{ 
	QString s = QFileDialog::getOpenFileName(this, tr("Open File"),".",tr("Pdf files (*.pdf)"));
	if (s == NULL)
		return;
	boost::shared_ptr<pdfobjects::CPdf> pdf2 = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance (s.toAscii().data(), pdfobjects::CPdf::ReadOnly));
	page = pdf->insertPage(pdf2->getPage(1),1);
}
void TabPage::deletePage()
{
	//removes actual page and displays the one after
	if (pdf->getPageCount() <= 1)
		return;
	size_t i = pdf->getPagePosition(page);
	pdf->removePage(i);
	if ( i > pdf->getPageCount() ) //if removing last page..
		i =pdf->getPageCount() ;
	page = pdf->getPage(i);
	setFromSplash();
}
void TabPage::setFromSplash()
{
	SplashColor paperColor;
	paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
	SplashOutputDev splash (splashModeBGR8, 4, gFalse, paperColor);

	// display it = create internal splash bitmap
	page->displayPage(splash, displayparams);
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

	this->ui.content->setImage(image);
	//image.save("mytest.bmp","BMP");
	//this->ui.label->adjustSize();
	updatePageInfoBar();
}
void TabPage::wheelEvent( QWheelEvent * event ) //non-continuous mode
{
	if (event->delta() > 0 )
	{
		//wheeleing forward
		QScrollBar * bar = this->ui.scrollArea->horizontalScrollBar();
		if (( bar->value() > event->delta()) && 
			(this->previousPage()))
			bar->setValue(bar->maximum());
		else
			bar->setValue(bar->value() + event->delta() );
	}
	else
	{
		QScrollBar * bar = this->ui.scrollArea->horizontalScrollBar();
		if ( bar->value() < event->delta()*-1 )
		{
			bar->setValue(bar->minimum());
		}
		else
			bar->setValue(bar->value()+event->delta());
	}
	event->accept();
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
	if (!f)
	{
		QMessageBox::warning(this,tr("Cannot create file"),tr("Creation file failed"),
			QMessageBox::Ok);
		return;
	}
	pdf->clone(f);
	fclose(f);
}
TabPage::~TabPage(void)	{}

///---private--------
void TabPage::addRevision( int i )
{
	if ( i < 0)//remove last
	{
		ui.Revision->removeItem(ui.Revision->count()-1);
		return;//page was loaded
	}
	std::cout << "adding revision ";
	std::stringstream ss;
	std::cout << i << " " << pdf->getRevisionsCount() << std::endl;
	assert( (size_t)i < pdf->getRevisionsCount() );
	ss << i;
	std::string s;
	ss >> s;
	ui.Revision->addItem(QString(("Revision " + s).c_str()));
	ui.Revision->setCurrentIndex(i-1);
}

///--------------------------------PRIVATE SLOTS----------------

void TabPage::initRevision(int  revision) //snad su revizie od nuly:-/
{
	size_t pos = page->getPagePosition();
	pdf->changeRevision(revision);
	if (pos > pdf->getPageCount())
		pos = pdf->getPageCount();
	setFromSplash();
}

void TabPage::rotate(int angle, int begin, int end)
{
	std::cout << "Rotating" << angle << std::endl;
	if (begin == -1)
	{
		page->setRotation(angle);
		setFromSplash();
		return;
	}
	else
	{
		for (int i = begin; i< end; i++)
		{
			pdf->getPage(i)->setRotation(angle);
		}
	}
}
void TabPage::commitRevision()
{
	//save revision to pdf
	this->pdf->save(true);
	if (pdf->getRevisionsCount() == (size_t)this->ui.Revision->count()+1)
		addRevision(this->ui.Revision->count());	
	else
	{ 
		std::cout << " Not changed" << pdf->getRevisionsCount() << std::endl;
	}
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
	//only last time!
	if (pdf->getRevisionsCount() == 1)
		return;
	pdf->changeRevision(pdf->getRevisionsCount()-1);
	size_t pos = page->getPagePosition();
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
	//TODO skontrolovat, ci sa zmenia zamky na subore
	if (pos >  pdf->getPageCount())
		pos = pdf->getPageCount();
	page = pdf->getPage(pos);
	addRevision();
}
void TabPage::insertRange()
{
	//opens file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open pdf file"),".",tr("PdfFiles (*.pdf)"));
	if (fileName == NULL)
		return;//cancel pressed
	InsertPageRange range(pdf,pdf->getPagePosition(page),this,fileName);
	//updateBar
	updatePageInfoBar();
}
void TabPage::addEmptyPage()
{
	//insert empty page
	boost::shared_ptr<pdfobjects::CDict> pageDict(pdfobjects::CDictFactory::getInstance());
	boost::shared_ptr<pdfobjects::CName> type(pdfobjects::CNameFactory::getInstance("Page"));
	pageDict->addProperty("Type", *type);
	boost::shared_ptr<pdfobjects::CPage> pageToAdd(new pdfobjects::CPage(pageDict));
	pdf->insertPage(pageToAdd, pdf->getPagePosition(page));//insert after
}
void TabPage::print()
{
	QPrinter printer(QPrinter::HighResolution);

	QPrintDialog dialog(&printer, this);
	dialog.setWindowTitle(tr("Print Document"));
	if (dialog.exec() != QDialog::Accepted)
		return;

	QPainter painter;
	painter.begin(&printer);

	SplashColor paperColor; //not working! dunno why
	paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
	SplashOutputDev splash (splashModeBGR8, 4, gFalse, paperColor);
	Guchar * p = new Guchar[3];
	for (size_t pos = 1; pos <= pdf->getPageCount(); ++pos) {

		// Use the painter to draw on the page.

		// display it = create internal splash bitmap
		pdf->getPage(pos)->displayPage(splash, displayparams);
		splash.clearModRegion();

		QImage image(splash.getBitmap()->getWidth(), splash.getBitmap()->getHeight(),QImage::Format_RGB32);

		for ( int i =0; i< image.width(); i++)
		{
			for ( int j =0; j < image.height(); j++)
			{
				splash.getBitmap()->getPixel(i,j,p);
				image.setPixel(i,j, qRgb(p[0],p[1],p[2]));
			}
		}
		//we have the image
		QSize size(300,300);
		QImage t = image.scaled(size);
		t.save("test resized.bmp");
		painter.drawImage(0,0,t);
		if (pos != pdf->getPageCount())
			printer.newPage();
	}
	delete[] p;
	//sends to printer
	painter.end();
}
boost::shared_ptr<PdfOperator> TabPage::findNearestFont(int x, int y)
{
	boost::shared_ptr<PdfOperator> fontOper;
	std::vector<boost::shared_ptr<CContentStream> > ccs;
	page->getContentStreams(ccs);
	int dist = 100000;
	for ( size_t i =0; i< ccs.size(); i++)
	{
		/*std::vector<boost::shared_ptr<PdfOperator> > op;
		ccs[i]->getPdfOperators(op);
		for (int j = 0; j < op.Size(); j++)
		{
			if (tolower(op[j]->getName()) != "tf")
				continue;
			//mame font, zistime jeho vzdialenost
			iBBox b = op[j]->BBox();
			float pom = b.xLext-x;
			float pom2 = b.xLeft - y;
			if (pom2 >=0 && pom >x)
				continue;
			if ( pom*pom + pom2*pom2 > dist )
				continue;
			dist = pom*pom + pom2*pom2;
			fontOper = op[j]; //closest
		}*/
	}
	return fontOper;
}
//slot
void TabPage::clicked()
{
	
}
void TabPage::insertText( std::string text )
{
	//najdi najblissi font, ktp ry bol pouzity

	shared_ptr<PdfOperator> fontOper = findNearestFont(x,y);
	//creat composite pdf operators with this font
	Ops ops;
	//vytvor BT, ET 
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));
	shared_ptr<UnknownCompositePdfOperator> BT(new UnknownCompositePdfOperator("BT", "ET"));

	q->push_back(BT,q);
	BT->push_back(fontOper->clone(), getLastOperator(BT));
	//text matrix 

	PdfOperator::Operands posOperands;
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(1)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(1)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(x)));
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(y)));
	BT->push_back(createOperator("Tm", posOperands), getLastOperator(BT));

	PdfOperator::Operands textOperands;
	textOperands.push_back(shared_ptr<IProperty>(new CString (text)));
	BT->push_back(createOperator("Tj", textOperands), getLastOperator(BT));
	PdfOperator::Operands emptyOperands;
	BT->push_back(createOperator("ET", emptyOperands), getLastOperator(BT));
	q->push_back(createOperator("Q", emptyOperands), getLastOperator(q));

	ops.push_back(q);
	page->addContentStreamToBack(ops);
	setFromSplash();
}
//zatial nepotrebujeme getText
void TabPage::copyToClipBoard() //from selected/ highlighted
{
	//ak cchem vybrat len selected text, musim si pamat, kde konci vybranie
	//we have operators, that were selected, must be text
	//inf workin set there isonly text operators(TJ)
	std::string text = "";
	for ( size_t i =0; i < workingOpSet.size(); i++)
	{
		std::string tmp;
		
		workingOpSet[i]->getStringRepresentation(tmp); //len cast, ktoru sme vyznacili
		//chceme vybrat len ten text, co sme vybrali

		text += tmp.substr(0,selectedText[i]);
	}
	//QApplication::clipboard()->setText(text); //TODO move UP
}
void TabPage::updateSelectedRect( std::vector<shared_ptr<PdfOperator> > oops)
{
	for ( int i =0; i < oops.size(); i++)
	{
	//	this->ui.content->drawRect(oops[i]);
	}
	setFromSplash();
}
void TabPage::addToSelectOperator(QRect rect) //nove rectangle, co sme pribrali
{
	std::vector<shared_ptr<PdfOperator> > oops;
	//hodime probkem na inu funkciu
	//selectOperator(rect, oops);
//	workingset.push_back(oops.begin(), oops.end());
	//mame pridane nove operatory
}
void TabPage::selectOperators(const QRect rect, std::vector<shared_ptr<PdfOperator> > & opers) 
{
	//vyberame operatory
//	libs::Rectangle r(rect.left(), rect.down(), rect.right(), rect.up());
//	page->getObjectAtPosition(opers,r ); //select operators on page

}
void TabPage::setSelectedOperators(QRect rect)
{
	workingOpSet.clear();
	//selectOperator(rect, workingOpSet);
	//mame nove operatory, s ktorymi sa mozeme hrat
}

void TabPage::move(int difx, int dify) //on mouse event, called on mouse realease
{
	//z tyh selectlych vsetko posunieme
	//ak je to text, nastavime novy maticu
	//ak je to nie =co ine..panbu pomoz
}
void TabPage::rotateText( int angle )
{
	//we have the qrange
	//we have to find textMatrix
	
}
//slot
void TabPage::deleteText( std::string text)
{
	//create tree of text on this page 
	//search text, delete string & insert new text
	std::vector < shared_ptr<PdfOperator> > operators;
}

//slot
void TabPage::replaceText( std::string what, std::string by)
{
	//delete what text.
	//insert by text
}
void TabPage::getText()//get text from page
{
	//get only text iterator
	std::vector<shared_ptr<CContentStream> streams;
	page->getContentStreams(streams);
	for ( int i =0; i < streams.size(); i++ )
	{
		Oops ops;
		streams[i]->getPdfOperators(ops);
		TextOperatorIterator it = PdfOperator::getIterator(ops.front());
		while(!it.isEnd())
		{

		}
	}
	TextOperatorIterator it = PdfOperator::getIterator<TextOperatorIterator> (opers.front());	
}
