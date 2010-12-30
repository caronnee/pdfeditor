#pragma once
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

//END of PDF
#include <qwidget.h>
#include <QString>
#include <QFileDialog>
#include <QRect>
#include "ui_showPage.h"
#include "page.h"
#include "fontWidget.h"

#include <list>
#include <QTreeWidgetItem>
#include "typedefs.h"

using namespace boost;
using namespace pdfobjects;

//co spravit, ked prepnem na inu stranku
enum Mode
{
	DefaultMode,
	TextMode,
	OperatorsMode, //moze byt uzitocne
	ImageMode,
	AnntotationMode,
	DrawMode,
	NumberOfModes
};

//mozno to je moc vysoke?  Normalny text ma 10, na rise bude mat 5, alklurat neviem, ci je to v pixelochaa ak tam ma este tie svoje double...
#define EPSILON_Y 5
//TODO pozor na rotaciu stranky, bude to fachat?
//budeme predpokladat, ze vsetko toto je platne, ze sme to uz nastavili
struct OperatorData
{
	double _begin, _end;
	double _ymin, _ymax; 
	PdfOp _op;
	std::string _text; //jak bol text v tom, pripadne uprava o medzeru, prepisanie ma medzeru, ako konci -, odstranit
	OperatorData(PdfOp op) : _begin(0), _end(0), _op(op)
	{
		shared_ptr<TextSimpleOperator> txt = boost::dynamic_pointer_cast<TextSimpleOperator>(op);
		txt->getRawText(_text);
		libs::Rectangle r = _op->getBBox();
		_ymin = min<double>(r.yleft, r.yright);
		_ymax = max<double>(r.yleft, r.yright);
		clear();
	}

	void change()
	{
		double d = _begin;
		_begin = _end;
		_end = d;
	}
	void clear()
	{
		libs::Rectangle r = _op->getBBox();
		_begin = min<float>(r.xleft,r.xright);
		_end = max<float>(r.xleft,r.xright);
	}
	void restoreEnd()
	{
		double b = _begin;
		clear();
		_begin = b;
	}
	void set(int x,double &place)
	{
		clear();
		float width( _end - _begin );
		float oneLetterStep =  width/_text.size();
		int letters = x / oneLetterStep;
		place = letters * oneLetterStep;
	}
	void setBegin(int x)
	{
		set(x,_begin);
	}
	void setEnd(int x)
	{
		set(x,_end);
	}
	bool operator<(const OperatorData & oper) //zoradime podla y-osi
	{
		BBox a = _op->getBBox();
		BBox b = oper._op->getBBox();
		//cim vyssie je y, tym vyssie je na obrazovke, t.j. ty to bude prvsie
		//ak je rozdiel moc maly v y osi, si na jednej lajne
		float maxy = max(a.yleft, a.yright);
		float opMaxy = max(b.yleft, b.yright);
		
		if (fabs(maxy - opMaxy) > EPSILON_Y) //rozhodni podla y osi, cim mensi, tym blizsie
		{
			return maxy - opMaxy < 0;
		}
		maxy = min(a.xleft, a.xright);
		opMaxy = min(b.xleft,b.xright);
		return maxy < opMaxy;
	}
	bool forward(int x, int y)
	{
		BBox b = _op->getBBox();
		double maxY = max(b.yleft, b.yright);
		double maxX = max(b.xleft, b.xright);
		if (maxY + EPSILON_Y < y)
			return true; //pohni sa smerom dopredu
		if (maxY +EPSILON_Y > y)
			return false;
		//ak je na stejnej lajne cca, checkneme x
		return maxX < x;
	}
};
typedef std::list<OperatorData> TextData;

enum AcceptName
{
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
		names[OpTextName].add("TJ");
		names[OpTextName].add("Tj");
		names[OpTextName].add("'");
		names[OpTextName].add("\"");

		names[OpImageName].add("ID");
		names[OpImageName].add("Id");

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
class TabPage : public QWidget
{
	Q_OBJECT

private: //variables
	Ui::TabUI ui; 

	std::vector<std::string> acceptedAnotName;//TODO static alebo enum alebo cos
	//could be static. but :)
	QRegion _region;
	QString _name; //name of the file to be opened
	FontWidget * _font;	
	Mode _mode;

//	std::vector<AcceptOperatorName> opNames;
	/** pdf objects */
//	SplashColorPtr m_image; //pouzve sa neskor pri configu

	IsType typeChecker;
	pdfobjects::DisplayParams displayparams;	
	boost::shared_ptr<pdfobjects::CPdf> pdf;
	boost::shared_ptr<pdfobjects::CPage> page;
	TextData _textList;
	//oterator	 of selected
	TextData::iterator sTextIt;
	TextData::iterator sTextItEnd; //kde ten iterator konci
	bool _dataReady; //pouzivane vseobecne, kedy sa to hodi
	bool dirty;
	//TODO mat este jeden iterator Actual, aby sa to stale neprekreslovalo cele
	Ops workingOpSet;//zavisla na prave zobrazenej stranke
	CPage::Annotations _annots;
	DisplayPage * labelPage;
private:
	void toPdfPos(int x, int y, double & x1, double &y1);
	void toPixmapPos(double x1, double y1, int & x, int & y);

public:

	void createAnnot(AnnotType t, std::string * params);
	void delAnnot(int i); //page to u seba upravi, aby ID zodpovedali

	void mouseReleased(); //nesprav nic, pretoze to bude robit mouseMove
	void SetTextSelect();
	void UnSetTextSelect();
	TabPage(QString name);
	~TabPage(void);

	void setTree(shared_ptr<CDict> d, QTreeWidgetItem * item);
	void SetModeTextSelect();
	void highLightBegin(int x, int y); //nesprav nic, pretoze to bude robit mouseMove
	void highlightText(int x, int y); //tu mame convertle  x,y

private:
	void setAnnotations();
	void loadFonts(FontWidget * font);
	void getAtPosition(Ops& ops, int x, int y );
	void setTextData(TextData::iterator &begin, TextData::iterator end, shared_ptr<PdfOperator> op);

	//TODO zostit rotaciu boxu. to je but tm alebo Qstate
	QRect getRectangle( PdfOp ops);
	QRect getRectangle(BBox box);

	//private methods
	void addRevision( int i = -1);

	void setFromSplash();

	void updatePageInfoBar();
	// gets file, name is name of dialog
	QString getFile(QFileDialog::FileMode flags = QFileDialog::AnyFile);

	int x, y;

	void showClicked(int x, int y);
public:	
	void getBookMarks(); //LATER, treba actions zisti, ako sa vykoavaju
//	void changeImageProp(); // v selected mame images//LATER
	void insertImage(int x, int y, const QImage & img);
	//nastavi u page cakanie na skoncenie kreslenie ( nieco emitne:)
	void draw();
	void insertImageFile(int x, int y);
	void getText();
	void wheelEvent( QWheelEvent * event ); 
	void deletePage();
	void pageUp();
	void pageDown();
	void savePdf(char * name);
	//rotate page

public slots:
	void handleBookMark(QTreeWidget * item);
	void removeObjects();
	void clicked(int x, int y);
//	void updateSelectedRect( std::vector<shared_ptr<PdfOperator> > oops);
//	void selectOperators(const QRect rect, std::vector<shared_ptr<PdfOperator> > & opers) ;
	//void setSelectedOperators(QRect rect);

	void insertText( PdfOp op );

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

	void rotate(int i, int begin, int end);
private slots:
	void zoom(QString zoomscale);
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
	void replaceText( std::string what, std::string by);
	void deleteText( std::string text);
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
};
