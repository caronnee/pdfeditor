#ifndef __BOOKMARK__
#define __BOOKMARK__

#include <QTreeWidgetItem>
#include <kernel/static.h>
#include <kernel/indiref.h>

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
	Bookmark(QTreeWidgetItem * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_x(0),_y(0),_zoom(1) {} //0 je invalid v tomto pripade
	Bookmark(QTreeWidget * parent): QTreeWidgetItem(parent),_page(0),_loaded(true),_x(0),_y(0),_zoom(1) {} //0 je invalid v tomto
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
