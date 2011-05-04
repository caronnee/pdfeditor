#include "Search.h"
#include <string.h>

Search::Search()
{
	ui.setupUi(this);
}
void Search::next()
{
	std::string text(ui.text->toPlainText().toAscii().data());
	emit search(text,true);
}
void Search::prev()
{
	std::string text(ui.text->toPlainText().toAscii().data());
	emit search(text,false);
}
void Search::replace()
{
	std::string r,s;
	r = this->ui.replacetext->toPlainText().toAscii().data();
	s = this->ui.text->toPlainText().toAscii().data();
	emit replaceTextSignal(s,r);
}