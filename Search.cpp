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