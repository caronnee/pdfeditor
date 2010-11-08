#ifndef PAGE_H
#define PAGE_H

#include <QScrollArea>
#include "ui_page.h"

class Page : public QScrollArea
{
	Q_OBJECT

public:
	Page(QWidget *parent = 0);
	~Page();

private:
	Ui::page ui;
};

#endif // PAGE_H
