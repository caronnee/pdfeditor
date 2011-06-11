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
}
void InsertImage::setSize(float w,float h)
{
	ui.sizeX->setValue(w);
	ui.sizeY->setValue(h);
}
void InsertImage::setPosition(float pdfX,float pdfY, float scale)
{
	_scale = scale;
	ui.positionX->setValue(pdfX);
	ui.positionY->setValue(pdfY);
}
void InsertImage::setImagePath()
{
	//open dialogand get file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.xpm *.jpg)"));
	if (!fileName.size() == 0)
		this->ui.lineEdit->setText(fileName);
}
//v tomto okamihu mame nazov obrazka
//void InsertImage::rotationCm(int angle)
//{
//	float radian = toRadians(angle);
//	float scale = 1; //this->ui.scale->value();
//	double cs = scale*cos(static_cast<double>(radian));
//	QVariant cs1(cs);
//	double sn = scale*sin(static_cast<double>(radian));
//	QVariant sn1(sn);
//	QVariant sn2(sn*-1);
//	ui.cm->setItem(0,0,new QTableWidgetItem(cs1.toString()));
//	ui.cm->setItem(0,1,new QTableWidgetItem(sn1.toString()));
//	ui.cm->setItem(1,0,new QTableWidgetItem(sn2.toString()));
//	ui.cm->setItem(1,1,new QTableWidgetItem(cs1.toString()));
//}
void InsertImage::createInlineImage()
{	
	if (ui.lineEdit->text()==NULL)
			return;
	QImage image;
	if (!image.load(tr(ui.lineEdit->text().toAscii().data())))
	{
		QMessageBox::warning(this,tr("Unable to load file"),tr("LoadFailed"),QMessageBox::Ok);
		return;
	}
	image = image.scaled(ui.sizeX->value()*_scale,ui.sizeY->value()*_scale,Qt::IgnoreAspectRatio); //proporcie
	CDict image_dict;
	image_dict.addProperty ("W", CInt (image.width())); 
	image_dict.addProperty ("H", CInt (image.height()));
	image_dict.addProperty ("CS", CName ("RGB"));
	image_dict.addProperty ("BPC", CInt (8)); //bits per component, kontanta, ine pdf nevie:)

	int depth = image.depth(); //8 - 8 bitov per pixel
	std::vector<char> imageData; //32 bitove sa musia pretypovat na QRGB
	
	for(int h = 0; h< image.height(); h++)
	{
		for ( int w = 0; w < image.width(); w++)
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
	PdfInlineImage im = boost::shared_ptr<CInlineImage>(new CInlineImage (image_dict,imageData));
	biOp = boost::shared_ptr<pdfobjects::InlineImageCompositePdfOperator>(new InlineImageCompositePdfOperator (im));
}
void InsertImage::apply()
{
	bool create = false;
	if (biOp == NULL)
	{
		createInlineImage();
		create = true;
	}
	//add to buffer everything that is in image
	shared_ptr<UnknownCompositePdfOperator> q(new UnknownCompositePdfOperator("q", "Q"));
	
	int pixW = ui.sizeX->value(); 
	int pixH = ui.sizeY->value();
	float scaleX = (float)ui.scaleX->value()/(100.0f);
	float scaleY = (float)ui.scaleY->value()/(100.0f); //POCET KOMPONENT
	//mame maticy translacie & scale ( mozeme to dat dokopy))
	PdfOperator::Operands posOperands;
	double cs = cos(toRadians(ui.rotation->value())) * scaleX;
	double sn = sin(toRadians(ui.rotation->value())) * scaleY;
	if (create)
	{//translation rotation scale
		q->push_back(createOperatorTranslation(ui.positionX->value(),ui.positionY->value()-pixH ),getLastOperator(q));
		q->push_back(createOperatorRotation(toRadians(ui.rotation->value() ),getLastOperator(q));
		q->push_back(createOperatorScale(pixW,pixH ),getLastOperator(q));
	}
	else
	{
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cs*pixW)));
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(sn))); //zosikmenie?
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(-1*sn)));
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(cs*pixH)));
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(ui.positionX->value())));
		//y-ova suradnica sa pozunie vzhladom na obrazok
		float y = ui.positionY->value();
		y -= pixH; //mimo oblast -OK
		posOperands.push_back(shared_ptr<IProperty>(CRealFactory::getInstance(y)));
	}
	q->push_back(createOperator("cm",posOperands),getLastOperator(q));

	//image.save("outputSave.png");
	//vyhod alpha channel
	
	q->push_back(biOp,getLastOperator(q));
	PdfOperator::Operands o;
	q->push_back(createOperator("Q", o), getLastOperator(q));
	if (create)
		emit(insertImage(q));
	else
		emit(changeImage(q));
	biOp = boost::shared_ptr<InlineImageCompositePdfOperator>();
	this->hide();
}
void InsertImage::setImage(PdfOp ii)
{
	biOp = boost::dynamic_pointer_cast<InlineImageCompositePdfOperator>(ii->clone());
	ui.sizeX->setValue(biOp->getWidth());
	ui.sizeY->setValue(biOp->getHeight());
}
