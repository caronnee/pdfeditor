// Change font, change color
//TODO check if rotating rotate just the one object or all of them?
//pre dial rotate text only, rotate all
//highlightText
//search

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
//PDF
#include <kernel/factories.h>
#include <kernel/pdfoperators.h>
//created files
#include "insertpagerange.h"
#include "tree.h"
#include "bookmark.h"

#define DEFFONT "Helvetica"

void TabPage::handleBookMark(QTreeWidget * item, int col)
{
	page = pdf->getPage(((Bookmark *)(item))->getDest());
	setFromSplash();
}
TabPage::TabPage(QString name) : _name(name)
{
	ui.setupUi(this);
	dirty = false;

	//connections()
	connect (ui.previous,SIGNAL(clicked()),this,SLOT(previousPage()));
	connect (ui.next,SIGNAL(clicked()),this,SLOT(nextPage()));
	connect (ui.content,SIGNAL(MouseClicked(int, int)),this, SLOT(clicked(int, int))); //pri selecte sa to disconnectne a nahrasi inym modom
	connect(ui.tree,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(handleBookmark((QTreeWidgetItem *,int))));
	//end of connections

	pdf = boost::shared_ptr<pdfobjects::CPdf> ( pdfobjects::CPdf::getInstance (name.toAscii().data(), pdfobjects::CPdf::ReadWrite));

	page = boost::shared_ptr<pdfobjects::CPage> (pdf->getPage(1)); //or set from last
	//page->addText("myText...XXXX",libs::Point(1,1),"Helvetica");
	//pdf->save();
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
	SetModeTextSelect();
	_dataReady = false;
}

