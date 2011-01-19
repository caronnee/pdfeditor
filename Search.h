#ifndef ___SEARCH___
#define ___SEARCH___

#include <QWidget>
#include <QString>
#include "ui_search.h"

class Search : public QWidget 
{
	Q_OBJECT

	Ui::Search ui;
public:
	Search();
	void clicked();
signals:
	void search(QString);
};

#endif
