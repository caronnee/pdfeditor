#ifndef __BOOKMARK__
#define __BOOKMARK__

#include <QTreeWidgetItem>
#include "kernel/indiref.h"

class Bookmark : public QTreeWidgetItem
{
	int _page; //ktoru stranku ma zobrazit
	bool _hasNext;
	bool _loaded;
	pdfobjects::IndiRef _ref;
public:
	Bookmark(QTreeWidgetItem * parent): QTreeWidgetItem(parent), _page(0),_loaded(true) {} //0 je invalid v tomto pripade
	Bookmark(QTreeWidget * parent): QTreeWidgetItem(parent), _page(0),_loaded(true) {} //0 je invalid v tomto

	int getDest()const
	{
		return _page;
	}

	void setSubsection( pdfobjects::IndiRef ref ) 
	{
		_ref = ref;
		_loaded = false;
		setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	}
	void addSubsection(QTreeWidgetItem * item)
	{
		this->addChild(item);
		_loaded = true;
	}
	pdfobjects::IndiRef getIndiRef()const
	{
		return _ref; //TODO co ak tam nebude REG ale priamo dict? Mozne!
	}
	void setPage( int i ) 
	{
		_page = i;
	}

	bool loaded() 
	{
		return _loaded;
	}
};

#endif //__BOOKMARK__
