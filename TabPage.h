#ifndef __TABPAGE__
#define __TABPAGE__

#include <qwidget.h>
#include <QString>
#include <QFileDialog>
#include <QRect>
#include "ui_Tab.h"
#include "page.h"
#include "fontWidget.h"
#include <list>
#include <QTreeWidgetItem>
#include <QTextEdit>
#include "typedefs.h"
#include "debug.h"
#include "Search.h"
#include "comments.h"
#include "tree.h"
#include "insertImage.h"


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
#include <kernel/factories.h>
#include <kernel/displayparams.h>
#include <kernel\pdfoperatorsbase.h>

//END of PDF
using namespace boost;
using namespace pdfobjects;

//co spravit, ked prepnem na inu stranku


#include "operatorData.h"

typedef std::list<OperatorData> TextData;

enum AcceptName
{
	OpFontName,
	OpTextName,
	OpImageName,
	OpGraphicName,
	OpAcceptCount
};

struct AcceptOperatorName
{
	std::vector<std::string> names;
	bool isType(std::string name)
	{
		for (size_t i =0; i < names.size(); i++)
		{
			if (name == names[i]) //operator name
				return true;
		}
		return false;
	}
	void add(std::string name)
	{
		names.push_back(name);
	}
};
class IsType
{
	AcceptOperatorName names[OpAcceptCount];

public:
	IsType()
	{
		names[OpFontName].add("Tf");
		names[OpFontName].add("tf");
		names[OpTextName].add("TJ");
		names[OpTextName].add("Tj");
		names[OpTextName].add("'");
		names[OpTextName].add("\"");

		names[OpImageName].add("BI");
		/*names[OpImageName].add("ID");
		names[OpImageName].add("Id");*/

		names[OpGraphicName].add("DO");
		names[OpGraphicName].add("Do");
	}
	bool isType(int type, std::string n)
	{
		return names[type].isType(n);
	}
	bool acceptType(std::string name)
	{
		for ( int i =0 ; i < OpAcceptCount; i++)
		{
			if (isType(i,name))
				return true;
		}
		return false;
	}
};
enum AnnotType
{
	LinkAnnot,
	TextAnnot,
	HighLighAnnot,
	StrokeAnnot
};
class OpenPdf;

class TabPage : public QWidget
{
	Q_OBJECT

private: //variables
	int _acceptedType;
	OpenPdf * _parent;
	Ui::TabUI ui; 
	QWidget * widget;
	Search * _search;
	InsertImage * _image;
	QPoint _mousePos;
	Tree _searchEngine;
	Comments * _cmts;
	//std::vector<std::string> acceptedAnotName;//TODO static alebo enum alebo cos
	//could be static. but :)
	QRegion _region;
	QString _name; //name of the file to be opened
	FontWidget * _font;	
	Mode _mode;
	//PdfOp _workingOp;
	IsType typeChecker;
	pdfobjects::DisplayParams displayparams;	
	boost::shared_ptr<pdfobjects::CPdf> _pdf;
	boost::shared_ptr<pdfobjects::CPage> _page;
	TextData _textList;
	//oterator	of selected
	TextData::iterator sTextIt, sTextItEnd, sTextMarker; //kde ten iterator konci
	bool _dataReady; //pouzivane vseobecne, kedy sa to hodi
	bool _selected;
	PdfOp _selectedImage;

	CPage::Annotations _annots;
	DisplayPage * _labelPage;
private:
	pdfobjects::IndiRef createAppearanceDict(float *dim);
	void fillCoordinates(std::vector<float>& coordinates, float * dim);
	void initRevisions();
	PdfOperator::Iterator TabPage::findTdAssOp(PdfOperator::Iterator iter);
	void rotatePdf(int angle, double& x,double& y,bool fromPixMap);
	float findDistance(std::string s,TextData::iterator textIter);
	void SetNextPageRotate();
	/* vytvorit textovy list */
	void searchPrev(QString srch);
	void searchForw(QString srch);
	void getSelected(int x , int y, Ops ops);
	void toPdfPos(int x, int y, double & x1, double &y1);
	void toPixmapPos(double x1, double y1, int & x, int & y);
	void inDirection(TextData::iterator & it, bool forw);
	void setSelected(TextData::iterator& first, TextData::iterator& last);
	void showAnnotation();
public:
	void deleteSelectedImage();
	void raiseChangeSelectedImage();
	void createList();
	void highLightAnnSelected();
	void delAnnot(int i); //page to u seba upravi, aby ID zodpovedali
	void SetTextSelect();
	
	void UnSetTextSelect();
	TabPage(OpenPdf *,QString name);
	~TabPage(void);

