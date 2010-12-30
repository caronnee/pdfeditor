#include "debug.h"
#include "TabPage.h"
#include "globalfunctions.h"
//QT$
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QScrollBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
//PDF
#include <kernel/pdfoperators.h>
#include <kernel/cannotation.h>
#include <kernel/carray.h>

//created files
#include "insertpagerange.h"
#include "tree.h"
#include "bookmark.h"
#include <float.h>

void TabPage::handleBookMark(QTreeWidget * item)
{
	page = pdf->getPage(((Bookmark *)(item))->getDest());
	setFromSplash();
}

TabPage::TabPage(QString name) : _name(name)
{
	_font = NULL;
	ui.setupUi(this);
	labelPage = new DisplayPage();
	this->ui.scrollArea->setWidget(labelPage);
	//hode verythind except...
	this->ui.pageManipulation->hide();
	this->ui.displayManipulation->hide();
	QObject::connect(this->ui.zoom, SIGNAL(currentIndexChanged(QString)),this,SLOT(zoom(QString)));
	dirty = false;

	acceptedAnotName.push_back("Link");
	acceptedAnotName.push_back("Text");
	acceptedAnotName.push_back("Highlight");
	acceptedAnotName.push_back("Underline");
	acceptedAnotName.push_back("Strikeout");

	//connections
	connect (ui.previous,SIGNAL(clicked()),this,SLOT(previousPage()));
	connect (ui.next,SIGNAL(clicked()),this,SLOT(nextPage()));
	connect (labelPage,SIGNAL(MouseClicked(int, int)),this, SLOT(clicked(int, int))); //pri selecte sa to disconnectne a nahrasi inym modom
	connect(ui.tree,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(handleBookmark((QTreeWidgetItem *,int))));
	//end of connections

	pdf = boost::shared_ptr<pdfobjects::CPdf> ( pdfobjects::CPdf::getInstance (name.toAscii().data(), pdfobjects::CPdf::ReadWrite));

	page = boost::shared_ptr<pdfobjects::CPage> (pdf->getPage(1)); //or set from last
	// init splash bitmap
	QStringList list;

	std::string s;
	for ( size_t i = 0; i< pdf->getRevisionsCount(); i++)
	{
		std::stringstream ss;
		ss << i;
		ss >> s;
		list.append(("Revision " + s).c_str());
	}
	ui.Revision->addItems(list);
	ui.Revision->setCurrentIndex(list.count()-1);
	setFromSplash();
	_dataReady = false;
	//setZoom();
	for ( int i =50; i< 500; i+=50)
	{
		QVariant s(i);
		this->ui.zoom->addItem( s.toString()+" %",s);
	}
	this->ui.zoom->setCurrentIndex(1);
	SetModeTextSelect();
}
void TabPage::zoom(QString zoomscale)//later with how much pages, if all or not
{
	//odstranit breberky za tym
	zoomscale = zoomscale.remove("%");
	zoomscale = zoomscale.remove(" ");
	float scale = zoomscale.toFloat()/100;
	float dpix=labelPage->logicalDpiX();
	float dpiy =labelPage->logicalDpiY();
	displayparams.hDpi = dpix * scale;
	displayparams.vDpi = dpiy * scale;
	this->setFromSplash();
}

