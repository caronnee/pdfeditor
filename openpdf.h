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

	// extrcts text
	void getText();

	//rotates active page
	void rotate(int angle);

	///Opens another pdf
	void openAnotherPdf();

	///removes tab when closed
	void closeAndRemoveTab(int);

	//saves under original name
	void save();

	///save unser another name
	void saveAs();

	///move the actual page up
	void pageUp();

	///moves the actual page down
	void pageDown();

	//insert range from another pdf
	void insertRange();

	//inserts empty page
	void insertEmpty();

	///print a page
	void print();

	//Deletes actual page, if theree is more that one page
	void deletePage();
};
