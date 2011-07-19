#include <kernel/pdfoperators.h>
#include "typedefs.h"
#include <QString>

typedef boost::shared_ptr<pdfobjects::TextSimpleOperator> PdfTextOperator;
//TODO pozor na rotaciu stranky, bude to fachat?
//budeme predpokladat, ze vsetko toto je platne, ze sme to uz nastavili
struct OperatorData
{
	double _begin, _end;
	double _ymin, _ymax; 
	double _charSpace;
	PdfTextOperator _op;
	double _origX, _origX2;
	QString _text; //jak bol text v tom, pripadne uprava o medzeru, prepisanie ma medzeru, ako konci -, odstranit
public:
	OperatorData(PdfOp op);
	float GetPreviousStop();
	float GetNextStop();
	void change(bool from_beg);
	void clear();
	void restoreBegin();
	void restoreEnd();
	void set(float x,double &place);
	int letters(double x);
	double position(int letters);
	void setMark(float x, bool beg);
	void setBegin(float x);
	void setEnd(float x);
	bool operator<(const OperatorData & oper); //zoradime podla y-osi
	bool forward (double x, double y)const;
	//split odla toho, ako sme to vysvietili
	void split(QString& split1, QString& split2, QString& split3);
	void replaceAllText(std::string s);
};