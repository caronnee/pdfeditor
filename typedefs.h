#ifndef __TYPEDEFS__
#define __TYPEDEFS__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperatorsbase.h>

enum Mode
{
	ModeInsertText,
	ModeSelectText,
	DefaultMode,
	OperatorsMode, //moze byt uzitocne
	ImageMode,
	ImageModePart,
	AnntotationMode,
	DrawMode,
	ModeEmitPosition,
	NModes
};

typedef std::vector<boost::shared_ptr<pdfobjects::PdfOperator> > Ops;
typedef std::list<boost::shared_ptr<pdfobjects::PdfOperator> > OpsList;
typedef boost::shared_ptr<pdfobjects::PdfOperator> PdfOp;
typedef pdfobjects::PdfOperator::BBox BBox;

#endif // __TYPEDEFS__
