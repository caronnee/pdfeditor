#ifndef ___SEARCH___
#define ___SEARCH___

#include <QWidget>
#include <QString>
#include "ui_search.h"
#include <string>
#include "typedefs.h"

class Search : public QWidget 
{
	Q_OBJECT

	Ui::Search ui;

	int getFlags();
public:
	Search(QWidget * widget);
protected:
	void showEvent ( QShowEvent * event );
public slots:
	void next();
	void prev();
	void replace();
	void stop();
signals:
	void replaceTextSignal(QString a1,QString a2);
	void search(QString,int); //flags
	void stopSignal();
};

#endif
