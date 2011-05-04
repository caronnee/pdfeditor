#ifndef ___IMAGE_MODULE__
#define ___IMAGE_MODULE__
#include <QWidget>
#include "ui_insertImage.h"
#include <kernel/pdfedit-core-dev.h>
#include "typedefs.h"

class InsertImage : public QWidget
{
	Q_OBJECT

	Ui::Image ui;
public:
	InsertImage();
public slots:
	void apply();
	void rotationCm(int angle);
	void setImagePath();
signals:
	void insertImage(PdfOp op); //jake casti
};
#endif
