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

InsertImage::InsertImage( QWidget * parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.cm->setItem(0,0,new QTableWidgetItem("1"));
	ui.cm->setItem(0,1,new QTableWidgetItem("0"));
	ui.cm->setItem(1,0,new QTableWidgetItem("0"));
	ui.cm->setItem(1,1,new QTableWidgetItem("1"));
	ui.cm->setItem(2,0,new QTableWidgetItem("0"));
	ui.cm->setItem(2,1,new QTableWidgetItem("0"));
	ui.scale->setValue(100);
}
void InsertImage::setPosition(float pdfX,float pdfY)
{
	QVariant var(pdfX);
	QVariant var2(pdfY);
	ui.cm->setItem(2,0,new QTableWidgetItem(var.toString()));
	ui.cm->setItem(2,1,new QTableWidgetItem(var2.toString()));
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
	bool create = false;
	if (inline_image == null)
	{
		create = true;
		if (ui.lineEdit->text()==NULL)
			return;
		//snaz sa ho otvorit
		{
			FILE * f = fopen(ui.lineEdit->text().toAscii().data(),"r");
			if (!f)
				return;
			fclose(f);
		}

		QImage image;
		if (!image.load(tr(ui.lineEdit->text().toAscii().data())))
		{
			QMessageBox::warning(this,tr("Unable to load file"),tr("LoadFailed"),QMessageBox::Ok);
			return;
		}
		image = image.scaled(image.width()/4,image.height()/4); //proporcie
		CDict image_dict;
		image_dict.addProperty ("W", CInt (image.width())); 
		image_dict.addProperty ("H", CInt (image.height()));
		image_dict.addProperty ("CS", CName ("RGB"));
		image_dict.addProperty ("BPC", CInt (8)); //bits per component, kontanta, ine pdf nevie:)

		int depth = image.depth(); //8 - 8 bitov per pixel

		std::vector<char> imageData; //32 bitove sa musia pretypovat na QRGB

		/*QImage im = image.alphaChannel();
		im.save("aplhaChangge.png");*/ //alfa neskor

		for(int h = 0; h< pixH; h++)
		{
			for ( int w = 0; w < pixW; w++)
			{
				QRgb color = image.pixel(w,h);
				int r = qRed(color);
				int g = qGreen(color);
				int b = qBlue(color);
				imageData.push_back(r);
				imageData.push_back(g);
				imageData.push_back(b);
			}
		}
		inline_image = (new CInlineImage (image_dict,imageData));
	}
	//add to buffer everything that is in image
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));
	
	float scale = this->ui.scale->value()/100.0f;
	int pixW = inline_image->width(); //get property from image-dic
	int pixH = inline_image->height();
	float scaleX = scale * pixW;
	float scaleY = scale * pixH; //POCET KOMPONENT
	//mame maticy translacie & scale ( mozeme to dat dokopy))
	PdfOperator::Operands posOperands;
	QVariant var = ui.cm->item(0,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scaleX)));
	var = ui.cm->item(0,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scaleY)));
	var = ui.cm->item(1,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scaleX)));
	var = ui.cm->item(1,1)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>()*scaleY)));
	var = ui.cm->item(2,0)->data(0);
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(var.value<float>())));
	var = ui.cm->item(2,1)->data(0);
	//y-ova suradnica sa pozunie vzhladom na obrazok
	float y =var.value<float>();
	y -= pixH; //TODO co sa stane, ak je to mimo oblasti?
	posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(y)));

	q->push_back(createOperator("cm",posOperands),getLastOperator(q));

	//image.save("outputSave.png");
	//vyhod alpha channel
	
	shared_ptr<InlineImageCompositePdfOperator> BI(new InlineImageCompositePdfOperator (inline_image));
	q->push_back(BI,getLastOperator(q));
	PdfOperator::Operands o;
	q->push_back(createOperator("Q", o), getLastOperator(q));
	if (create)
		emit(insertImage(q));
	else
		emit(changeImage(q));
}
void InsertImage::setImage(shared_ptr<CInlineImage> ii)
{
	inline_image = ii;
}
