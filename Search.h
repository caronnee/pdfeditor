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
	void replace();
signals:
	void replaceTextSignal(std::string a1,std::string a2);
	void search(std::string,bool);
};

#endif
