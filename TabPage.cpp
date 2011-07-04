#include "debug.h"
#include "TabPage.h"
#include "globalfunctions.h"
#include "typedefs.h"

//created files
#include "insertpagerange.h"
#include "tree.h"
#include "page.h"
#include "bookmark.h"
#include "globalfunctions.h"
#include "utils/types/coordinates.h"
#include <float.h>
#include <vector>
#include <QRgb>
//QT$
#include <QMessageBox>
#include <QRect>
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
#include <QClipboard>
#include "ui_convertpagerange.h"

//PDF
#include <kernel/pdfoperators.h>
#include <kernel/cannotation.h>
#include <typedefs.h>

#include "openpdf.h"

//#include <kernel/carray.h>

//operatory, ktore musim zaklonovat, ked chcem pohnut textom

std::string nameInTextOperators[] = { "w","j","J","M","d","ri","i","gs", "CS","cs", "SC","SCN", "sc","scn", "G","g","RG","rg","k","K","Tc","Tw", "Tz", "TL", "Tf","Tr","Ts","Td","TD","Tm","T*" };

void TabPage::handleBookmark(QTreeWidget * item, int)
{
	_page = _pdf->getPage(((Bookmark *)(item))->getDest());
	redraw();
}

TabPage::TabPage(OpenPdf * parent, QString name) : _name(name), _mode(DefaultMode),_parent(parent)
{
	_pdf = boost::shared_ptr<pdfobjects::CPdf> ( pdfobjects::CPdf::getInstance (name.toAscii().data(), pdfobjects::CPdf::ReadWrite));
	debug::changeDebugLevel(0);
	_page = boost::shared_ptr<pdfobjects::CPage> (_pdf->getPage(1)); //or set from last
	displayparams.rotate = _page->getRotation();
	//displayparams.upsideDown = true;
	// init splash bitmap
	_acceptedType = OpTextName;
	_selected = false;
	_font = NULL;
	_cmts = NULL;
	ui.setupUi(this);
	_labelPage = new DisplayPage();
	_cmts = new Comments();
	_search = new Search(NULL);//neptraia k tomuto mimiokienku
	//_search->show();
	_image = new InsertImage(NULL);
	this->ui.scrollArea->setWidget(_labelPage);	
	//this->ui.displayManipulation->hide();
	QObject::connect(_search, SIGNAL(search(QString,bool)),this,SLOT(search(QString,bool)));
	QObject::connect(_search, SIGNAL(replaceTextSignal(QString,QString)),this,SLOT(replaceText(QString,QString)));
	QObject::connect(this->ui.zoom, SIGNAL(currentIndexChanged(QString)),this,SLOT(zoom(QString)));
	{
		std::string links[] = { ANNOTS(CREATE_ARRAY) };
		for ( int i =0; i < ASupported; i++)
		{
			//			this->ui._annots-> (links[i]);
		}
	}
	_font = new FontWidget(NULL);
	loadFonts(_font);
	//	_font->show();
	connect(_font, SIGNAL(fontInPage(std::string)), this, SLOT(addFontToPage(std::string)));
	connect(_font, SIGNAL(text(PdfOp)), this, SLOT(insertText(PdfOp)));
	connect(_font, SIGNAL(changeTextSignal()), this, SLOT(changeSelectedText()));
	//show text button, hide everything else

	//connections
	//	connect(_cmts,SIGNAL(close()),this,SLOT(closeAnnotDiag()));
	//pridanie anotacie

	//connect(_cmts,SIGNAL(annotation(Annot)),this,SLOT(insertAnnotation(Annot)));
	connect(this,SIGNAL(pdfPosition(float,float,int,int)),_cmts,SLOT(setRectangle(float,float,int,int)));
	//	connect(_cmts,SIGNAL(annotationTextMarkup(Annot)),this,SLOT(setMarkupProperty()));
	connect(this,SIGNAL(parsed(std::vector<float>)),_cmts,SLOT(setPoints(std::vector<float>)));

	connect (ui.previous,SIGNAL(clicked()),this,SLOT(previousPage()));
	connect (ui.next,SIGNAL(clicked()),this,SLOT(nextPage()));
	connect (_labelPage,SIGNAL(MouseClicked(QPoint)),this, SLOT(clicked(QPoint))); //pri selecte sa to disconnectne a nahrasi inym modom
	connect (this,SIGNAL(markPosition(QPoint)),_labelPage, SLOT(markPosition(QPoint))); //pri selecte sa to disconnectne a nahrasi inym modom
	connect (_labelPage,SIGNAL(MouseReleased(QPoint)),this, SLOT(mouseReleased(QPoint))); //pri selecte sa to disconnectne
	connect (_labelPage,SIGNAL(InsertTextSignal(QPoint)),this,SLOT(raiseInsertText(QPoint)));
	connect (_labelPage,SIGNAL(DeleteTextSignal()),this,SLOT(deleteSelectedText()));
	connect (_labelPage,SIGNAL(EraseTextSignal()),this,SLOT(eraseSelectedText()));
	connect (_labelPage,SIGNAL(ChangeTextSignal()),this,SLOT(raiseChangeSelectedText()));

	connect (_labelPage,SIGNAL(InsertImageSignal(QPoint)),this,SLOT(raiseInsertImage(QPoint)));

	connect (_labelPage,SIGNAL(DeleteImageSignal(QPoint)),this,SLOT(deleteImage(QPoint)));

	connect (_labelPage,SIGNAL(AnnotationSignal(QPoint)),this,SLOT(raiseAnnotation(QPoint)));
	connect (_labelPage,SIGNAL(DeleteAnnotationSignal(QPoint)),this,SLOT(deleteAnnotation(QPoint)));
	//connect (_labelPage,SIGNAL(ChangeImageSignal(QPoint)),this,SLOT(raiseChangeImage(QPoint)));
	connect(ui.tree,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(handleBookmark((QTreeWidgetItem *,int))));

	//end of connections
	connect( _image, SIGNAL(insertImage(PdfOp)),this,SLOT(insertImage(PdfOp)));
	connect( _image, SIGNAL(changeImage(PdfOp)),this,SLOT(changeSelectedImage(PdfOp)));

	connect( _cmts,SIGNAL(annotationTextMarkup(Annot)),this,SLOT(insertTextMarkup(Annot)));

	initRevisions();
	
	redraw();
	_dataReady = false;
	//setZoom();
	for ( int i =50; i< 500; i+=50)
	{
		QVariant s(i);
		this->ui.zoom->addItem( s.toString()+" %",s);
	}
	this->ui.zoom->setCurrentIndex(1);
	_dataReady = false;
	getBookMarks();
	createList();
	search("oftwar",true);
	//changeSelectedText();
}
void TabPage::initRevisions()
{
	QStringList list;
	ui.Revision->clear();
	std::string s;
	for ( size_t i = 0; i< _pdf->getRevisionsCount(); i++)
	{
		QVariant q(i);
		std::stringstream ss;
		ss << i;
		ss >> s;
		list.append(("Revision " + s).c_str());
	}
	ui.Revision->addItems(list);
	ui.Revision->setCurrentIndex(list.count()-1);
}
void TabPage::highLightAnnSelected()
{
	_cmts->insertMarkup();
}
void TabPage::raiseSearch()
{
	_search->show();
}
void TabPage::raiseChangeSelectedImage()
{
	_image->setImage( _selectedImage, 72.0f/displayparams.vDpi); //TODO x,y
	BBox b = _selectedImage->getBBox();
	double scaleX = 72.0f/displayparams.vDpi;
	double scaleY = 72.0f/displayparams.hDpi;
	double x = min(b.xleft,b.xright);
	x*=scaleX;
	double y = (float)displayparams.DEFAULT_PAGE_RY;
	y-= (min(b.yleft,b.yright))*scaleY;
	//uz mame bbox
	rotatePdf(_page->getRotation(),x,y,false);
	_image->setPosition(x,y,72.0f/displayparams.vDpi);
	_image->setSize(abs(b.xleft-b.xright), abs(b.xleft-b.xright));
	_image->show();
}