void TabPage::SetModeTextSelect()
{
	_mode = SelectText;	
	//show text button, hide everything else
	if (_textList.empty())
	{
		//get all pdf text operats in list
		Ops ops;
		page->getObjectsAtPosition( ops, displayparams.pageRect);
		//choose just testiterator
		Ops::iterator it = ops.begin();
		while ( it != ops.end())
		{
			std::string n; 
			(*it)->getOperatorName(n);
			if (!typeChecker.isType(OpTextName,n))
				continue;
			OperatorData data;
			data.op = *it;
			data.begin = data.end = 0;
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(data.op);
			txt->getRawText(data.text);
			data.rect = getRectangle(data.op);
			_textList.push_back(data);
		}
		//sort list
		dirty = true;
	}
}
void TabPage::UnSetTextSelect()
{
	_mode = Default;
}
void TabPage::clicked(int x, int y) //resp. pressed, u select textu to znamena, ze sa vyberie prvy operator
{
	switch (_mode)
	{
		case SelectText:
		{
		//	highLightBegin(x,y); //nesprav nic, pretoze to bude robit mouseMove
			break;
		}
		default:
			showClicked(x,y);//zmenit 
	}
}
void TabPage::mouseReleased() //nesprav nic, pretoze to bude robit mouseMove
{
	_dataReady = false;//TODO tu som este nieco chcela
}
void TabPage::highLightBegin(int x, int y) //nesprav nic, pretoze to bude robit mouseMove
{
	//najdi prvy operator, na ktory bolo kliknute
	Ops ops;
	getAtPosition(ops,x,y); //zaplnili sme operator
	//zistime, ze je to text
	std::string n;
	ops.back()->getOperatorName(n);
	if (!typeChecker.isType(OpTextName,n))
		return; //zoberieme iba posledny, vyditelny, ak siu na sebe
	_dataReady = true;
	sTextIt = _textList.begin();
	sTextItEnd = sTextIt;
	setTextData(sTextIt,_textList.end(),ops.back());
}
void TabPage::setTextData(TextData::iterator & it, TextData::iterator end,shared_ptr< PdfOperator > op)
{
	for ( ; it!= end; it++)
	{
		if (op == it->op)
			return;
	}
	throw "Unexpected operator, text is not present in tree, why, why? ";
}
void TabPage::highlightText(int x, int y) //tu mame convertle  x,y
{
	if (!_dataReady) //prvykrat, co sme dotkli nejakeho operatora
	{
		_region = QRegion(); //region je tie convertly
		highLightBegin(x,y);
		return;
	}
	//highlightuj
	//pohli sme sa na x, y
	//ak sme sa pohli "dopredu" v zmysle dopredu textu, tal sme OK
	//najsime operator
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
	if (ops.empty())
	{
		_dataReady = false;
		return;
	}//ideme vysvietit posledne. Ak je na tom mieste viacero textov, vysvietia sa podla toho, kde su v nasom liste, ale stene to bude fuj
	//najdime teto operator
	//ak sme sa pohli dopredu, tak y je mensia ako posledne, popripade x vyssie
	if (sTextItEnd->forward(x,y))
	{
		//najdi ten operator, ktoreho sme sa dotkli
		while (sTextItEnd->op != ops.back())
		{
			if (sTextItEnd == _textList.end())
				throw "neni v tree"; //TODO potom toto tu vymazat
			sTextItEnd++;
			if (sTextItEnd!=sTextIt)
				sTextItEnd->begin = 0;
		}
	}
	else //pohybujeme sa smerom dozadu
	{
		while (sTextItEnd->op != ops.back())
		{
			if (sTextItEnd == _textList.end())
				throw "neni v tree"; //TODO potom toto tu vymazat
			sTextItEnd--;
			if (sTextItEnd!=sTextIt)
				sTextItEnd->begin = 0;
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
	}
	sTextItEnd->addToRegion(x);

	//retapotrebujeme cely hilight vymazat //TODO to by sa malo zmenit, bo je to desne pomale
	ui.content->unsetImg( );
	while( first!=last)
	{
		QColor c(255,36,78);
		ui.content->fillRect( first->region,c );
		first ++;
	}
}
void TabPage::getAtPosition(Ops& ops, int x, int y )
{
	double px, py;
	displayparams.convertPixmapPosToPdfPos(x, y, px, py);
	//find operattors
	page->getObjectsAtPosition(ops, libs::Point(px,py));
}
void TabPage::showClicked(int x, int y)
{
	double px, py;
	displayparams.convertPixmapPosToPdfPos(x, y, px, py);
	//find operattors
	std::vector< shared_ptr < PdfOperator> > ops;
	page->getObjectsAtPosition(ops, libs::Point(px,py));
	workingOpSet.clear();	
	//vsetky tieto objekty vymalujeme TODO zistit orientaciu
	for ( int i =0; i < ops.size(); i++)
	{
		std::string s;
		ops[i]->getOperatorName(s); 

		libs::Rectangle b = ops[i]->getBBox();
		double x1, y1, x2, y2;

		displayparams.convertPdfPosToPixmapPos(b.xleft, b.yleft, x1,y1);
		displayparams.convertPdfPosToPixmapPos(b.xright, b.yright, x2, y2);

		QColor color(255, 255, 0, 50);
		this->ui.content->fillRect( x1, y1, x2, y2, color );
		workingOpSet.push_back(ops[i]);	
	}
}
QRect TabPage::getRectangle(shared_ptr < PdfOperator> ops)
{
	QRect r;
	libs::Rectangle b = ops->getBBox();
	double x1,y1,x2,y2;
	
	displayparams.convertPdfPosToPixmapPos(b.xleft, b.yleft, x1,y1);
	displayparams.convertPdfPosToPixmapPos(b.xright, b.yright, x2, y2);
//move according to page rotation
	int angle = page->getRotation();
	rotatePosition(x1,y1,x1,y1, angle);
	rotatePosition(x2,y2,x2,y2, angle);
	r.setTop(max<float>(y1,y2));
	r.setBottom(min<float>(y1,y2));
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
	Bookmark * b; 
	for (int i =0; i< outline.size(); i++)
	{
		for ( int i =0; i < outline.size(); i++)
		{
			setTree(outline[i],b);
			this->ui.tree->addTopLevelItem(b);
		}
	}
	//skrtni kazde, ktore nema page ako dest
}
void TabPage::setTree(shared_ptr<CDict> d, QTreeWidgetItem * item)
{
	std::vector<shared_ptr<CDict> > dict;
	try{
		QTreeWidgetItem * it;
		utils::getAllChildrenOfPdfObject(d,dict);
		Bookmark * b;
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
		for(int i =0; i < dict.size(); i++)
		{
			setTree(dict[i],it);
			item->addChild(it);
		}
	}
	catch(...) {}
}
void TabPage::insertImage(int x, int y) //positions
{
	//open dialogand get file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.xpm *.jpg)"));
	QImage img(fileName);
	//convert to buffer
	uchar * c = img.bits();
	std::vector<char> ch(c, c + img.byteCount() );
	CStream::Buffer buf(ch);

	QSize size = img.size();
	page->addInlineImage(buf,libs::Point(size.width(),size.height()), libs::Point(x,y));
	setFromSplash();
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
void TabPage::riseSel()
{
	//move only text operators

/*	TextOperatorIterator it = PdfOperator::getIterator<TextOperatorIterator> (workingOpSet.front());
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
	}*/

}
//rotate 
void TabPage::rotateObjects(int angle) //there can be text or image object
{
	//selected operator will be removed and new QSTATE added
	//let's loolk for Q
/*	float rAngle = toRadians(angle);
	shared_ptr< PdfOperator > parent;
	for ( int i =0; i < workingOpSet.size(); i++)
	{
		std::vector<PdfOperator> ops;
		workingOpSet[i]->getContentStream()->getPdfOperators(ops);
		PdfOperator::Iterator it = 
			PdfOperator::getIterator(ops.front());
		parent = findCompositeOfPdfOperator(it, workingOpSet[i]);
		//for graphical object
		if (strcmp(tolower(parent->getName())),"q")
		{
			//add cm rotation matrix
			pdfOperator::Operands operands;
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			shared_ptr< PdfOperator > o = createOperator("cm",operands);
			//addOperator
			parent->putBehind(getFirstOperator(parent), o);//
		}
		if (strcmp(tolower(parent->getName())),"bt")
		{
			pdfOperator::Operands operands;
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-sin(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cos(rAngle))));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			operands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(0)));
			shared_ptr< PdfOperator > o = createOperator("Tm",operands);
			//addOperator
			std::vector< PdfOperator> children;
arent->getChildren(children);
			parent->putBehind(children.front(), o);////rotate via tm
		}

	}*/
}
void TabPage::rotateText(int angle) //there can be text or image objects
{
	//for all selected operator, rotate, but just t
/*	for ( size_t i =0; i < workingOpSet.size(); i++)
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
		
	}*/
}
/*void TabPage::add(int x, int y)
{
//select operators and add to selected	
	double px, py;
	displayparams.convertPixmapPosToPdfPos(x, y, px, py);
	//find operattors
	std::vector< shared_ptr < PdfOperator> > ops;
	page->getObjectsAtPosition(ops, libs::Point(px,py));
	for ( int i =0; i<ops.size(); i++)
	{
		OperatorData d;
		d.begin = -1; //full
		d.end = 0;
		d.op = ops[i];
		workingOpSet.push_back(d);
	}
}*/
shared_ptr<PdfOperator> TabPage::insertText(double x, double y, std::string text, int angle )
{
	//creat composite pdf operators with this font
	Ops ops;
	//vytvor BT, ET 
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));
	shared_ptr<UnknownCompositePdfOperator> BT(new UnknownCompositePdfOperator("BT", "ET"));

	q->push_back(BT,q);
	PdfOperator::Operands fOperands;//TODO check poradie
	fOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(14)));//velkost pisme
	BT->push_back(createOperator("Tf", fOperands), getLastOperator(BT));
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
	return q;
}

void TabPage::move(int difx, int dify) //on mouse event, called on mouse realease
{
	//for each selected operator, move it accrding to position
	for ( size_t i =0; i< workingOpSet.size(); i++)
	{
		//u textov nastavime Td

	}
}
void TabPage::search(std::string text)
{
/*	TextData::iterator it = _textList.begin();
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
	this->ui.content->fillRect(beg->region);*/
}
//slot
void TabPage::deleteText( std::string text)
{
	//create tree of text on this page 
	//search text, delete string & insert new text
	std::vector<PdfOperator> opers;
/*	while (search(text, opers))
	{
		std::string todel = text;
		
		//ide nam len o zaciatok a koniec
		//v kazdom operator bude kusok
		//vsetko medzi tym sa zmaze
	}*/
}

//slot

void TabPage::replaceText( std::string what, std::string by)
{
	////TODO
	//delete what text.
	//insert by text
}
void TabPage::getText()//get text from page
{
	//get only text iterator
	std::vector<shared_ptr<CContentStream> > streams;
	page->getContentStreams(streams);
	std::string tmp ="";
	for ( int i =0; i < streams.size(); i++ )
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
			it.next();
		}
	}
//	std::cout << tmp << std::endl; //show new box
}
