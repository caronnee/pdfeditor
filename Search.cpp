#include "Search.h"
#include <string.h>

Search::Search(QWidget * parent) : QWidget(parent),_ignoreCase(false),_wholeWord(false),_regexp(false)
{
	ui.setupUi(this);
	connect(this->ui.text, SIGNAL(returnPressed()),this, SLOT(next()));
	connect(this->ui.nextButton, SIGNAL(pressed()),this, SLOT(next()));
	connect(this->ui.prevButton, SIGNAL(pressed()),this, SLOT(prev()));

}

void Search::next()
{
	emit search(ui.text->text(),true);
}
void Search::prev()
{
	emit search(ui.text->text(),false);
}
void Search::replace()
{
//	emit replaceTextSignal(this->ui.text->toPlainText(),this->ui.replacetext->toPlainText());
}
