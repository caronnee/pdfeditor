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
	Search(QWidget * widget);
public slots:
	void next();
	void prev();
	void replace();
signals:
	void replaceTextSignal(QString a1,QString a2);
	void search(QString,bool);
};

#endif
