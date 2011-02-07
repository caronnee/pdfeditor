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
	void clicked();
signals:
	void search(std::string);
};

#endif
