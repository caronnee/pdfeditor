#pragma once
#include <QTabWidget>

class OpenPdf :
	public QTabWidget
{
	Q_OBJECT
private:

public:
	OpenPdf(QWidget * widget);
	~OpenPdf(void);

public slots:

	/* Opens another pdf*/
	void openAnotherPdf();
	void closeAndRemoveTab(int);
	void save();
	void saveAs();
	void pageUp();
	void pageDown();
};
