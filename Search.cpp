#include "Search.h"

Search::Search()
{
	ui.setupUi(this);
}
void Search::clicked()
{
	emit search(ui.text->toPlainText().toAscii().data());
}
