#ifndef ___SEARCH___
#define ___SEARCH___

#include <QWidget>
#include <QString>
#include "ui_search.h"
#include <string>

class Search : public QWidget 
{
	Q_OBJECT

	Ui::Search ui;
public:
	Search();
public slots:
	void next();
	void prev();
signals:
	void search(std::string,bool);
};

#endif
