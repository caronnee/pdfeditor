#ifndef __BOOKMARK__
#define __BOOKMARK__
#include<QTreeWidgetItem>

class Bookmark : public QTreeWidgetItem
{
	int _page; //ktoru stranku ma zobrazit
public:
	Bookmark(int page): _page(page) {}
	int getDest()const
	{
		return _page;
	}
};

#endif //__BOOKMARK__
