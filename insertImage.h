#ifndef ___IMAGE_MODULE__
#define ___IMAGE_MODULE__
#include <QWidget>
#include "ui_insertImage.h"
#include <kernel/pdfedit-core-dev.h>
#include <kernel/cinlineimage.h>
#include <boost/shared_ptr.hpp>
#include <kernel\pdfoperators.h>
#include <xpdf\xpdf\GfxState.h>
#include "typedefs.h"

class InsertImage : public QWidget
{
	Q_OBJECT

	PdfOp _invertCm;
	Ui::Image ui;
	float _scale;
	typedef boost::shared_ptr<pdfobjects::CInlineImage> PdfInlineImage;
	boost::shared_ptr<pdfobjects::InlineImageCompositePdfOperator> biOp;
	//to, kde sa pridava obrazok, je uz v matici
	void createInlineImage();
	void init();
public:
	InsertImage(QWidget * parent);
	void setImage(PdfOp image, double scale);//TODO x,y
public slots:
	void apply();
	//void rotationCm(int angle);
	void setImagePath();
	void setPosition(float f1, float f2,float scale); //todo xscale, yscale
	void setSize(float w,float h);
signals:
	void insertImage(PdfOp op); //jake casti
	void changeImage(PdfOp q);
};
#endif