void TabPage::deleteImage(QPoint point)
{
	//zistime, co mame pod pointom;
	double x,y;
	toPdfPos(point.x(),point.y(),x,y);
	Ops ops;
	_page->getObjectsAtPosition(ops,libs::Rectangle(0,0,1000,1000));
	//hladame BI. Dictionary nevymazavame - bez operatora sa to stejne nezobrazi
	int i =0;
	InlineImageOperatorIterator it(ops.back(),false);
	PdfOp op;
	/*if (it.valid())
	{
	op = it.getCurrent();
	shared_ptr<InlineImageCompositePdfOperator> img = boost::dynamic_pointer_cast<InlineImageCompositePdfOperator>(op);
	BBox b = op->getBBox();
	PdfOperator::PdfOperators child;		
	it.next();
	}*/
	if (!it.valid())
		return;
	op = it.getCurrent();
	//mame iba obrazky
	//zmaz len ten, ktore je 'navrchu' -> je v ops posledny?
	op->getContentStream()->deleteOperator(op);
	redraw();
}
void TabPage::raiseInsertImage(QRect rect)
{
	double x = rect.x()*72.0f/displayparams.vDpi,
		y = (float)displayparams.DEFAULT_PAGE_RY-rect.y()*72.0f/displayparams.vDpi;
	rotatePdf(_page->getRotation(),x,y,true);
	_image->setPosition(x,y,72.0f/displayparams.vDpi);
	_image->setSize(rect.width()*72.0f/displayparams.vDpi, rect.height()*72.0f/displayparams.vDpi);
	_image->show();
}
void TabPage::zoom(QString zoomscale)//later with how much pages, if all or not
{
	//odstranit breberky za tym
	zoomscale = zoomscale.remove("%");
	zoomscale = zoomscale.remove(" ");
	float scale = zoomscale.toFloat()/100;
	float dpix=_labelPage->logicalDpiX();
	float dpiy =_labelPage->logicalDpiY();
	displayparams.hDpi = dpix * scale;
	displayparams.vDpi = dpiy * scale;
	this->redraw();
}
void TabPage::fillCoordinates(std::vector<float>& coordinates, float * dim)
{
	TextData::iterator iter = sTextIt;
	float dpix = displayparams.vDpi/72.f;
	float dpiy = displayparams.hDpi/72.f;
	while (true)
	{
		coordinates.push_back(iter->GetPreviousStop()/dpix);
		coordinates.push_back(displayparams.DEFAULT_PAGE_RY - iter->_ymax/dpiy);
		coordinates.push_back(iter->_end/dpix);
		coordinates.push_back(displayparams.DEFAULT_PAGE_RY - iter->_ymax/dpiy); //minima
		coordinates.push_back(iter->_begin/dpix);
		coordinates.push_back(displayparams.DEFAULT_PAGE_RY - iter->_ymin/dpiy);
		coordinates.push_back(iter->_end/dpix);
		coordinates.push_back(displayparams.DEFAULT_PAGE_RY - iter->_ymin/dpiy);
		dim[0] = min(dim[0],iter->GetPreviousStop()/dpix);
		dim[1] = min(dim[1],displayparams.DEFAULT_PAGE_RY - iter->_ymax/dpiy);
		dim[2] = max(dim[2],iter->_end/dpix);
		dim[3] = max(dim[3],displayparams.DEFAULT_PAGE_RY - iter->_ymin/dpiy);
		if ( iter == sTextItEnd )
			break;
		iter++;
	}
}
pdfobjects::IndiRef TabPage::createAppearanceDict(float*dim)
{
	boost::shared_ptr<pdfobjects::CStream> apStream(new CStream());
	apStream->addProperty("Type",*PdfProperty(CNameFactory::getInstance("XObject")));
	apStream->addProperty("SubType",*PdfProperty(CNameFactory::getInstance("Form")));
	apStream->addProperty("FormType",*boost::shared_ptr<CInt>(CIntFactory::getInstance(1)));
	{
		CArray rct;
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(dim[0]))));
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(dim[1]))));
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(dim[2]))));
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(dim[3]))));
		apStream->addProperty("BBox",rct);
	}
	{
		CArray arr;
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(1)));
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(0)));
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(0)));
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(1)));
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(-dim[0])));
		arr.addProperty(*PdfProperty(CRealFactory::getInstance(-dim[1])));
		apStream->addProperty("Matrix",arr);
	}
	{
		//group pre transparency
		boost::shared_ptr<pdfobjects::CDict> group(CDictFactory::getInstance());
		group->addProperty("S",*PdfProperty(CNameFactory::getInstance("Transparency")));
		group->addProperty("Type",*PdfProperty(CNameFactory::getInstance("Group")));
		apStream->addProperty("Group",*group);
	}
	{
		//resource
		boost::shared_ptr<pdfobjects::CDict> resDict(CDictFactory::getInstance());
		boost::shared_ptr<pdfobjects::CDict> grDict(CDictFactory::getInstance());
		boost::shared_ptr<pdfobjects::CDict> g0Dict(CDictFactory::getInstance());

		CArray arr2;
		arr2.addProperty(*PdfProperty(CNameFactory::getInstance("PDF")));
		resDict->addProperty("ProcSet",arr2);

		
		g0Dict->addProperty("Type",*PdfProperty(CNameFactory::getInstance("ExtGState")));

		g0Dict->addProperty("AIS",*PdfProperty(CBoolFactory::getInstance(false)));
		g0Dict->addProperty("BM",*PdfProperty(CNameFactory::getInstance("Multiply")));
		grDict->addProperty("TransGs",*g0Dict);
		resDict->addProperty("ExtGState",*grDict);
		apStream->addProperty("Resources",*resDict);
	}
	{
		CArray rct;
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(0))));
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(0))));
		rct.addProperty(*(PdfProperty(CRealFactory::getInstance(1))));
		apStream->addProperty("Border",rct);
	}
	PdfOperator::Operands operands;
	operands.push_back((PdfProperty(CNameFactory::getInstance("TransGs"))));
	PdfOp op = createOperator("gs",operands);
	apStream->addToBuffer(op);
	operands.clear();

	operands.push_back((PdfProperty(CRealFactory::getInstance(1.0f))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(1.0f))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(0.0f))));
	op = createOperator("rg",operands);
	apStream->addToBuffer(op);

	operands.clear();
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[0]))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[1]))));
	op = createOperator("m",operands);
	apStream->addToBuffer(op);

	operands.clear();
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[2]))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[1]))));
	op = createOperator("l",operands);
	apStream->addToBuffer(op);

	operands.clear();
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[1]))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[3]))));
	op = createOperator("l",operands);
	apStream->addToBuffer(op);

	operands.clear();
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[0]))));
	operands.push_back((PdfProperty(CRealFactory::getInstance(dim[3]))));
	op=createOperator("l",operands);
	apStream->addToBuffer(op);

	operands.clear();
	apStream->addToBuffer(createOperator("h",operands));
	apStream->addToBuffer(createOperator("f",operands));

	//std::string text("173.11566 660.86603 m 181.534 660.86603 l 181.534 639.63373 l 164.869 639.63373 l h f");
	apStream->validate();
	return _pdf->addIndirectProperty(apStream);//hack
}
void TabPage::insertTextMarkup(Annot annot)
{
	//for all selected text
	if (!_selected)
		return;
	insertAnnotation(annot);
	_page->getAllAnnotations(_annots);
	annot = _annots.back();//posledny pridany, na nom budeme pachat zmeny
	std::vector<float> coordinates;
	float dim[4] = {FLT_MAX,FLT_MAX,0,0};
	fillCoordinates(coordinates, dim);
	IndiRef indi = createAppearanceDict(dim);
	{
		CArray rect;
		for ( int i = 0; i < 4;i++)
			rect.addProperty(*(PdfProperty(CRealFactory::getInstance(dim[i]))));
		annot->getDictionary()->setProperty("Rect",rect); 
	}
	//vytvor ApearanceStream
	//boost::shared_ptr<pdfobjects::CDict> apDict(pdfobjects::CDictFactory::getInstance());

	CArray points;
	for ( int i = 0; i < coordinates.size();i++)
		points.addProperty(*(PdfProperty(CRealFactory::getInstance(coordinates[i]))));
	annot->getDictionary()->addProperty("QuadPoints",points);
	boost::shared_ptr<pdfobjects::CDict> nDict(CDictFactory::getInstance());
	
	PdfProperty prop(pdfobjects::CRefFactory::getInstance(indi));
	nDict->addProperty("N",*prop);
	annot->getDictionary()->addProperty("AP",*nDict);
	std::string m;
	//apDict->getStringRepresentation(m);
	//annot->getDictionary()->getProperty("AP")->getStringRepresentation(m);
	//fprintf(stderr,"$s\n",m.c_str());

	//najsi mi vsetky textove operatory a z nich vycuvam svojich 8 cisel, pravepodobne z gxfconfu a pozicie offset operatorov ( vektory )
	//TODO treba osetrit na to, ak chceme zvyraznit len jednu cast textoveho operatora
	redraw();
}
void TabPage::closeAnnotDiag()
{
	delete _cmts; //cleanup za anotacie
	_cmts = NULL;
	//_mode = ModeSelectText;
	//disconnect all?
}
void TabPage::insertAnnotation(Annot a)
{
	//vlozime do aktualnej stranky
	//pre kazdu vysvietenu
	//boundigbox pre vysvieteny text - ak ziady nie je, tak bod
	_page->addAnnotation(a);

//	std::string m;
	//_pdf->addIndirectProperty(a->getDictionary())
//	_annots[0]->getDictionary()->setProperty("AP",a->getDictionary()->getpo);
//	_pdf->getIndirectProperty(i)->getStringRepresentation(m);
	//setFromSplash();
}
void TabPage::deleteAnnotation(QPoint point)
{
	_page->getAllAnnotations(_annots);
	for (int i =0; i < _annots.size(); i++)
	{
		shared_ptr<CArray> rect;
		PdfProperty prop = _annots[i]->getDictionary()->getProperty("Rect");
		rect = pdfobjects::IProperty::getSmartCObjectPtr<CArray>(prop);
		float x1,x2,y1,y2;
		x1 = utils::getSimpleValueFromArray<CReal>(rect,0);
		y1 = utils::getSimpleValueFromArray<CReal>(rect,1);
		x2 = utils::getSimpleValueFromArray<CReal>(rect,2);
		y2 = utils::getSimpleValueFromArray<CReal>(rect,3);
		//dostat annotecny rectangle
		BBox b(x1,y1,x2,y2);
		QRect convertedRect = getRectangle(b);
		if (convertedRect.contains(point))
		{
			_page->delAnnotation(_annots[i]);
			redraw();
			return;
		}
	}
	printf("Anotation was not deleted");
}
void TabPage::raiseChangeSelectedText()
{
	QString s,s1,s2,s3;
	TextData::iterator it = sTextIt;
	sTextIt->split(s1,s,s3);
	while (it != sTextItEnd)
	{//TODO medzera
		it++;
		//musime vratane posledneho
		if (it != sTextItEnd)
			s+=it->_text;
		else
		{
			it->split(s1,s2,s3);
			s+=s2;
		}
	}//TODO set font heigth
	_font->setHeight(sTextIt->_op->getFontHeight());
	_font->setText(s);
	_font->setChange();
}
void TabPage::waitForPosition()
{
	_mode = ModeEmitPosition;
}
void TabPage::raiseAnnotation(QPoint point)
{
	//_mode = ModeEmitPosition;
	double x,y;
	displayparams.convertPixmapPosToPdfPos(point.x(), point.y(),x,y);
	float xdif = displayparams.vDpi/72;
	float ydif = displayparams.hDpi/72;
	_cmts->setRectangle(x/xdif,y/ydif,30,30);//pre zvysok sa to vyhodi a nahradi sadou anotacii	
	_cmts->show();
}

