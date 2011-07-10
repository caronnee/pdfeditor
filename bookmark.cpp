#include "bookmark.h"
//#include <kernel/pdfedit-core-dev.h>
//#include <kernel/indiref.h>

using namespace pdfobjects;

int Bookmark::getDest()const
{
	return _page;
}

void Bookmark::setSubsection( pdfobjects::IndiRef ref ) 
{
	_ref = ref;
	_loaded = false;
	setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
}
void Bookmark::addSubsection(QTreeWidgetItem * item)
{
	this->addChild(item);
	_loaded = true;
}
pdfobjects::IndiRef Bookmark::getIndiRef()const
{
	return _ref; //TODO co ak tam nebude REG ale priamo dict? Mozne!
}
void Bookmark::setPage( int i ) 
{
	_page = i;
}

bool Bookmark::loaded() 
{
	return _loaded;
}

void Bookmark::setX( double t )
{
	_x = t;
}

void Bookmark::setY( double t )
{
	_y = t;
}

void Bookmark::setZoom( double t )
{
	_zoom = t;
}

double Bookmark::getX()
{
	return _x;
}

double Bookmark::getY()
{
	return _y;
}

double Bookmark::getZoom()
{
	return _zoom;
}
