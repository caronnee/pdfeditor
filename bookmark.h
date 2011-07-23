#ifndef __BOOKMARK__
#define __BOOKMARK__

#include <typedefs.h>
#include <QTreeWidgetItem>
#include <kernel/static.h>
#include <kernel/indiref.h>
#include <kernel/cpdf.h>
#include <kernel/iproperty.h>

class AnalyzeItem : public QTreeWidgetItem
{
	void Init();
	bool _hasChild;
	bool _isProp;
	pdfobjects::PropertyType _type;
	PdfInstance _pdfs;
	PdfProperty _prop;
	PdfOp _op;
	bool _loaded; //name, type, value, indiref
public:
	AnalyzeItem(QTreeWidget * parent, PdfProperty prop);
	AnalyzeItem(QTreeWidgetItem * parent, PdfProperty prop);
	AnalyzeItem(QTreeWidgetItem * parent, PdfOp prop);
	bool loaded();
	void load();
	void loadOperator();
	void loadProperty();
};

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
	Bookmark(QTreeWidgetItem * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_y(0),_x(0),_zoom(1) {} //0 je invalid v tomto pripade
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
