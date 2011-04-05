#include "Search.h"
#include <string.h>

Search::Search()
{
	ui.setupUi(this);
}
void Search::clicked()
{
	std::string text(ui.text->toPlainText().toAscii().data());
	emit search(text);
}