	void setTree(shared_ptr<CDict> d, QTreeWidgetItem * item);
	void highLightBegin(int x, int y); //nesprav nic, pretoze to bude robit mouseMove
	void highlightText(QPoint point); //tu mame convertle  x,y

	void moveText(int difX, int difY);
	void insertBefore(PdfOp op, PdfOp before);
	void createAddMoveString(PdfOp bef, double x, double y, QString name);
	void insertTextAfter(PdfOp opBehind, double td, double ymax, QString s);

private:
	void insertAnnotation(Annot a);
	void loadFonts(FontWidget * font);
	void getAtPosition(Ops& ops, int x, int y );
	void setTextData(TextData::iterator &begin, TextData::iterator end, shared_ptr<PdfOperator> op);
	void deleteText( QString text);

	//TODO zIstit rotaciu boxu. to je but tm alebo Qstate
	QRect getRectangle( PdfOp ops );
	QRect getRectangle( BBox box );

	//private methods
	void addRevision( int i = -1);

	
	void updatePageInfoBar();
	// gets file, name is name of dialog
	QString getFile(QFileDialog::FileMode flags = QFileDialog::AnyFile);
	void showClicked(int x, int y);
public:	
	void redraw();
	void getBookMarks(); //LATER, treba actions zisti, ako sa vykoavaju
//	void changeImageProp(); // v selected mame images//LATER
	//nastavi u page cakanie na skoncenie kreslenie ( nieco emitne:)
	void draw();
	void wheelEvent( QWheelEvent * event ); 
	void deletePage();
	void pageUp();
	void pageDown();
	void savePdf(char * name);
	//rotate page

public slots:
	void save();
	void saveAs();
	void saveEncoded();

	void setTextOperator();
	void setImageOperator();

	std::string addFontToPage(std::string id);
	void replaceText( QString what, QString by);
	void changeSelectedImage(PdfOp op);
	void raiseSearch();
	void closeAnnotDiag();
	void changeSelectedText();
	void deleteSelectedText();
	void eraseSelectedText();
	void replaceSelectedText(QString by);
	void insertImage(PdfOp op);
	void mouseReleased(QPoint); //nesprav nic, pretoze to bude robit mouseMove
	void insertTextMarkup(Annot annot);
	void waitForPosition(); //nastao stav taky aby emitovala aktualne kliknitu poziciu
	
	void deleteAnnotation(QPoint);
	void search(QString text,bool forw);
	void handleBookmark(QTreeWidget * item, int);
	void highlight(); //nesprav nic, pretoze to bude robit mouseMove
//	void removeObjects();
	void clicked(QPoint point);
//	void updateSelectedRect( std::vector<shared_ptr<PdfOperator> > oops);
//	void selectOperators(const QRect rect, std::vector<shared_ptr<PdfOperator> > & opers) ;
	//void setSelectedOperators(QRect rect);

	void insertText( PdfOp op );
	void raiseInsertText(QPoint);
	void raiseChangeSelectedText();
	void raiseInsertImage(QRect);
	void raiseAnnotation(QPoint point);

	void deleteImage(QPoint point);
	///Sets image to previous page
	bool previousPage();
	///Sets image to next page
	bool nextPage();

	///insert range
	void insertRange();

	/// Adds empty page
	void addEmptyPage();

	//prints pdf
	void print();

	/** exports text to the chosen file & opens that file ( txt ) in view */
	void exportText();

	void rotate(int angle);

private slots:

	void zoom(QString zoomscale);
	/// init pdf-reader to have this revision
	void initRevision(int revision);

	/// Inserts range of file from existing PDF
	void insertPageRangeFromExisting();

	/// Saves actual made changes to new revision
	/** Nothign else happens, no need to  
	 */
	void commitRevision();

	/// Saves revision-specific pdf to new pdf
	void exportRevision();

	/// Revert revision
	void revertRevision();
//----------------------------------------------------------------------------------------------------	
	/* To implement
	void showTextAnnot(std::string name);
	void copyToClipBoard(); //from selected/ highlighted
	void rotateObjects(int angle);
	void handleAnnotation(int i);
	void rotateText( int angle );
	boost::shared_ptr<PdfOperator> findNearestFont(int x, int y);
	void move(int difx, int dify); //on mouse event, called on mouse realease
	void riseSel();
	void changeText(std::string name, int size);//tazkopadne?
	void search(std::string text);
	*/
signals:
	void markPosition(QPoint point); //reverted point
	void parsed(std::vector<float>);
	void pdfPosition(float a, float b, int w,int h);
	void pdfText(std::string s);
};

#endif
