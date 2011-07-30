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
	bool _ignoreCase;
	bool _wholeWord;
	bool _regexp;

	int getFlags();
public:
	Search(QWidget * widget);
protected:
	void showEvent ( QShowEvent * event );
public slots:
	void next();
	void prev();
	void replace();
signals:
	void replaceTextSignal(QString a1,QString a2);
	void search(QString,int); //flags
};

#endif
