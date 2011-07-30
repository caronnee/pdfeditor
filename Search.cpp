#include "Search.h"
#include <string.h>

Search::Search(QWidget * parent) : QWidget(parent, Qt::Window),_ignoreCase(false),_wholeWord(false),_regexp(false)
{
	ui.setupUi(this);
	connect(this->ui.text, SIGNAL(returnPressed()),this, SLOT(next()));
	connect(this->ui.nextButton, SIGNAL(pressed()),this, SLOT(next()));
	connect(this->ui.prevButton, SIGNAL(pressed()),this, SLOT(prev()));
}

void Search::showEvent ( QShowEvent * event )
{
	this->ui.text->setFocus();
}
void Search::next()
{
	emit search(ui.text->text(),getFlags() | SearchForward);
}
int Search::getFlags()
{
	int flags = 0;
	flags |= this->ui.caseSensitive->isChecked()? SearchCaseSensitive : 0;
	flags |= this->ui.wholeWord->isChecked()? SearchWholeWords : 0;
	flags |= this->ui.regexp->isChecked()? SearchRegexp: 0;
	return flags;
}
void Search::prev()
{
	emit search(ui.text->text(),getFlags());
}
void Search::replace()
{
//	emit replaceTextSignal(this->ui.text->toPlainText(),this->ui.replacetext->toPlainText());
}
