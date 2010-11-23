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

//END of PDF
#include <qwidget.h>
#include <QString>
#include <QFileDialog>
#include <QRect>
#include "ui_showPage.h"

using namespace boost;
using namespace pdfobjects;

//strom s peviazanymi listami
struct Tree
{
	int y; //podla coho to budem delit
	Tree * more;
	Tree * prev;
	//value, if Any
	boost::shared_ptr< PdfOperator > TextOp;// BT, chceme operantText
};

//co spravit, ked prepnem na inu stranku
enum Mode
{
	HighlightText,
	SelectText
};

struct OperatorData
{
	int begin,end;
	shared_ptr< PdfOperator > op;
};
class TabPage :
	public QWidget
{
	Q_OBJECT

private: //variables
	Ui::TabUI ui;

	QString _name; //name of the file to be opened

	/** pdf objects */
	SplashColorPtr m_image;
	pdfobjects::DisplayParams displayparams;	
	boost::shared_ptr<pdfobjects::CPdf> pdf;
	boost::shared_ptr<pdfobjects::CPage> page;
	
	std::vector < OperatorData > workingOpSet;//zavisla na prave zobrazenej stranke
	std::vector<int> selectedText; //tot ma stejne po workingOpSet
	std::vector<QRect> selectedRegions; 

public:
	TabPage(QString name);
	~TabPage(void);

private:
	QRect getRectangle(shared_ptr < PdfOperator> ops);
	//private methods
	void addRevision( int i = -1);
	void setFromSplash();

	void updatePageInfoBar();
	// gets file, name is name of dialog
	QString getFile(QFileDialog::FileMode flags = QFileDialog::AnyFile);

	int x, y;
	boost::shared_ptr<PdfOperator> findNearestFont(int x, int y);

public:	

	void riseSel();
	void getText();
	void wheelEvent( QWheelEvent * event ); 
	void deletePage();
	void pageUp();
	void pageDown();
	void savePdf(char * name);
	void rotate(int i, int begin, int end);
	void rotateObjects(int angle);

public slots:
	void showClicked(int x, int y);
	void clicked(int x, int y);
//	void updateSelectedRect( std::vector<shared_ptr<PdfOperator> > oops);
//	void copyToClipBoard(); //from selected/ highlighted
	void move(int difx, int dify); //on mouse event, called on mouse realease
	void selectOperators(const QRect rect, std::vector<shared_ptr<PdfOperator> > & opers) ;
	void setSelectedOperators(QRect rect);
	void rotateText( int angle );
	void replaceText( std::string what, std::string by);
	void deleteText( std::string text);
	shared_ptr<PdfOperator> insertText(double x, double y, std::string text, int angle=0 );


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

private slots:

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
};