void TabPage::createList()
{
	_textList.clear();
	//get all pdf text operators in list
	Ops ops;
	libs::Rectangle rect(0,0,FLT_MAX,FLT_MAX);
	_page->getObjectsAtPosition( ops, rect);
	//choose just test iterator
	Ops::iterator it = ops.begin();
	//float fontSize = 0;
	while ( it != ops.end())
	{
		std::string n;
		(*it)->getOperatorName(n);
		if (!typeChecker.isType(OpTextName,n))
		{
			it++;
			continue;
		}
		OperatorData data(*it);
		_textList.push_back(data);
		it++;
	}
	//sort list
	_textList.sort();
	_selected = false;
	sTextIt = sTextItEnd = sTextMarker = _textList.begin();
}
void TabPage::raiseInsertText(QPoint point)
{
	double x,y;
	displayparams.convertPixmapPosToPdfPos(point.x(), point.y(), x, y); //TODO upravit aby to neblblo
	//toPdfPos(point.x(),point.y(),x,y);
	////do povodneho stavu .. hotfix
	//x/=displayparams.vDpi/72;
	//y/=displayparams.hDpi/72;
	_font->setPosition(x,y);
	_font->setInsert();
}
void TabPage::UnSetTextSelect()
{
	_mode = DefaultMode;
	if(_font)
		delete _font;
	_font = NULL;
}
void TabPage::clicked(QPoint point) //resp. pressed, u select textu to znamena, ze sa vyberie prvy operator
{
	switch (_parent->getMode())
	{
	case ModeInsertText:
		{
			//show position
			emit markPosition(point);
			raiseInsertText(point); //TODO angle
			break;
		}
	case ModeEmitPosition: //pre anotacie
		{
			if (_mousePos.x()<0)
				_mousePos = _labelPage->convertCoord(point);
			else
			{
				double a = min(_mousePos.x(),point.x()),b = min(_mousePos.y(),point.y());
				rotatePdf(_page->getRotation(),a,b,true);
				emit pdfPosition(a,b,abs(_mousePos.x()-point.x()),abs(_mousePos.y()-point.y()));
				_mousePos = QPoint(-1,-1);
			}
			break;
		}
	case ModeSelectImage:
		{//vyber nejaky BI operator a daj ho to vybranych
			Ops ops;
			_page->getObjectsAtPosition(ops,libs::Point(point.x(),point.y()));
			while(!ops.empty())
			{
				std::string name;
				ops.back()->getOperatorName(name);
				if (typeChecker.isType(OpImageName,name))
				{
					_selected = true;
					_selectedImage = ops.back();
					BBox bbox = _selectedImage->getBBox();
					rotatePdf(_page->getRotation(),bbox.xleft, bbox.yleft,false);
					rotatePdf(_page->getRotation(),bbox.xright, bbox.yright,false);
					QRect rect(min(bbox.xleft,bbox.xright), min(bbox.yleft, bbox.yright), abs(bbox.xleft-bbox.xright), abs(bbox.yleft-bbox.yright));
					_labelPage->drawRectangle(rect);
					break;
				}
				ops.pop_back();
			}
			break;
		}
	case ModeSelectText:
		{
			highlightText(point); //nesprav nic, pretoze to bude robit mouseMove
			break;
		}
	case ModeImagePart:
	case ModeInsertImage:
		{
			if (!_dataReady)
			{
				_mousePos = point;
				_dataReady = true;
				return;
			}
			break;
		}
	case ModeExtractImage:
		{
			Ops ops;
			_page->getObjectsAtPosition(ops,libs::Point(point.x(),point.y()));
			while (!ops.empty())
			{
				std::string name;
				ops.back()->getOperatorName(name);
				if (typeChecker.isType(OpImageName,name))
					break;
				ops.pop_back();
			}
			if (ops.empty())
				return;
			PdfOperator::Operands op;
			ops.back()->getParameters(op);
	//		image_dict.addProperty ("CS", CName ("RGB"));
	//image_dict.addProperty ("BPC", CInt (8)); //
			shared_ptr<pdfobjects::CInlineImage> inIm =  boost::dynamic_pointer_cast<pdfobjects::CInlineImage>(op.back());
			boost::shared_ptr<IProperty> prop = inIm->getProperty("CS");
			std::string cs = utils::getNameFromIProperty(prop);
			if (cs!="RGB")
			{
				QMessageBox::warning(this, "Unable to export inline image","Unimplented color space", QMessageBox::Ok,QMessageBox::Ok);
				return; 
			}
			int bpp = utils::getIntFromIProperty(inIm->getProperty("BPC"));
			if(bpp!=8)
			{
				QMessageBox::warning(this, "Unable to export inline image","Wrong depth", QMessageBox::Ok,QMessageBox::Ok);
				return;
			}
			QImage image(QSize(inIm->width(),inIm->height()),QImage::Format_ARGB32);
			CStream::Buffer buffer = inIm->getBuffer();
			int index = 0;

			for ( int h=0; h<inIm->height();h++)
				for ( int w=0; w<inIm->width();w++)
				{ 
					QColor c((unsigned char)buffer[index],(unsigned char)buffer[index+1],(unsigned char)buffer[index+2],0);
					image.setPixel(w,h,c.rgb());
					index+=3;
				}

			QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), "", tr("Images (*.png *.jpg *.bmp))"));
			if (fileName.isEmpty())
				return;
			image.save(fileName);
			break;
		}
	case ImageMode:
		{
			//show only picture, all picture
			//get only operator
			double px, py;
			//convert
			toPdfPos(point.x(),point.y(), px, py);
			//find operattdisplayparamsors
			Ops ops;
			_page->getObjectsAtPosition(ops, libs::Point(px,py));
			//vsetky tieto objekty vymalujeme TODO zistit orientaciu
			for ( size_t i =0; i < ops.size(); i++)
			{
				//ukaz len povolene typy
				std::string s;
				ops[i]->getOperatorName(s); 
				if (!typeChecker.isType(OpImageName,s))
				{
					QClipboard *clipBoard = QApplication::clipboard();
					clipBoard->setImage(_labelPage->getImage().copy(getRectangle(ops[i])));
					break;
				}
			}
		}
	case ModeOperatorSelect:
	default: //ukazuje celeho operatora
		{
			_labelPage->unsetImg();
			showClicked(point.x(),point.y());//zmenit 
		}
	}
}

