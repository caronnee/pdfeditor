#pragma once
#include <QTabWidget>
#include <QString>
#include <QTimer>
#include "typedefs.h"

struct HelptextIcon
{
	const char * helpText;
	const char * icon;
	const char * description;
};

class OpenPdf :
	public QTabWidget
{
	Q_OBJECT
	

public:
	OpenPdf(QWidget * widget);
	~OpenPdf(void);

	void open(QString s);
	Mode _previous;
	Mode _mode;
	QColor _color;
	QColor _highlightColor;
	std::string _author;
	QTimer pageTimer;
public:
	void setMode(Mode mode);
	Mode getMode() const { return _mode; }

signals:
	QColor GetActualHColorSignal();
	QColor GetActualColorSignal();
	void ModeChangedSignal(HelptextIcon);
public slots:
	void initAnalyze();
	void search(QString s, int flags);
	void deleteSelectedText();
	void eraseSelectedText();
	void changeSelectedText();
	void highlightSelected();

	void setModeOperator();
	void setModeView();
	void setModeChangeAnnotation();
	void setHighlighCommentText();
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
	void stopSearch();

	///print a page
	void print();
	//Deletes actual page, if theree is more that one page
	void deletePage();
	void setModeDeleteHighLight();
	void setModeDeleteAnnotation();
	void pdfChanged();
	void redraw();
	void setPreviousMode();
	void setColor(QColor);
	void setHColor(QColor);
	QColor getColor();
	QColor getHColor();
	void setModeInsertLinkAnotation();
	void setModeSetting();
	void about();
	void extractImage();

signals:
	void OpenSuccess(QString);
public:
	std::string Author()const;
	void checkClose();
	//void resizeEvent(QResizeEvent *event);
};