void TabPage::SetModeTextSelect()
{
	//bind widget to the page show
	_mode = TextMode;	
	_font = new FontWidget(NULL);
	loadFonts(_font);
	_font->show();
	connect(_font, SIGNAL(text(PdfOp)), this, SLOT(insertText(PdfOp)));
	//show text button, hide everything else
	if (_textList.empty())
	{
		//get all pdf text operats in list
		Ops ops;
		libs::Rectangle rect(0,0,FLT_MAX,FLT_MAX);
		page->getObjectsAtPosition( ops, rect);
		//choose just testiterator
		Ops::iterator it = ops.begin();
		while ( it != ops.end())
		{
			std::string n; 
			(*it)->getOperatorName(n);
			if (!typeChecker.isType(OpTextName,n))
			{
			//	DEBUGLINE(n);
				it++;
				continue;
			}
			OperatorData data(*it);
			_textList.push_back(data);
			it++;
		}
		//sort list
		_textList.sort();
		dirty = true;
	}
}
void TabPage::UnSetTextSelect()
{
	_mode = DefaultMode;
	if(_font)
		delete _font;
	_font = NULL;
}
void TabPage::clicked(int x, int y) //resp. pressed, u select textu to znamena, ze sa vyberie prvy operator
{
	switch (_mode)
	{
		case TextMode:
		{
			highlightText(x,y); //nesprav nic, pretoze to bude robit mouseMove
			break;
		}
		default:
		{
			labelPage->unsetImg();
			showClicked(x,y);//zmenit 
		}
	}
}
void TabPage::mouseReleased() //nesprav nic, pretoze to bude robit mouseMove
{
	_dataReady = false;//TODO tu som este nieco chcela
	DEBUGLINE("Data released");
}
void TabPage::highLightBegin(int x, int y) //nesprav nic, pretoze to bude robit mouseMove
{
	//najdi prvy operator, na ktory bolo kliknute
	Ops ops;
	getAtPosition(ops,x,y); //zaplnili sme operator
	//zistime, ze je to text
	std::string n;
	if (ops.empty())
		return;
	ops.back()->getOperatorName(n);
	if (!typeChecker.isType(OpTextName,n))
		return; //zoberieme iba posledny, viditelny, ak su na sebe
	_dataReady = true;
	DEBUGLINE("Operator found");
	sTextIt = _textList.begin();
	setTextData(sTextIt,_textList.end(),ops.back());
	sTextIt->setBegin(x);//zarovnane na pismenko
	sTextItEnd = sTextIt;
}
void TabPage::setTextData(TextData::iterator & it, TextData::iterator end,shared_ptr< PdfOperator > op)
{
	for ( ; it!= end; it++)
	{
		if (op == it->_op)
			return;
	}
	throw "Unexpected operator, text is not present in tree, why, why? ";
}
void TabPage::highlightText(int x, int y) //tu mame convertle  x,y, co sa tyka ser space
{
	if (!_dataReady) //prvykrat, co sme dotkli nejakeho operatora
	{
		labelPage->unsetImg( );
		highLightBegin(x,y);
		return;
	}
	//highlightuj
	//pohli sme sa na x, y
	//ak sme sa pohli "dopredu" v zmysle dopredu textu, tal sme OK
	//najdime operator
	Ops ops;
	getAtPosition(ops,x,y);
	//ak nie je ziadny textovy operator
	while ( !ops.empty())
	{
		std::string n;
		ops.back()->getOperatorName(n);
		if(typeChecker.isType( OpTextName, n))
			break;
		ops.pop_back();
	}
	if (ops.empty()) // pridaj najblizsie text
		return;
	//ideme vysvietit posledne. Ak je na tom mieste viacero textov, vysvietia sa podla toho, kde su v nasom liste, ale stene to bude fuj
	//najdime teto operator
	//ak sme sa pohli dopredu, tak y je mensia ako posledne, popripade x vyssie

	libs::Rectangle b;
	int x1, y1;
	int x2, y2;
	QColor color(255,36,255,50);
	sTextItEnd->restoreEnd();
	if (sTextItEnd->forward(x,y))
	{
		DEBUGLINE("forward");
		//najdi ten operator, ktoreho sme sa dotkli
		while (sTextItEnd->_op != ops.back())
		{
			if (sTextItEnd == _textList.end())
				throw "neni v tree"; //TODO potom toto tu vymazat
			b = sTextItEnd->_op->getBBox();
			toPixmapPos(sTextItEnd->_begin, b.yleft, x1,y1);
			toPixmapPos(sTextItEnd->_end, b.yright, x2, y2);
			labelPage->fillRect( x1, y1, x2, y2, color );
			sTextItEnd++;
		}		
		double a,b;
		toPdfPos(x, y, a, b);
		sTextItEnd->setEnd(a);
		toPixmapPos(sTextItEnd->_begin, sTextItEnd->_ymin, x1,y1);
		toPixmapPos(sTextItEnd->_end, sTextItEnd->_ymax, x2, y2);
		labelPage->fillRect( x1, y1, x2, y2, color );
		return;
	}
	else //pohybujeme sa smerom dozadu
	{
		DEBUGLINE("before");
		return;
		while (sTextItEnd->_op != ops.back())
		{
		}
	}
	//teraz zistime, co sme to vlastne spachali, t.j vysvietime to, co sme vysvietili povodne
	//najskor budeme rata s tym, ze nebudeme pridavat pomocou ctrl:)
	TextData::iterator first = sTextIt;
	TextData::iterator last = sTextItEnd; //TODO opravit na to, aby s dam davali aj medzery a pod, mozno by stacil upravit BBox, rozirit a dat do stromu
	if( *first < *last)
	{
		TextData::iterator p = first;
		first = last;
		last = p; //zmenili mse
		first->change();
		last->change();
	}
	//sTextItEnd->addToRegion(x);
	//teraz potrebujeme cely highlight vymazat //TODO to by sa malo zmenit, bo je to desne pomale
	_dataReady = false;
}
void TabPage::getAtPosition(Ops& ops, int x, int y )
{
	double px, py;
	toPdfPos(x, y, px, py);
	//find operattors
	page->getObjectsAtPosition(ops, libs::Point(px,py));
}
void TabPage::toPdfPos(int x, int y, double & x1, double &y1)
{
	displayparams.convertPixmapPosToPdfPos(x, y, x1, y1);
	x1 *=displayparams.hDpi/72;
	y1 *=displayparams.vDpi/72;
}
void TabPage::toPixmapPos(double x1, double y1, int & x, int &y)
{
	double x2, y2;
	x1 /=displayparams.hDpi/72;
	y1 /=displayparams.vDpi/72;
	displayparams.convertPdfPosToPixmapPos(x1, y1, x2, y2);
	x = x2;
	y = y2;
}
void TabPage::showClicked(int x, int y)
{
	double px, py;
	//convert
	toPdfPos(x,y, px, py);
	//find operattdisplayparamsors
	Ops ops;
	page->getObjectsAtPosition(ops, libs::Point(px,py));
	workingOpSet.clear();	
	//vsetky tieto objekty vymalujeme TODO zistit orientaciu
	for ( size_t i =0; i < ops.size(); i++)
	{
		//ukaz len povolene typy
		std::string s;
		ops[i]->getOperatorName(s); 
		if (!typeChecker.acceptType(s))
			continue;
		shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(ops[i]);
		txt->getRawText(s);
		libs::Rectangle b = ops[i]->getBBox();
		int x1, y1;
		int x2, y2;

		toPixmapPos(b.xleft, b.yleft, x1,y1);
		toPixmapPos(b.xright, b.yright, x2, y2);

		QColor color(255, 255, 0, 50);
		labelPage->fillRect( x1, y1, x2, y2, color );
		workingOpSet.push_back(ops[i]);	
	}
}
QRect TabPage::getRectangle(shared_ptr < PdfOperator> ops)
{
	QRect r;
	libs::Rectangle b = ops->getBBox();
	int x1,y1,x2,y2;
	
	toPixmapPos(b.xleft, b.yleft, x1,y1);
	toPixmapPos(b.xright, b.yright, x2, y2);
//move according to page rotation
	/*int angle = page->getRotation();

	DEBUGLINE(y1);
	DEBUGLINE(y2);
	DEBUGLINE(angle);
	rotatePosition(x1,y1,x1,y1, angle);
	rotatePosition(x2,y2,x2,y2, angle);
	getc(stdin);*///TODO rotation when displaying
	
	r.setTop(min<float>(y1,y2));
	r.setBottom(max<float>(y1,y2));
	r.setLeft(min<float>(x1,x2));
	r.setRight(max<float>(x1, x2));
	return r;
}
void TabPage::updatePageInfoBar()
{
	//page changes
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
void TabPage::getBookMarks()
{
	//LATER, TODO, zistit, na aku stranku sa odkazuju, potazne odsek
	//ket from XREGWritel all GoLink
	//get from dictionary outlines  and get everythong that has page reference
	//asi to nebudeme hrotit
	//na nejak on show()
	std::vector<shared_ptr<CDict> > outline;
	pdf->getOutlines(outline);
	std::vector<shared_ptr<CDict> > dicts;
	for (size_t i =0; i< outline.size(); i++)
	{
		QTreeWidgetItem * b = new QTreeWidgetItem; 
		setTree(outline[i],b);
		this->ui.tree->addTopLevelItem(b);
	}
	//skrtni kazde, ktore nema page ako dest
}
void TabPage::setTree(shared_ptr<CDict> d, QTreeWidgetItem * item)
{
	std::vector<shared_ptr<CDict> > dict;
	try{
		QTreeWidgetItem * b;
		utils::getAllChildrenOfPdfObject(d,dict);
		if (d->containsProperty("Dest"))
		{
			int page;
			shared_ptr<CArray> ar = IProperty::getSmartCObjectPtr<CArray>(d->getProperty("Dest"));
			shared_ptr<IProperty> ip = ar->getProperty(0);
			page = utils::getValueFromSimple<CInt>(ip);
			b = new Bookmark(page);
		}
		else
			b = new Bookmark(-1);
		for(size_t i =0; i < dict.size(); i++)
		{
			setTree(dict[i],b);
			item->addChild(b);
		}
	}
	catch(...) {}
}
void TabPage::removeObjects() //vsetko, co je vo working
{
	for (size_t i = 0; i < workingOpSet.size(); i++ )
	{
		PdfOperator::Iterator it = PdfOperator::getIterator(workingOpSet[i]);
		workingOpSet[i]->getContentStream()->deleteOperator(it,true);
	}
	workingOpSet.clear();
}
void TabPage::insertImage(int x, int y, const QImage& img) //positions
{
	const uchar * c = img.bits();
	std::vector<char> ch(c, c + img.byteCount() );
	CStream::Buffer buf(ch);

	QSize size = img.size();
	page->addInlineImage(buf,libs::Point(size.width(),size.height()), libs::Point(x,y));
	setFromSplash();

}
void TabPage::insertImageFile(int x, int y)
{
	//open dialogand get file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.xpm *.jpg)"));
	QImage img(fileName);
	insertImage(x,y,img);
	//convert to buffer
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
	//image = image.scaled(QSize(max(x2,x1),max(y1,y2)));
	labelPage->setImage(image);
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
	event->accept(); //TODO opravit
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
	std::cout << "Adding revision ";
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

void TabPage::rotate(int angle, int begin, int end) //rotovanie pages
{
//std::cout << "Rotating" << angle << std::endl;
	if (begin == -1)
	{
		page->setRotation(angle);
	}
	else
	{
		for (int i = begin; i< end; i++)
		{
			pdf->getPage(i)->setRotation(angle);
		}
	}
	setFromSplash();
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

	QPainter painter(&printer);

	SplashColor paperColor;
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
		QSize size(printer.pageRect().width(), printer.pageRect().height());
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



void TabPage::draw() //change mode to drawing
{
//	this->ui.content->beginDraw();
	_mode = DrawMode;
}
//na kazdej stranke mozu byt anotacie, po kliknuti na ne vyskoci pop-up alebo sa inak spravi akcia
//page bude vediet o interaktovnyh miestach -> kvoli mouseMove
void TabPage::setAnnotations()
{
	//akonahle sa zmeni straka, upozornim page na to ze tam moze mat anotacie
	//dostan oblasti anotacii z pdf
	page->getAllAnnotations(_annots);
	//v page nastav vsetky aktivne miesta
	for(size_t i =0; i< _annots.size(); i++)
	{
		shared_ptr<CArray> rect;
		_annots[i]->getDictionary()->getProperty("Rect")->getSmartCObjectPtr<CArray>(rect);
		int x1,x2,y1,y2;
		x1 = utils::getSimpleValueFromArray<CInt>(rect,0);
		y1 = utils::getSimpleValueFromArray<CInt>(rect,0);
		x2 = utils::getSimpleValueFromArray<CInt>(rect,0);
		y2 = utils::getSimpleValueFromArray<CInt>(rect,0);
		//dostat annotecny rectangle
		BBox b(x1,y1,x2,y2);	
		QRect convertedRect = getRectangle(b);
		labelPage->addPlace(convertedRect);
	}
}

void TabPage::createAnnot(AnnotType t, std::string * params)
{
	//mame begin iter a enditer
	shared_ptr<CAnnotation> annot;
	switch (t)
	{
		case TextAnnot:
		{
			//potrebujeme iba text
			shared_ptr<IProperty> prop ( CNameFactory::getInstance(params[0].c_str()));
			std::string n = "Contents";
			annot->getDictionary()->addProperty(n,*prop);
			break;
		}
		default:
			break;
	}
}
void TabPage::delAnnot(int i) //page to u seba upravi, aby ID zodpovedali
{
	page->delAnnotation(_annots[i]);
	_annots[i] = _annots.back();
	_annots.pop_back();
}
QRect TabPage::getRectangle(BBox b)
{
	int x1,x2,y1,y2;
//TODO tot snad ani nemusi fungovat...check!
	toPixmapPos(b.xleft, b.yleft, x1,y1);
	toPixmapPos(b.xright, b.yright, x2,y2);
	QRect r(QPoint(min(x1,x2),max(y1,y2)),QPoint(max(x1,x2),min(y1,y2)));
	return r;
}


void TabPage::loadFonts(FontWidget* fontWidget)
{
	//dostanme vsetky fontu, ktore su priamov pdf. bohuzial musime cez vsetky pages
	CPage::FontList fontList;
	for ( size_t i = 1; i <= pdf->getPageCount(); i++ )
	{
		pdf->getPage(i)->getFontIdsAndNames(fontList);
		//fontList.insert(fontList.end(), fontList2.begin(), fontList2.end());
		for( CPage::FontList::iterator it = fontList.begin(); it!=fontList.end(); it++)
		{
			fontWidget->addFont(it->first, it->second);
		}

	}
/*	CPageFonts::SystemFontList flist = CPageFonts::getSystemFonts();
	for ( CPageFonts::SystemFontList::iterator i = flist.begin(); i != flist.end(); i++ )
	{
		fontWidget->addFont(*i,*i);
	}*/
}
void TabPage::insertText( PdfOp op )
{
	//mame operator vytvoreny vo fontWidget
	//pridame do page
	//creat composite pdf operators with this font
	Ops ops;
	ops.push_back(op);
	page->addContentStreamToBack(ops);
	setFromSplash();
}

//slot

void TabPage::getText()//get text from page
{
	BBox b;
	//get only text iterator
	std::vector<shared_ptr<CContentStream> > streams;
	page->getContentStreams(streams);
	std::string tmp ="";
	for ( size_t i =0; i < streams.size(); i++ )
	{
		Ops ops;
		streams[i]->getPdfOperators(ops);
		TextOperatorIterator it = PdfOperator::getIterator<TextOperatorIterator> (ops.front());	
		while(!it.isEnd())
		{
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(it.getCurrent());
			std::string s;
			txt->getRawText(s);//bacha na t, ze to moze byt te lomitkova a hexadec repr..alebo nie?
			tmp+= s;
			BBox box = it.getCurrent()->getBBox();
			b.xright = max(max(box.xright,box.xleft),b.xright);
			b.xleft = max(max(box.xleft,box.xright),b.xleft);
			b.yright = max(max(box.yright,box.yleft),b.yright);
			b.yleft = max(max(box.yleft,box.yright),b.yleft);
			it.next();
		}
	}
//	std::cout << tmp << std::endl; //show new box
}
/*
void TabPage::showTextAnnot(std::string name)
{
	//TODO novy textbox
}
void TabPage::replaceText( std::string what, std::string by)
{
	////TODO
	//delete what text.
	//insert by text
}
//slot
void TabPage::deleteText( std::string text)
{
	//create tree of text on this page 
	//search text, delete string & insert new text
	Ops opers;
	while (search(text, opers))
	{
		std::string todel = text;
		
		//ide nam len o zaciatok a koniec
		//v kazdom operator bude kusok
		//vsetko medzi tym sa zmaze
	}
}
//bolo kliknute na anotaciu, ideme ju vykonat
void TabPage::handleAnnotation(int id)
{
	//mame identifikator
	//_annots;
	//Ukazeme widget s tymto textom
	//ak je to link, tak rovno skocime
	shared_ptr<CAnnotation> c = _annots[id];
	//zistime, kam mame skocit
	shared_ptr<IProperty> name = c->getDictionary()->getProperty("SubType");
	std::string n = utils::getValueFromSimple<CName>(name);
	if ( n == "Link")
	{
		//dostan page, na ktoru ma ist
		if (c->getDictionary()->containsProperty("Dest"))
		{
			//ok,skaceme
			shared_ptr< CArray > array; 
			c->getDictionary()->getProperty("Dest")->getSmartCObjectPtr<CArray>(array);
			page = pdf->getPage(utils::getSimpleValueFromArray<CInt>(array,0));
			setFromSplash();
			return;
		}//alebo action
		if (c->getDictionary()->containsProperty("A"))
		{
			shared_ptr<CDict> d; 
			d = utils::getCDictFromDict(c->getDictionary(),"A"); //toto musi byt Launch, TODO overit
			std::string nam = utils::getStringFromDict(d,"F");
			//zistime lauch a jeho parametre
			//TODO QDialog a launch
		}
		throw "Unsupported type";//TODO vymazat z anotacii alebo ich tam vobec nedavat
		return;
	}
	//other Annotations
	//tot by malo zobrazit pdfko
	std::string name2 = utils::getStringFromDict(c->getDictionary(), "Contents");
	showTextAnnot(name2);
	return;
}
//rotate 
void TabPage::rotateObjects(int angle) //vsetky objekty wo workingOpSet
{
	//selected operator will be removed and new QSTATE added
	//let's look for Q
	float rAngle = toRadians(angle); //su nastavene iterBegin a iterEnd, z neho vypraprarujeme working set
	shared_ptr< PdfOperator > parent;
	for ( int i =0; i < workingOpSet.size(); i++)
	{
		Ops ops;
		workingOpSet[i]->getContentStream()->getPdfOperators(ops);
		PdfOperator::Iterator it = 
		PdfOperator::getIterator(ops.front());
		parent = findCompositeOfPdfOperator(it, workingOpSet[i]);
		//for graphical object
		std::string opName;
		parent->getOperatorName(opName);
		for ( int i =0; i< opName.length(); i++)
			opName[i] = tolower(opName[i]);
		OpsList children;
		if (strcmp(opName.c_str(),"q")==0)
		{
			//add cm rotation matrix
			PdfOperator::Operands operands;
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			shared_ptr< PdfOperator > o = createOperator("cm",operands);
			//addOperator
			parent->getChildren(children);
			parent->getContentStream()->insertOperator(PdfOperator::getIterator(children.front()),o,true);//FUJ, to snad ani nemoze fungovat..., a mozno to chce false
		}
		if (strcmp(opName.c_str(),"bt"))
		{
			PdfOperator::Operands operands;
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			shared_ptr< PdfOperator > o = createOperator("Tm",operands);
			//addOperator
			parent->getChildren(children);
			parent->getContentStream()->insertOperator(PdfOperator::getIterator(children.front()),o,true);//FUJ, to snad ani nemoze fungovat...
		}

	}
}
void TabPage::rotateText(int angle) //there can be text or image objects
{
	//for all selected operator, rotate, but just t
	for ( size_t i =0; i < workingOpSet.size(); i++)
	{
		if (!isTextOp(workingOpSet[i]))
			continue;
		//we have TJ, remove object from parent, create new at same position, but anothe rotate, otherwise we could alter also another text
		//get textMatrix
		shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(workingOpSet[i]);
		std::string s; 
		txt->getRawText(s);
		BBox b = txt->getBBox();

		parent->remove(workingOpSet[i]); //remove from parent, linearizator will remove empty operators
		workingOpSet[i] = insertText(min(b.xLeft,b.xRight),min(b.yLeft,b.yRight),s, angle);
		
	}
}
boost::shared_ptr<PdfOperator> TabPage::findNearestFont(int x, int y)
{
	boost::shared_ptr<PdfOperator> fontOper;
	std::vector<boost::shared_ptr<CContentStream> > ccs;
	page->getContentStreams(ccs);
	int dist = 100000;
	for ( size_t i =0; i< ccs.size(); i++)
	{
		std::vector<boost::shared_ptr<PdfOperator> > op;
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
		}
	}
	return fontOper;
}
void TabPage::move(int x, int y) //on mouse event, called on mouse realease
{
	//for each selected operator, move it accrding to position
	for ( size_t i =0; i< workingOpSet.size(); i++)
	{
		//u textov nastavime Td
		
	}
}
//slot
void TabPage::riseSel()
{
	//move only text operators

	TextOperatorIterator it = PdfOperator::getIterator<TextOperatorIterator> (workingOpSet.front());
	while (!it.isEnd())
	{
		//len Tj, potrebujeme pred pridat Ts, ak uz predty nejake ts nie je
		// dostaneme composit
		PdfOperator::Iterator bit = PdfOperator::getIterator<PdfOperator::Iterator> (workingOpSet.front());
		shared_ptr< PdfOperator >  parent = findCompositeOfPdfOperator(bit,it.getCurrent());
		//insert before, if full, first insert and then remove
		
		PdfOperator::Operands intop;
		intop.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-5)));
		parent->push_back( it.getCurrent()->clone(),it.getCurrent());//TODO must check if Tj is only one
		parent->push_back( createOperator("Tj", intop),it.getCurrent());//TODO must check if Tj is only one
		
//split text acording to highlighted
	}

}
void TabPage::changeText(std::string name, int size) //toto bude vlastnostiach(dialog)
{
	//mame vyznaceny beginiter a end string
	Ops matrix;
	TextData::iterator it = sTextIt;
	while ( it!= sTextItEnd )
	{
		//
		it++;
	}
}
void TabPage::search(std::string text)
{
	TextData::iterator it = _textList.begin();
	Tree t(text);
	int end;
	while (it != _textList.end() )
	{
		end = t.fill(it->_text);
		if(end>=0)
			break;
		it++;
	}
	if (end<0)
		return;
	//mame posledny operator
	TextData::iterator beg = it;
	it->end = end;
	it->begin = 0;

	int sum = t.getSize();
	//chod dozadu az sum nebude 0
	while (sum<0)
	{
		sum-= beg->text.length();
		beg--;
		beg->begin = 0;
		beg->end=beg->text.length();
	}
	if (sum <= 0)
		beg->begin = beg->end+sum;
	else
		beg->begin = 0;
	beg->setRegion(); //setne vzhladom na begin, end
	while (beg!=it)
	{
		this->ui.content->fillRect(beg->region);
		beg++;
	}
	this->ui.content->fillRect(beg->region);
}
*/
