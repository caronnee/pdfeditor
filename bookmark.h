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
	/** This load will later call operator loader or property loader */
	void load();
};

/** \brief class for Bookmark handling */
/** This class is similar to AnalyzationItem. It works in lazy-load regime. When it loads, it contains valid page, zoom and X,Y coordinates. If it containt page=0 it means that tjhe bookmark is invalid and its parent should reload it */
class Bookmark : public QTreeWidgetItem
{
	int _page; 
	bool _hasNext;
	bool _loaded;
	pdfobjects::IndiRef _ref;
	double _y;
	double _x;
	int _zoom;
public:
	/** \brief contructor for main bookmark */
	Bookmark(QTreeWidgetItem * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_y(0),_x(0),_zoom(1) {} 
	/** \brief Constructor for bookmark in item */
	/** Initilializes item to invalid values */
	Bookmark(QTreeWidget * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_y(0),_x(0),_zoom(1) {} 

	/** \brief gets destination page */
	/** \return page number. If this page is less or equal to zero, the bookmark is invalid */
	int getDest()const;

	/** \brief Sets the beginning of the nej subsection */
	/** Sets the bookmark reference where start its subsection. */
	void setSubsection( pdfobjects::IndiRef ref );

	/** \brief creates new subsection */
	/** After the item was requested for getting bookmark subsection, this method will be called. */
	void addSubsection(QTreeWidgetItem * item);

	/** \brief gets indirect reference to the subsection's first page */
	/** This is here due to PDF specification, every next page is set via indirect reference to pages dictionary */
	pdfobjects::IndiRef getIndiRef()const;

	/** \brief sets the destination page */
	void setPage( int i );

	/** \brief checks if the bookmarks was loaded */
	bool loaded();

	/** \brief sets X coordinate */
	void setX( double t );

	/** \brief sets Y coordinate */
	void setY( double t );

	/** \brief sets zoom */
	void setZoom( double t );

	/** \brief gets X coordinate */
	double getX();

	/** \brief gets Y coordinate */
	double getY();

	/** \brief gets zoom */
	double getZoom();
};

#endif //__BOOKMARK__
