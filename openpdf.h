#pragma once
#include <QTabWidget>
#include <QString>
#include "typedefs.h"

class OpenPdf :
	public QTabWidget
{
	Q_OBJECT
private:
	void open(QString s);
public:
	OpenPdf(QWidget * widget);
	~OpenPdf(void);

	Mode _mode;
public:
	void setMode(Mode mode);
	Mode getMode() const { return _mode; }
signals:
	void ModeChangedSignal(QString);
public slots:
	void search(QString s, bool v);
	void deleteSelectedText();
	void eraseSelectedText();
	void changeSelectedText();
	void highlightSelected();

	void setModeExtractImage();
	void setModeImagePart();
	void setModeInsertText();
	void setModeSelectText();
	void setModeInsertImage();
	void setModeDefault();
	void setModeSelectImage();
	void setModeInsertAnotation();

	void changeSelectedImage();
	void deleteSelectedImage();
	void saveEncoded();
	// extracts text
	void getText();
	void derotate();
	//rotates active page
	void rotate();

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
	void setModeDeleteAnnotation();
	void pdfChanged();
	void redraw();
	void setPreviousMode();
};
