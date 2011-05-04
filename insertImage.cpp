#include "insertImage.h"
#include "globalfunctions.h"
#include <QFileDialog>
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperators.h>
#include <kernel/factories.h>
#include <kernel/cinlineimage.h>

using namespace boost;
using namespace pdfobjects;

InsertImage::InsertImage()
{
	ui.setupUi(this);
	QVariant x(100);
	ui.cm->setItem(2,0,new QTableWidgetItem(x.toString()));
	ui.cm->setItem(2,1,new QTableWidgetItem(x.toString()));
}
void InsertImage::setImagePath()
{
	//open dialogand get file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.xpm *.jpg)"));
	if (!fileName.size() == 0)
		this->ui.lineEdit->setText(fileName);
}
//v tomto okamihu mame nazov obrazka
void InsertImage::rotationCm(int angle)
{
	float radian = toRadians(angle);
	float scale = 1; //this->ui.scale->value();
	double cs = scale*cos(static_cast<double>(radian));
	QVariant cs1(cs);
	double sn = scale*sin(static_cast<double>(radian));
	QVariant sn1(sn);
	QVariant sn2(sn*-1);
	ui.cm->setItem(0,0,new QTableWidgetItem(cs1.toString()));
	ui.cm->setItem(0,1,new QTableWidgetItem(sn1.toString()));
	ui.cm->setItem(1,0,new QTableWidgetItem(sn2.toString()));
	ui.cm->setItem(1,1,new QTableWidgetItem(cs1.toString()));
}
void InsertImage::apply()
{
	if (ui.lineEdit->text()==NULL)
		return;
	//snaz sa ho otvorit
	{
		FILE * f = fopen(ui.lineEdit->text().toAscii().data());
		if (!f)
			return;
		fclose(f);
	}
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
	// WHAT has to be cStream Buffer, whatever it is..
//	for ( int i =0; i<  ui.lineEdit->text().toAscii().data() );
	const char * b = ui.lineEdit->text().toAscii().data();
	CStream::Buffer buffer(b, b+ui.lineEdit->text().toAscii().length());
	//for ( int i =0; i<  ui.lineEdit->text().toAscii().data() );
	shared_ptr<CInlineImage> inline_image (new CInlineImage (image_dict,buffer));
	shared_ptr<InlineImageCompositePdfOperator> BI(new InlineImageCompositePdfOperator (inline_image));

	q->push_back(BI,getLastOperator(q));
	PdfOperator::Operands o;
	q->push_back(createOperator("Q", o), getLastOperator(q));

	std::vector<shared_ptr<PdfOperator> > contents;
	contents.push_back(q);
	emit(insertImage(q));
}
//void InsertImage::setAngle(int angle)
//{
//	//set cosie, sine
//	double cs = cos(static_cast<double>(angle));
//	QVariant cs1(cs);
//	double sn = sin(static_cast<double>(angle));
//	QVariant sn1(sn);
//	QVariant sn2(sn*-1);
//	ui.cm->setItem(0,0,new QTableWidgetItem(cs1.toString()));
//	ui.cm->setItem(0,1,new QTableWidgetItem(sn1.toString()));
//	ui.cm->setItem(1,0,new QTableWidgetItem(sn1.toString()));
//	ui.cm->setItem(1,1,new QTableWidgetItem(cs1.toString()));
//}