void TabPage::deleteSelectedImage()
{
	if (!_selected)
		return;
	std::string s;
	_selectedImage->getOperatorName(s);
	_selectedImage->getContentStream()->deleteOperator(_selectedImage);
	_selectedImage = PdfOp(); //empty
	_selected = false;
	redraw();
}
void TabPage::mouseReleased(QPoint point) //nesprav nic, pretoze to bude robit mouseMove
{
	switch (_parent->getMode())
	{
	case ModeSelectText:
		{
			highlightText(point);
			break;
		}
	case ModeInsertImage:
		{
			QRect r(min(_mousePos.x(),point.x()),min(_mousePos.y(),point.y()), 
				abs(_mousePos.x()-point.x()), abs(_mousePos.y()-point.y()));
			raiseInsertImage(r);
			_dataReady=false;
			break;
		}
	case ModeImagePart: //data ready
		{
			//copy to clipboard
			QClipboard *clipBoard = QApplication::clipboard();
			QRect rect(min(point.x(),_mousePos.x()), min(point.y(),_mousePos.y()), abs(point.x()-_mousePos.x()),abs(point.y()-_mousePos.y()));
			clipBoard->setImage(_labelPage->getImage().copy(rect));
			return;
		}
	default:
	//	highlight();//TODO ZMAZST
		break;
	}
	_dataReady = false;
}
void TabPage::highLightBegin(int x, int y) //nesprav nic, pretoze to bude robit mouseMove
{
	double px=x,py=y;
	//najdi prvy operator, na ktory bolo kliknute
	Ops ops;
	getAtPosition(ops,px,py); //zaplnili sme operator
	//zistime, ze je to text
	std::string n;
	if (ops.empty())
		return;
	TextOperatorIterator it(ops.back(),false);
	if (!it.valid())
		return; //zoberieme iba posledny, viditelny, ak su na sebe
	_dataReady = true;
	DEBUGLINE("Operator found");
	sTextIt = _textList.begin();
	setTextData(sTextIt,_textList.end(),it.getCurrent());
	sTextIt->clear();
	sTextIt->setBegin(x);//zarovnane na pismenko
	sTextMarker = sTextItEnd = sTextIt;
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

void TabPage::highlightText(QPoint point) //tu mame convertle  x,y, co sa tyka ser space
{
	double px = point.x(), py = point.y();
	rotatePdf(_page->getRotation(),px,py,true);
	point.setX(px); point.setY(py);
	if (!_dataReady) //prvykrat, co sme dotkli nejakeho operatora
	{
		_mousePos = point;
		_labelPage->unsetImg();
		highLightBegin(point.x(),point.y()); //oznaci zaciatok
		return;
	}//sTextIt
	Ops ops;
	getAtPosition(ops,point.x(),point.y());
	if (ops.empty())
		return;
	TextOperatorIterator it(ops.back(),false);
	PdfOp o = it.getCurrent();
	if (it.valid())
		o = it.getCurrent();
	OperatorData s(o);
	double xBegin;
	double xEnd;
	TextData::iterator move = sTextMarker;
	move->clear();
	if ( *move < s)
	{
		while (move->_op!=s._op)
		{
			move++;
			move->clear();
		}
	}
	else
	{
		while (move->_op!=s._op)
		{
			move--;
			move->clear();
		}
	}
	if (*move < *sTextMarker)
	{
		sTextIt = move;
		sTextIt->setBegin(min(point.x(),_mousePos.x()));
		sTextItEnd = sTextMarker;
		sTextItEnd->setEnd(max(_mousePos.x(),point.x()));
	}
	else if (move == sTextMarker)
	{
		sTextIt = sTextItEnd = sTextMarker;
		sTextIt->setBegin(min(point.x(),_mousePos.x()));
		sTextIt->setEnd(max(point.x(),_mousePos.x()));
	}
	else
	{
		sTextIt = sTextMarker;
		sTextIt->setBegin(_mousePos.x());
		sTextItEnd = move;
		sTextItEnd->setEnd(point.x());
	}

	_selected =  true;
	highlight();
	//_dataReady = false;
}

void TabPage::highlight()
{
	if (!_selected)
		return;
	QColor color(255,0,255,50);
	TextData::iterator first = sTextIt;
	TextData::iterator last = sTextItEnd; 
	bool forw = (*sTextIt) < (*sTextItEnd);
	double x1,x2,y1,y2;
	QVector<QRect> region;
	assert(region.isEmpty());
	while (true)
	{
		x1= first->GetPreviousStop();
		y1 = first->_ymin;
		rotatePdf(_page->getRotation(),x1,y1,false);
		x2 = first->GetNextStop();
		y2 = first->_ymax;
		rotatePdf(_page->getRotation(),x2,y2,false);
		QRect r(min(x1,x2),min(y1,y2), fabs(x2-x1),fabs(y1-y2));
		if (r.width() > first->_charSpace)
			region.append(r);
		if (first == sTextItEnd)
		{
			this->ui.scrollArea->ensureVisible(x2,y2);
			break;
		}
		inDirection(first, forw);
	}
	_labelPage->fillRect( region, color );
}

//zatial len v ramci jednej stranky
void TabPage::moveText(int difX, int difY) //on mouse event, called on mouse realease
{
	//for each selected operator, move it accrding to position
	if (!_selected) //spravne nastavene 
		return;
	//ostran z listu
	TextData::iterator first = sTextIt; 
	TextData::iterator last = sTextItEnd; 
	bool forw = (*sTextIt) < (*sTextItEnd);
	if (!forw)
	{
		first = sTextItEnd;
		last = sTextIt;
	}
	QString s1,s2,s3,s4;
	//problem je, e to moze by tiez cast - jedna sa len o prve a posledne. To zmazeme, ponechame cast a insterime znova
	_selected = false;
	if (first==last)
	{ //sprav to same len s jednym iteratorom
		//treba ho rozdelit na niekolko operatorov. Jelikoz je to jeden, tak na tri
		double b = first->_begin;
		double e = first->_end;
		first->replaceAllText(s1); //netreba nic mazat, je prave jedno
		createAddMoveString(first->_op,e+difX,first->_ymax+difY,s3);
		createAddMoveString(first->_op,b+difX,first->_ymax+difY,s2);
		return;
	}
	first->split(s1,s2,s3);	//zajima nas iba s2 -> od begin po end, s3 bude prazdna
	float x = first->_begin , y=first->_ymax;
	first->replaceAllText(s1); //tuto sa to pomeni
	last->replaceAllText(s3);
	last->split(s1,s3,s4); //last je end
	createAddMoveString(first->_op,last->_begin+difX,last->_ymax+y,s2);
	while (first!=last)
	{
		PdfOp o = FontWidget::createTranslationTd(x+difX, y+difY);
		insertBefore(o, last->_op);
		last--;
		x = last->_begin;
		y = last->_ymax;
	}
	createAddMoveString(first->_op,first->_begin+difX,first->_ymax+y,s2);

	_textList.sort();
}
void TabPage::insertBefore(PdfOp op, PdfOp before)
{
	PdfOp clone = before->clone();
	before->getContentStream()->replaceOperator(before,op);	
	op->getContentStream()->insertOperator(op,clone);
}
void TabPage::createAddMoveString(PdfOp bef, double x, double y, QString name)
{
	PdfOperator::Operands ops;
	ops.push_back(boost::shared_ptr<IProperty> (new CString(name.toAscii().data())));
	PdfOp p = createOperator("tj",ops);
	bef->getContentStream()->insertOperator(bef,p);
	OperatorData d(p);
	_textList.push_back(d);
	PdfOp op = FontWidget::createTranslationTd(x,y);
	bef->getContentStream()->insertOperator(bef,p);	
}
void TabPage::inDirection(TextData::iterator& iter, bool forward)
{
	if (forward)
		iter++;
	else 
		iter--;
}
void TabPage::getAtPosition(Ops& ops, int x, int y )
{
	double px=x, py=y;
	rotatePdf(_page->getRotation(),px,py,true);
	//find operattors
	_page->getObjectsAtPosition(ops, libs::Point(px,py));
}
void TabPage::toPdfPos(int x, int y, double & x1, double &y1)
{
	displayparams.convertPixmapPosToPdfPos(x, y, x1, y1); //TODO upravit aby to neblblo
	//x1 = x1*displayparams.vDpi/72; //pretoze BBoxy su takto spravene
	//y1 = y1*displayparams.hDpi/72;
	//upside down
	//y1 = DisplayParams::DEFAULT_PAGE_RY*displayparams.vDpi/72 -y1;
	//bboxes su v pdf stejne podla pdicka
	//x1*= displayparams.vDpi/72; //pretoze BBoxy su takto spravene
	//y1*= displayparams.hDpi/72;
}
void TabPage::toPixmapPos(double x1, double y1, int & x, int &y)
{
	double x2, y2;
	displayparams.convertPdfPosToPixmapPos(x1, y1, x2, y2);
	x1 *= displayparams.hDpi/72;
	y1 *= displayparams.vDpi/72;
	x = x2;
	y = y2;
}

void TabPage::rotatePdf(int angle, double& x,double& y, bool fromPixmap) //dostanem pozcie ake su v pdfku
{
	angle = (angle + 360)%360;
	double par[] = { displayparams.vDpi/72.0f, displayparams.hDpi/72.0f};
	double resx = DisplayParams::DEFAULT_PAGE_RX*par[0];
	double resy = DisplayParams::DEFAULT_PAGE_RY*par[1];

	::rotate(angle,resx,resy);
	if (!fromPixmap)
		::rotate(angle,x,y);
	switch (angle/90)
	{
	case 1: //musime menit X os
		x = fabs(resx) - fabs(x);
		break;
	case 2:
		x = fabs(resx) - fabs(x);
		y = fabs(resy) - fabs(y);
		break;
	case 3:
		y = fabs(resy) - fabs(y);
		break;
	default:
		break;
	}
	if (fromPixmap)
		::rotate(angle,x,y);
	x = fabs(x);
	y = fabs(y);
}
void TabPage::showClicked(int x, int y)
{
	double px=x, py=y;
	//convert
	//toPdfPos(x,y, px, py);
	rotatePdf(_page->getRotation(),px,py,true);

	Ops ops;
	_page->getObjectsAtPosition(ops, libs::Point(px,py));

	for ( size_t i =0; i < ops.size(); i++)
	{
		//ukaz len povolene typy
#if _DEBUG 
		std::string s;
		ops[i]->getOperatorName(s);
		if (!typeChecker.isType(OpTextName,s))
			continue;
		std::wstring w;
		shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(ops[i]);
		txt->getFontText(w);
#endif
		libs::Rectangle b = ops[i]->getBBox();

		QColor color(55, 55, 200,100);
		rotatePdf(_page->getRotation(),b.xleft,b.yleft,false);
		rotatePdf(_page->getRotation(),b.xright,b.yright,false);
		_labelPage->fillRect( b.xleft, b.yleft, b.xright, b.yright, color );
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
	ss << _pdf->getPageCount();
	std::string s2;
	ss >> s2;
	ss.clear();
	ss << _page->getPagePosition();
	std::string s1;
	ss >> s1;

	this->ui.pageInfo->setText( (s1 + " / " +s2).c_str() );
}
void TabPage::pageUp()
{
	int pos = _pdf->getPagePosition(_page);
	this->_pdf->removePage(pos);
	this->_pdf->insertPage(_page,pos-2);
	updatePageInfoBar();
}
void TabPage::pageDown()
{
	int pos = _pdf->getPagePosition(_page);
	this->_pdf->removePage(pos);
	this->_pdf->insertPage(_page,pos);
	updatePageInfoBar();
}
bool TabPage::previousPage()
{
	if (_pdf->getPagePosition(_page) == 1)
		return false;
	_page = _pdf->getPrevPage(_page);
	displayparams.rotate = _page->getRotation();
	this->redraw();
	return true;
}
bool TabPage::nextPage()
{
	if (_pdf->getPagePosition(_page) == _pdf->getPageCount())
		return false;
	_page = _pdf->getNextPage(_page);
	displayparams.rotate = _page->getRotation();
	this->redraw();
	return true;
}
void TabPage::getBookMarks()
{
	//LATER, TODO, zistit, na aku stranku sa odkazuju, potazne odsek
	//ket from XREGWritel all GoLink
	//na nejak on show()
	std::vector<shared_ptr<CDict> > outline;
	_pdf->getOutlines(outline);
	std::vector<shared_ptr<CDict> > dicts;
	printf( " bookmarks %d\n",outline.size());
	for (size_t i =0; i< outline.size(); i++)
	{
		QTreeWidgetItem * b = new QTreeWidgetItem(); 
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
//void TabPage::removeObjects() //vsetko, co je vo working
//{
//	for (size_t i = 0; i < workingOpSet.size(); i++ )
//	{
//		PdfOperator::Iterator it = PdfOperator::getIterator(workingOpSet[i]);
//		workingOpSet[i]->getContentStream()->deleteOperator(it,true);
//	}
//	workingOpSet.clear();
//}
void TabPage::changeSelectedImage(PdfOp op)
{
	if (!_selected)
		return;
	_selectedImage->getContentStream()->replaceOperator(_selectedImage,op);
	_selected = false;
	redraw();
}
void TabPage::insertImage(PdfOp op) //positions
{
	Ops ops;
	ops.push_back(op);
	_page->addContentStreamToBack(ops);
	redraw();
}
void TabPage::insertPageRangeFromExisting()
{ 
	QString s = QFileDialog::getOpenFileName(this, tr("Open File"),".",tr("Pdf files (*.pdf)"));
	if (s == NULL)
		return;
	boost::shared_ptr<pdfobjects::CPdf> pdf2 = boost::shared_ptr<pdfobjects::CPdf> ( 
		pdfobjects::CPdf::getInstance (s.toAscii().data(), pdfobjects::CPdf::ReadOnly));
	_page = _pdf->insertPage(pdf2->getPage(1),1);
}
void TabPage::deletePage()
{
	//removes actual page and displays the one after
	if (_pdf->getPageCount() <= 1)
		return;
	size_t i = _pdf->getPagePosition(_page);
	_pdf->removePage(i);
	if ( i > _pdf->getPageCount() ) //if removing last page..
		i =_pdf->getPageCount() ;
	_page = _pdf->getPage(i);
	redraw();
}
void TabPage::redraw()
{
	SplashColor paperColor;
	paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
	SplashOutputDev splash (splashModeBGR8, 4, gFalse, paperColor);

	/*Ops oTest;
	page->getObjectsAtPosition(oTest,libs::Point(229,619));*/
	// display it = create internal splash bitmap
	_page->displayPage(splash, displayparams);
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
	_labelPage->setImage(image);
	//image.save("mytest.bmp","BMP");
	//this->ui.label->adjustSize();
	updatePageInfoBar();
	showAnnotation();
	createList();//TODO only in right mode
}
void TabPage::wheelEvent( QWheelEvent * event ) //non-continuous mode
{
	if (event->delta() > 0 )
	{
		//wheeling forward
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
void TabPage::saveEncoded()
{
#ifdef _DEBUG
	float dim[4] = {FLT_MAX,FLT_MAX,0,0};
	std::string out;
	//prop->getStringRepresentation(out);
	_pdf->saveDecoded("decoded");
	return;
#endif
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("PdfFile Decoded (*.decoded)"));
	_pdf->saveDecoded(fileName.toAscii().data());
}
void TabPage::save() //revision je inde
{
	_pdf->save(false);
	initRevisions();
}
void TabPage::saveAs()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("PdfFile (*.pdf)"));
	savePdf(fileName.toAscii().data());
	initRevisions();
}
void TabPage::savePdf(char * name)
{
	if (name == NULL)
	{
		_pdf->save();
		initRevisions();
		return;
	}
	_pdf->saveChangesToNew(name);
	initRevisions();
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
	//std::cout << "Adding revision ";
	std::stringstream ss;
	//std::cout << i << " " << pdf->getRevisionsCount() << std::endl;
	assert( (size_t)i < _pdf->getRevisionsCount() );
	ss << i;
	std::string s;
	ss >> s;
	ui.Revision->addItem(QString(("Revision " + s).c_str()));
	ui.Revision->setCurrentIndex(i-1);
}

///--------------------------------PRIVATE SLOTS----------------

void TabPage::initRevision(int  revision) //snad su revizie od nuly:-/
{
	size_t pos = _page->getPagePosition();
	_pdf->changeRevision(revision);
	if (pos > _pdf->getPageCount())
		pos = _pdf->getPageCount();
	redraw();
}

void TabPage::rotate(int angle) //rotovanie pages
{
	//std::cout << "Rotating" << angle << std::endl;
	_page->setRotation(_page->getRotation()+angle);
	displayparams.rotate = _page->getRotation();

	redraw();
	createList();
}
void TabPage::commitRevision()
{
	//save revision to pdf
	this->_pdf->save(true);
	if (_pdf->getRevisionsCount() == (size_t)this->ui.Revision->count()+1)
		addRevision(this->ui.Revision->count());	
	else
	{ 
		//std::cout << " Not changed" << pdf->getRevisionsCount() << std::endl;
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
	_pdf->clone(f);
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
	if (_pdf->getRevisionsCount() == 1)
		return;
	size_t pos = _pdf->getPagePosition( _page );
	_pdf->changeRevision(_pdf->getRevisionsCount()-1);
	_pdf->save(false);
	//save to file that is tmpdte
	FILE * f;
	std::string s(_name.toAscii().data());
	s+= ".tmp";
	f = fopen(s.c_str(), "w");
	if (!f)
	{
		QMessageBox::warning(this, tr("Not able to revert"),tr("There was problem to open file for writing"), QMessageBox::Ok);
	}
	//TODO skontrolovat, ci sa zmenia zamky na subore
	if (pos >  _pdf->getPageCount())
		pos = _pdf->getPageCount();
	_page = _pdf->getPage(pos);
	addRevision();
}
void TabPage::insertRange()
{
	//opens file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open pdf file"),".",tr("PdfFiles (*.pdf)"));
	if (fileName == NULL)
		return;//cancel pressed
	InsertPageRange range(_pdf,_pdf->getPagePosition(_page),this,fileName);
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
	_pdf->insertPage(pageToAdd, _pdf->getPagePosition(_page));//insert after
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
	for (size_t pos = 1; pos <= _pdf->getPageCount(); ++pos) {

		// Use the painter to draw on the page.

		// display it = create internal splash bitmap
		_pdf->getPage(pos)->displayPage(splash, displayparams);
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
		if (pos != _pdf->getPageCount())
			printer.newPage();
	}
	delete[] p;
	//sends to printer
	painter.end();
}

void TabPage::draw() //change mode to drawing
{
	//	this->ui.content->_beginDraw();
	_mode = DrawMode;
}
//na kazdej stranke mozu byt anotacie, po kliknuti na ne vyskoci pop-up alebo sa inak spravi akcia
//page bude vediet o interaktovnyh miestach -> kvoli mouseMove
void TabPage::showAnnotation()
{
	//akonahle sa zmeni stranka, upozornim page na to ze tam moze mat anotacie
	//dostan oblasti anotacii z pdf
	_page->getAllAnnotations(_annots);
	//v page nastav vsetky aktivne miesta - text only
	for(size_t i =0; i< _annots.size(); i++)
	{
		std::vector<std::string> names; //subtype == text
		_annots[i]->getDictionary()->getAllPropertyNames(names);
		PdfProperty p = _annots[i]->getDictionary()->getProperty("Subtype");
		std::string type = utils::getNameFromIProperty(p);
		if (type == "Popup")
		{
			_page->delAnnotation(_annots[i]);
			redraw();
			return;
		}
		std::string m;
		/*for (int a = 0; a< names.size();a++)
		{
		_annots[i]->getDictionary()->getProperty(names[a])->getStringRepresentation(m);
		}*/
		_annots[i]->getDictionary()->getStringRepresentation(m);
		if ( type != "Text")
			continue;
		//printf("%s\n",m.c_str());
		/*	PdfProperty prop = _annots[i]->getDictionary()->getProperty("Subtype");
		if (pdfobjects::IProperty::getSmartCObjectPtr(*/
		//pre vsetky mi napis do konzole ich properties
		shared_ptr<CArray> rect;
		PdfProperty prop = _annots[i]->getDictionary()->getProperty("Rect");
		rect = pdfobjects::IProperty::getSmartCObjectPtr<CArray>(prop);
		float x1,x2,y1,y2;
		x1 = utils::getSimpleValueFromArray<CReal>(rect,0);
		y1 = utils::getSimpleValueFromArray<CReal>(rect,1);
		x2 = utils::getSimpleValueFromArray<CReal>(rect,2);
		y2 = utils::getSimpleValueFromArray<CReal>(rect,3);
		//dostat annotecny rectangle
		BBox b(x1,y1,x2,y2);
		QRect convertedRect = getRectangle(b);
		_labelPage->addPlace(convertedRect); //teraz vie o vsetkych miestach
	}
}

void TabPage::delAnnot(int i) //page to u seba upravi, aby ID zodpovedali
{
	_page->delAnnotation(_annots[i]);
	_annots[i] = _annots.back();
	_annots.pop_back();
}
QRect TabPage::getRectangle(BBox b)
{
	double x1,x2,y1,y2;
	//TODO tot snad ani nemusi fungovat...check!
	displayparams.convertPdfPosToPixmapPos( b.xleft, b.yleft, x1,y1);
	displayparams.convertPdfPosToPixmapPos( b.xright, b.yright, x2,y2);
	QRect r(QPoint(min(x1,x2),max(y1,y2)),QPoint(max(x1,x2),min(y1,y2)));
	return r;
}

void TabPage::loadFonts(FontWidget* fontWidget)
{
	//dostanme vsetky fontu, ktore su priamov pdf. bohuzial musime cez vsetky pages
	CPage::FontList fontList;

	CPageFonts::SystemFontList flist = CPageFonts::getSystemFonts();
	for ( CPageFonts::SystemFontList::iterator i = flist.begin(); i != flist.end(); i++ )
	{
		fontWidget->addFont(*i,*i); //TODO zrusit a pridat do fontu, ktory si to moze naloadovat sam
	}
}
void TabPage::insertText( PdfOp op )
{
	Ops ops;
	ops.push_back(op);
	_page->addContentStreamToBack(ops);
	TextOperatorIterator iter(op);assert(iter.valid());
	createList();//TODO zoptimalizovat, nsert ba jedneho prvku
	redraw();
}

//slot
void TabPage::changeSelectedText() //vsetko zosane na svojom mieste, akurat sa pridaju 
{
	assert(_selected);
	float corr = displayparams.vDpi/72;
	float h = sTextIt->_op->getFontHeight();
	float y = displayparams.DEFAULT_PAGE_RY - (sTextIt->_ymin+h)/corr; //TODO toto nemusi byt vyska BBoxu
	float pos = sTextIt->GetPreviousStop();
	_font->setPosition(pos/corr,y); //pretoze toto je v default user space
	//rozdelime na dva pripady - pokial je to roznake a pokial je zaciatok erozny od konca
	if ( sTextIt==sTextItEnd ) //TODO co ak je s3 prazdne? -> Compact?:)
	{
		eraseSelectedText();
		_font->setInsert();
		_font->apply(); //musi tu byt apply, pretoze sa moze zmenit farba - potrebujeme Q operator
		_font->close();
		redraw();
		createList();
		return;
	}
	//stale ame v operatoroch ulozene suradnice
	std::vector<PdfOp> operators;
	//prvy je vzdy stejny,ziadne Td
	PdfOperator::Operands operands;
	QString s1,s2,s3;
	operands.clear();
	TextData::iterator it = sTextIt;
	it->split(s1,s2,s3);
	float dist = it->GetPreviousStop() - it->_origX - sTextItEnd->_charSpace;
	dist*= corr;
	float dx=sTextIt->GetPreviousStop() - sTextIt->_origX;
	float dy=0;
	dx*=corr;
	assert(s3==""); //namiesto prveho TD bude 
	{
		operands.clear();
		operands.push_back(PdfProperty(CStringFactory::getInstance(s2.toAscii().data())));
		operators.push_back(createOperator("Tj",operands));
	}//spracovane prve
	float lastX = pos;
	float lastY = it->_ymin;
	while(it!=sTextItEnd) //spravi vratane poslendeho
	{
		it++;
		//td
		operands.clear();
		dist = it->_origX - lastX;
		dist /= corr;
		operands.push_back(PdfProperty(CRealFactory::getInstance(dist)));
		dist = it->_ymin - lastY;
		dist /= corr;
		operands.push_back(PdfProperty(CRealFactory::getInstance(dist)));
		operators.push_back(createOperator("Td",operands));
		//tj
		operands.clear();
		QString dummy1,dummy2, s = it->_text;
		if ( it == sTextItEnd )
			it->split(dummy1,s,dummy2);
		operands.push_back(PdfProperty(CStringFactory::getInstance(s.toAscii().data())));
		operators.push_back(createOperator("Tj",operands));
		lastX = it->_origX;
		lastY = it->_ymin;
	}
	//posledne zaverecne TD kvoli moznemu poslednemu rozbitiu polsendeho operatora
	{
		sTextItEnd->split(s1,s2,s3);
		assert(s1=="");
		operands.clear();
		dist = lastX - sTextItEnd->GetPreviousStop()-sTextItEnd->_charSpace;
		dist /= corr;
		operands.push_back(PdfProperty(CRealFactory::getInstance(dist)));
		operands.push_back(PdfProperty(CRealFactory::getInstance(0)));
		operators.push_back(createOperator("Td",operands));
	}
	eraseSelectedText();
	_font->createFromMulti(operators);
	//vsetky ostatne Tj uchovame + musime zachovat ich rozostupenie - vytvorizme im nove Td na zaklade vzdialenost medzi nimi
	redraw();
	createList();
}
float TabPage::findDistance(std::string s,TextData::iterator textIter)
{
	float res = 0;
	shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(textIter->_op);
	for (int i=0; i< s.size(); i++)
	{
		res+=txt->getWidth(s[i]);
		res+=textIter->_charSpace;
	}
	return res;
}
void TabPage::insertTextAfter(PdfOp opBehind, double td, double ymax, QString s)
{
	std::list<PdfOp> ops;
	opBehind->getContentStream()->getPdfOperators(ops);
	PdfOperator::Iterator iter(PdfOperator::getIterator(ops.front()));
	shared_ptr<pdfobjects::CompositePdfOperator> comp = findCompositeOfPdfOperator(iter,opBehind);
	_font->createBT();
	std::list<PdfOp> children;
	comp->getChildren(children);
	std::list<PdfOp>::iterator it = children.begin();
	while (*it != opBehind)
	{
		std::string n;
		(*it)->getOperatorName(n);
		if ( typeChecker.isType(OpTextName,n))
		{
			it ++;
			continue;
		}
		_font->addToBT((*it)->clone()); //no tak tam bt viac klonov, no:) ked tak do samostatnej listy a pri TJ sa to deletne
		it++;
	}
	_font->addParameters();
	PdfOp td1 = FontWidget::createTranslationTd(td, ymax);
	_font->addToBT(td1);
	//daj tam text
	{
		PdfOperator::Operands ops;
		ops.push_back(shared_ptr<IProperty>(new CString(s.toAscii().data())));
		PdfOp tj = createOperator("Tj", ops);
		_font->addToBT(tj);
	}
	comp->getContentStream()->insertOperator(comp, _font->createET());
}

/*
void TabPage::showTextAnnot(std::string name)
{
//TODO novy textbox
}
*/
//TODO ask if there should be deletion after what it found
void TabPage::replaceText( QString what, QString by)
{
	while ( true )
	{
		search(what,true);
		if (!_selected)
			break;
		//mame selectnute
		replaceSelectedText(by);
	}
}
void TabPage::setSelected(TextData::iterator& first, TextData::iterator& last)
{
	if (!_selected)
	{
		first = _textList.begin();
		last = _textList.end();
		return;
	}
	if (*last < *first)	
	{
		first = sTextItEnd;
		last = sTextIt;
	}
	else
	{
		first = sTextIt;
		last = sTextItEnd;
	}
}
//slot
void TabPage::search(QString srch, bool forw)
{
	if (forw)
		searchForw(srch);
	else
		searchPrev(srch);
}
QString revert(QString s)
{
	QString rev;
	for ( int i = s.size()-1; i>=0; i--)
		rev += s[i];
	return rev;
}
void TabPage::searchPrev(QString srch)
{
	_searchEngine.setPattern(revert(srch)); //vytvor strom, ktory bude hladat to slovo
	for(int i = 0; i< _pdf->getPageCount(); i++)
	{
		//vysviet prve, ktore najdes
		TextData::iterator iter = _textList.end();
		iter--;
		if (_selected)
		{
			iter = sTextIt;//nic nemen v hladacom engine
		}
		else
		{
			std::wstring w;
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
			txt->getFontText(w);
			_searchEngine.setText(revert(QString::fromStdWString(w)));
		}
		iter->clear();
		float prev = iter->_begin;
		while (iter != _textList.end())
		{
			switch (_searchEngine.search())
			{
			case Tree::Next:
				{
					if (iter == _textList.begin())
						goto NextPage;
					iter--;
					shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
					float dx = txt->getWidth(' ');
					if (fabs(prev - iter->_end) > dx ) //from which space?
						_searchEngine.acceptSpace();
					prev = iter->_begin;
					break;
				}
			case Tree::Found:
				{
					prev = iter->_end; 
					_labelPage->unsetImg();
					double a, b;
					//ak je 
					iter->setBegin(iter->position(iter->_text.size() - _searchEngine._end));
					sTextItEnd = iter;
					for ( int i = 0; i < _searchEngine._tokens; i++)
					{
						iter++;
						iter->clear();
					}
					sTextIt = iter;
					b = iter->position(iter->_text.size() - _searchEngine._begin); 
					//iter->setEnd(a);
					iter->setEnd(b);
					_selected = true; 
					highlight();
					return;
				}
			default:
				{
					throw "Unexpected t->search() token";
				}
			}
			std::wstring w;
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
			txt->getFontText(w);
			_searchEngine.setText(revert(QString::fromStdWString(w)));
		}
		//next page, netreba davat do splashu
NextPage:
		if (_pdf->getPagePosition(_page) == 1)
			_pdf->getPage(_pdf->getPageCount());
		else
			_page = _pdf->getPrevPage(_page);
		//nastav nove _textbox, pretoze sme stejne v textovom rezime
	}
	QMessageBox::warning(this, tr("Not found"),
		tr("String cannot be found"),
		QMessageBox::Ok,
		QMessageBox::Ok);
	//set from th beginning
}
void TabPage::searchForw(QString srch)
{
	for(int i = 0; i< _pdf->getPageCount(); i++)
	{
		_searchEngine.setPattern(srch); //vytvor strom, ktory bude hladat to slovo, pre kazdu stranku znova
		//vysviet prve, ktore najdes
		TextData::iterator iter = _textList.begin();
		iter->clear();
		if (_selected)
		{
			iter = sTextIt;//nic nemen v hladacom engine
			_searchEngine.setText(iter->_text);
			_searchEngine._begin=sTextIt->letters(sTextIt->_begin);
		}
		else
		{
			std::wstring w;
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
			txt->getFontText(w);
			_searchEngine.setText(QString::fromStdWString(w));
		}
		float prev = iter->_end;
		while (iter != _textList.end())
		{
			switch (_searchEngine.search())
			{
			case Tree::Next:
				{
					iter++;
					if (iter == _textList.end())
						goto NextPage;
					//s = iter->_text;
					//sSimp
					//float sizeOfSpace = iter->_op->
					//aprozumijeme medzeru
					shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
					float dx = txt->getWidth(' ');
					if (fabs(prev - iter->_begin) > dx ) //from which space?
						_searchEngine.acceptSpace();
					prev = iter->_end;

					break;
				}
			case Tree::Found:
				{
					_labelPage->unsetImg();
					prev = iter->_end; 
					double a, b;
					//ak je 
					iter->setEnd(iter->position(_searchEngine._end+1));
					sTextItEnd = iter;
					for ( int i = 0; i < _searchEngine._tokens; i++)
					{
						iter--;
						iter->clear();
					}
					sTextIt = iter;
					b = iter->position(_searchEngine._begin+1); 
					//iter->setEnd(a);
					iter->setBegin(b);
					_selected = true; 
					highlight();
					return;
				}
			default:
				{
					throw "Unexpected t->search() token";
				}
			}
			std::wstring w;
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
			txt->getFontText(w);
			_searchEngine.setText(QString::fromStdWString(w));
		}
		//next page, etreba davat do splashu
NextPage:
		if (_pdf->getPagePosition(_page) == _pdf->getPageCount())
			_pdf->getPage(1);
		else
			_page = _pdf->getNextPage(_page);
		//nastav nove _textbox, pretoze sme stejne v textovom rezime
	}
	QMessageBox::warning(this, tr("Not found"),
		tr("String cannot be found"),
		QMessageBox::Ok,
		QMessageBox::Ok);
	//set from th beginning
}
PdfOperator::Iterator TabPage::findTdAssOp(PdfOperator::Iterator iter)
{
	iter.next(); //prve Tj
	while(iter.valid())
	{
		std::string name;
		iter.getCurrent()->getOperatorName(name);
		for ( int i =0; i < TextOperatorIterator::namecount; i++)
			if ( name == TextOperatorIterator::accepted_opers[i])
				return PdfOperator::Iterator(); //invalid
		if (name == "TD")
			return PdfOperator::Iterator(); //invalid
		if (name == "ET")
			return PdfOperator::Iterator(); //invalid
		if (name == "td")
			return iter;
		iter.next();
	}
	return PdfOperator::Iterator(); //invalid
}
void TabPage::deleteSelectedText() //sucasne zarovna
{
	if (!_selected)
		return;
	//prvy  replasni, ostatne vymaz
	QString s[3];
	sTextIt->split(s[0],s[1],s[2]);
	PdfOperator::Operands operand;
	operand.push_back(shared_ptr<IProperty>(new CString(s[0].toAscii().data())));;
	PdfOp op = createOperator("Tj",operand);
	_selected = false;
	double diffX =0.0; //y bude vzdy nulove
	if (s[2]!="") //prave jeden operand, nieco este zostalo
	{
		assert(sTextIt == sTextItEnd);
		diffX = fabs(sTextIt->_end -sTextIt->_begin);
		{
			PdfOperator::Operands operand2;
			operand2.push_back(shared_ptr<IProperty>(shared_ptr<IProperty>(new CString(s[2].toAscii().data()))));
			PdfOp op2 = createOperator("Tj",operand2);
			sTextIt->_op->getContentStream()->insertOperator(sTextIt->_op,op2);
		}
	}
	if (s[0]!="")
		sTextIt->_op->getContentStream()->replaceOperator(sTextIt->_op->getIterator(sTextIt->_op),op);
	if (sTextIt == sTextItEnd)
		goto End;
	sTextIt++;
	while(sTextIt!=sTextItEnd)//delete also Td operators
	{
		PdfOperator::Iterator it = findTdAssOp(PdfOperator::getIterator(sTextIt->_op));
		if (it.valid())
		{
			PdfOp tdOp = it.getCurrent();
			tdOp->getContentStream()->deleteOperator(it);
		}
		sTextIt->_op->getContentStream()->deleteOperator(sTextIt->_op);
		sTextIt++;
	}
	sTextIt->split(s[0],s[1],s[2]);
	assert(s[0]=="");
	if (s[2]=="")
		sTextIt->_op->getContentStream()->deleteOperator(sTextIt->_op);
End:
	_selected = false;
	createList();
	redraw();
}
void TabPage::replaceSelectedText(QString by)
{
	TextData::iterator first, last;
	setSelected(first, last);
	//delete the part
	if (first != last)
	{
		//ak nie su stejne, stejne to zopakuj len pre prve a zvysok zmaz
		QString s[3];
		TextData::iterator i1,i2;
		setSelected(i1,i2);
		i1->split(s[0],s[1],s[2]);
		i1->replaceAllText(s[0]+s[1]+by);
		i1++;
		TextData::iterator it = i1;
		for(; i1!=i2; i1++ )
		{
			i1->_op->getContentStream()->deleteOperator(i1->_op,true);
		}//ten dalsi bude mat asi spravne maticu, jelikoz je to tj-bud bude pokracovat -OK alebo vlastnu - OK
		createList();
		return;
	}
	//zaciatok aj koniec je stejny
	QString s[3];//iba s1 mame vymazat
	first->split(s[0],s[1],s[2]);
	first->replaceAllText(s[0]+by+s[2]);
	//ostatne sa posunu, ak si v stejnom tj-> posunu sa s vlozenim. Ok nie su, maju maticu
}
void TabPage::eraseSelectedText()
{
	if (!_selected)
		return; //staci iba vymazat a vhodne pridat operator \Td
	float corr = 72.0f/displayparams.vDpi;
	if (sTextIt == sTextItEnd)
	{
		QString s[3];
		sTextIt->split(s[0],s[1],s[2]);
		//obkolesime vkladany operator s TD s prislusnou velkostou 
		float dist;
		if (!s[2].isEmpty()) //s2 not empty-> nejaky operator zostava potom 
		{
			PdfOperator::Operands operands;
			std::string e(s[2].toAscii().constData());
			//s[2].toStdString()
			operands.push_back(shared_ptr<IProperty>(CStringFactory::getInstance(e)));
			PdfOp op = createOperator("Tj",operands); //stejny operator, stejny fot
			dist = sTextIt->GetNextStop() - sTextIt->_origX;
			dist*=corr;
			PdfOp optd = FontWidget::createTranslationTd(-dist,0);
			//dist *= 72.0f/displayparams.hDpi;
			sTextIt->_op->getContentStream()->insertOperator(PdfOperator::getIterator(sTextIt->_op),optd);
			sTextIt->_op->getContentStream()->insertOperator(PdfOperator::getIterator(sTextIt->_op),op);
			optd = FontWidget::createTranslationTd(dist,0);
			sTextIt->replaceAllText(s[0]);
			sTextIt->_op->getContentStream()->insertOperator(PdfOperator::getIterator(sTextIt->_op),optd);
		}
		else
			sTextIt->replaceAllText(s[0]);
		//vlozime to, ze sme menili
		return;
	}
//	float distX1 =0;// -sTextIt->GetPreviousStop() + sTextIt->_origX2;//kolko sme zmazali
	float distX2 = sTextItEnd->GetNextStop() - sTextItEnd->GetPreviousStop()-sTextItEnd->_charSpace;
	distX2 *=corr;
    //najskor musim deletnut tie, ktore urcite nechceme, v dalsom kuse kodu robim replace a nema sa to rado
	TextData::iterator it = sTextIt;
	it++;
	while (true)
	{
		if(it==sTextItEnd)
			break;
		//delete operator
		it->_op->getContentStream()->deleteOperator(it->_op);
		it++;
	}

	QString s1,s2,s3;
	{
		//uchovame to, co z praveho operatoru zostalo
		sTextIt->split(s1,s2,s3); //splitneme
		assert(s3=="");
//		PdfOp td = FontWidget::createTranslationTd(distX1,0);
//		sTextIt->_op->getContentStream()->insertOperator( sTextIt->_op,td);
		sTextIt->replaceAllText(s1); // povodny operator bude nezmeneny az na to , ze mu
		//td = FontWidget::createTranslationTd(-distX1,0);
		//insertBefore( td, sTextIt->_op);
	}
	{
		sTextItEnd->split(s1,s2,s3);
		assert(s1 == "");
		PdfOp op = FontWidget::createTranslationTd(-distX2,0);
		sTextItEnd->_op->getContentStream()->insertOperator(sTextItEnd->_op,op);
		op = FontWidget::createTranslationTd(distX2,0);
		sTextItEnd->replaceAllText(s3);
		insertBefore(op, sTextItEnd->_op);
	}
	_selected = false;
}
void TabPage::deleteText( QString text)
{
	replaceText(text,"");
}
void TabPage::exportText()
{
	//dialog na pocet stranok
	int beg = 1, end=2;
	if (_pdf->getPageCount() !=1)
	{
		QDialog * dialog = new QDialog(this);
		Ui::PageDialog pdialog;
		pdialog.begin->setMaximum(_pdf->getPageCount());
		pdialog.begin->setMinimum(1);
		pdialog.end->setMinimum(1);
		pdialog.end->setMaximum(_pdf->getPageCount());
		pdialog.setupUi(dialog);
		dialog->setWindowTitle("Select page range");
		dialog->show();
		if (dialog->result() == QDialog::Rejected)
			return;
		beg = pdialog.begin->value();
		end = pdialog.end->value();
		dialog->close();
		delete dialog;
	}
	int old = _page->getPagePosition();
	_page=_pdf->getPage(beg);
	QTextEdit * edit = new QTextEdit(this);
	//TODO nejaka inicializacia
	QString text;//TODO check ci nie je text moc dlhy, odmedzenia?
	for ( size_t i = beg; i < end; i++)
	{
		float prev =_textList.size() > 0  ? _textList.begin()->_begin : 0;
		for (TextData::iterator iter = _textList.begin(); iter != _textList.end(); iter++)
		{
			shared_ptr<TextSimpleOperator> txt= boost::dynamic_pointer_cast<TextSimpleOperator>(iter->_op);
			if ( fabs(iter->_origX - prev) > iter->_charSpace )
				text.append(" ");
			std::wstring test;
			txt->getFontText(test);
			text.append(QString::fromStdWString(test));
			prev = iter->_origX2;
		}
		SetNextPageRotate();
		createList();
	}
	edit->setText(text);
	edit->setReadOnly(true);
	edit->setGeometry(QRect(0,0,150,150));
	edit->setWindowFlags(Qt::Window);
	//text += QString::from
	//edit->setText(text.latin());
	edit->show();
	_page=_pdf->getPage(old);
	//cakaj na e
}
void TabPage::SetNextPageRotate()
{
	size_t i = _pdf->getPagePosition(_page);
	if ( i == _pdf->getPageCount() )
		_page = _pdf->getPage(1);
	else
		_page = _pdf->getPage(i);
}
/*
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

*/
void TabPage::setImageOperator()
{
	_acceptedType = OpImageName;
}
void TabPage::setTextOperator()
{
	_acceptedType = OpTextName;
}
void TabPage::getSelected( int x, int y, Ops ops)
{
	//show only picture, all picture
	//get only operator
	double px, py;
	//convert
	toPdfPos(x,y, px, py);
	//find operattdisplayparamsors
	_page->getObjectsAtPosition(ops, libs::Point(px,py));

}

std::string TabPage::addFontToPage( std::string name )
{
	CPageFonts::FontList fonts;
	_page->getFontIdsAndNames(fonts);
	CPageFonts::FontList::const_iterator it=pdfobjects::findFont(fonts,name);
	if (it!=fonts.end())
		return it->second;
	std::string ret = _page->addSystemType1Font(name);
	return ret;
}
