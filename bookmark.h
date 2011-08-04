#ifndef __BOOKMARK__
#define __BOOKMARK__

#include <typedefs.h>
#include <QTreeWidgetItem>
#include <kernel/static.h>
#include <kernel/indiref.h>
#include <kernel/cpdf.h>
#include <kernel/iproperty.h>

/** \brief main class handling analysation */
/** This class handles all properties and operators of the PDF document \n
	It is used for diplaying ifnformation accourting to the proprty it contains. It is used in such way that at	first it handles DocCatalog of the pdf. I works in the lazy/loading regime. If user clicks on the item tha represents AnalyzeItem, it checks if it is loaded. If it is not, it loads all the properties and operator and parameters */
class AnalyzeItem : public QTreeWidgetItem
{
	/**\brief Initializes analyzation item */
	/** This method is used usually in constructor */
	void Init();
	bool _hasChild;
	bool _isProp;
	pdfobjects::PropertyType _type;
	PdfInstance _pdfs;
	PdfProperty _prop;
	PdfOp _op;
	bool _loaded; //name, type, value, indiref

	void loadOperator();
	void loadProperty();
public:
	/** \Brief contructor for DOC catalog */
	AnalyzeItem(QTreeWidget * parent, PdfProperty prop);

	/** \Brief contructor for Subitems with property type */
	AnalyzeItem(QTreeWidgetItem * parent, PdfProperty prop);

	/** \Brief contructor for Subitems with operator type */
	AnalyzeItem(QTreeWidgetItem * parent, PdfOp prop);

	/** \brief Checks if the analyzation item was loaded */
	bool loaded();
	/** \brief Loads analyzation item */
	void load();
};

/** \brief class for Bookmark handling */
/** This class is similar to AnalyzationItem. It works in lazy-load regime. When it loads, it contains valid page, zoom and x,Y coordinates */
class Bookmark : public QTreeWidgetItem
{
	int _page; //ktoru stranku ma zobrazit
	bool _hasNext;
	bool _loaded;
	pdfobjects::IndiRef _ref;
	double _y;
	double _x;
	int _zoom;
public:
	/** \brief contructor for main bookmark */
	Bookmark(QTreeWidgetItem * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_y(0),_x(0),_zoom(1) {} //0 je invalid v tomto pripade
	/** \brief Constructor for bookmark in item */
	Bookmark(QTreeWidget * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_y(0),_x(0),_zoom(1) {} //0 je invalid v tomto
	int getDest()const;
	void setSubsection( pdfobjects::IndiRef ref );
	void addSubsection(QTreeWidgetItem * item);
	pdfobjects::IndiRef getIndiRef()const;
	void setPage( int i );
	bool loaded();
	void setX( double t );
	void setY( double t );
	void setZoom( double t );
	double getX();
	double getY();
	double getZoom();
};

#endif //__BOOKMARK__
