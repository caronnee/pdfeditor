#include "insertImage.h"
#include "globalfunctions.h"
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperators.h>
#include <kernel/factories.h>
#include <kernel/cinlineimage.h>

using namespace boost;
using namespace pdfobjects;

InsertImage::InsertImage(QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	QVariant x(100); //todo zmazat
	ui.cm->setItem(0,0,new QTableWidgetItem("1"));
	ui.cm->setItem(0,1,new QTableWidgetItem("0"));
	ui.cm->setItem(1,0,new QTableWidgetItem("0"));
	ui.cm->setItem(1,1,new QTableWidgetItem("1"));
	ui.cm->setItem(2,0,new QTableWidgetItem(x.toString()));
	ui.cm->setItem(2,1,new QTableWidgetItem(x.toString()));
	ui.scale->setValue(100);
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
		FILE * f = fopen(ui.lineEdit->text().toAscii().data(),"r");
		if (!f)
			return;
		fclose(f);
	}
	//add to buffer everything that is in image
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));

	float scale = this->ui.scale->value()/100.0f;
	//mame maticy translacie & scale ( mozeme to dat dokopy))
	PdfOperator::Operands posOperands;
	QVariant var = ui.cm->item(0,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));
	var = ui.cm->item(0,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));
	var = ui.cm->item(1,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));
	var = ui.cm->item(1,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));
	var = ui.cm->item(2,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));
	var = ui.cm->item(2,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scale)));

	q->push_back(createOperator("cm",posOperands),getLastOperator(q));

	QImage image;
	if (!image.load(ui.lineEdit->text()))
	{
		QMessageBox::warning(this,tr("Unable to load file"),tr("LoadFailed"),QMessageBox::Ok);
		return;
	}
	CDict image_dict;
	image_dict.addProperty ("W", CInt (image.width())); //TODO sizex
	image_dict.addProperty ("H", CInt (image.height())); //TODO sizey
	image_dict.addProperty ("CS", CName ("RGB"));
	image_dict.addProperty ("BPC", CInt (image.bytesPerLine()));

	std::vector<char> imageData(image.bits(),image.bits()+image.byteCount());

	shared_ptr<CInlineImage> inline_image (new CInlineImage (image_dict,imageData));
	shared_ptr<InlineImageCompositePdfOperator> BI(new InlineImageCompositePdfOperator (inline_image));

	q->push_back(BI,getLastOperator(q));
	PdfOperator::Operands o;
	q->push_back(createOperator("Q", o), getLastOperator(q));

	emit(insertImage(q));
}
