#include "insertImage.h"
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperators.h>
#include <kernel/factories.h>
#include <kernel/cinlineimage.h>

using namespace boost;
using namespace pdfobjects;

InsertImage::InsertImage()
{
	ui.setupUi(this);
}
//v tomto okamihu mame nazov obrazka
void InsertImage::inserting()
{
	//add to buffer everything that is in image
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));

	//mame maticy translacie & scale ( mozeme to dat dokopy))
	PdfOperator::Operands posOperands;
	QVariant var = ui.cm->item(0,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(0,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(1,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(1,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(2,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(2,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));

	q->push_back(createOperator("cm",posOperands));

	CDict image_dict;
	image_dict.addProperty ("W", CInt (0)); //TODO sizex
	image_dict.addProperty ("H", CInt (0)); //TODO sizey
	image_dict.addProperty ("CS", CName ("RGB"));
	image_dict.addProperty ("BPC", CInt (8));
	//CInlineImage img (image_dict, what);
	shared_ptr<CInlineImage> inline_image (new CInlineImage (/*image_dict, what*/));
	shared_ptr<InlineImageCompositePdfOperator> BI(new InlineImageCompositePdfOperator (inline_image));

	q->push_back(BI,getLastOperator(q));
	PdfOperator::Operands o;
	q->push_back(createOperator("Q", o), getLastOperator(q));

	std::vector<shared_ptr<PdfOperator> > contents;
	contents.push_back(q);
	emit(insertImage(q));
}
