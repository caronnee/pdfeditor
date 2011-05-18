#ifndef ___IMAGE_MODULE__
#define ___IMAGE_MODULE__
#include <QWidget>
#include "ui_insertImage.h"
#include <kernel/pdfedit-core-dev.h>
#include "typedefs.h"
#include <png.h>

class InsertImage : public QWidget
{
	Q_OBJECT

	Ui::Image ui;
	shared_ptr<CInlineImage> inline_image;
	//to, kde sa pridava obrazok, je uz v matici
public:
	InsertImage(QWidget * parent);
	void setImage(shared_ptr<CInlineImage> ii);
public slots:
	void apply();
	void rotationCm(int angle);
	void setImagePath();
	void setPosition(float f1, float f2);
signals:
	void insertImage(PdfOp op); //jake casti
	void changeImage(PdfOp q);
};
#endif

