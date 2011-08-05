#include <kernel/pdfoperators.h>
#include "typedefs.h"
#include <QString>
#include "kernel/displayparams.h"
#include "utils/types/coordinates.h"

typedef boost::shared_ptr<pdfobjects::TextSimpleOperator> PdfTextOperator;

/** \brief structure used for text operation */
/** this structure is used for sorting all the pdf operators and manipulating with them. Every operator initilizes bounding boxes. The text is then extracted from operator. In every case there must be font precent in the operator, otherwise we will get nasty exception */
struct OperatorData
{
	double _begin, _end;
	double _ymin, _ymax; 
	double _charSpace;
	PdfTextOperator _op;
	double _origX, _origX2;
	QString _text; 
	float _width;
	std::vector<float> _letters;
	size_t _id;
#ifdef _DEBUG
	std::string operatorName;
#endif // _DEBUG
public:
	/** \brief contructor */
	OperatorData(PdfOp op, pdfobjects::DisplayParams& displayParams);
	/** \brief position of the first selected char */
	/** if there is none, returns least X coordinate */
	float getPreviousStop();
	/** \brief position after the last selected char */
	/** if there is none, returns position after the last char */
	float getNextStop();

	/** \brief switched the beginning and the end position */
	void change(bool from_beg);
	/** \brief clears selection */
	void clear();
	/** \brief restores beginning of the operator but end is still selected */
	void restoreBegin();
	/** \brief enhances selection to the end of the operator */
	void restoreEnd();
	/** \brief sets slection - either beginning or ending */
	void set(float x,double &place);
	/** \brief check how many letters from the beginning is there until position X */
	int letters(double x);
	/** \brief get the position based upon count of letters from the beginning of the operator */
	double position(int letters);
	/** \brief sets selection from the beginning */
	void setBegin(float x);
	/** \brief sets the end of the selection */
	void setEnd(float x);
	/** \brief comparator *
	/** \return true if operator is logically before the other It calls forward method. The operators must be ordered weakly, therefore of forward method decided that the operators are the same, both are there compared according to their unique IDs*/
	bool operator<(const OperatorData & oper); 
	/** \brief checks if the operator is logically before or after the other. If they have very similar or the same position, param eq will be set true */
	bool forward (const OperatorData & oper, bool & eq)const;
	/** \brief splits the string according to the selection */
	void split(QString& split1, QString& split2, QString& split3);
	/** \brief Destructor */
	~OperatorData();
};