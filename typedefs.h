#ifndef __TYPEDEFS__
#define __TYPEDEFS__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperatorsbase.h>
#include <kernel/pdfoperators.h>

#define BEGIN_ZOOM 50
#define MAX_ZOOM 500
#define ZOOM_STEP 50

#define MODES(XX) \
	XX(ModeInsertText,"InsertText mode : click on pdf to insert a text")	\
	XX(ModeSelectText,"Select text mode : Click and drag to select a text") \
	XX(ModeInsertImage,"InsertImage mode: Select a rectangle to insert an image") \
	XX(ModeSelectImage,"Select image mode: Click to select inline image")	\
	XX(ModeImageSelected, "Image selected. Click on buttons to change or to delete image")	\
	XX(ModeImagePartCopied,"Snapshot was copied to the clipboard")	\
	XX(ModeImagePart,"Extract pdf part as image mode: select a rectangle to be saved as image")	\
	XX(ModeExtractImage, "Extract image mode: click on inlineimage to extract an image")	\
	XX(ModeOperatorSelect,"Operator select mode: click on text operator to highligh it")	\
	XX(ModeInsertAnnotation,"Insert Text Annotation mode: click on pdf to insert an annotation")	\
	XX(ModeInsertLinkAnnotation,"Insert Link Annotation mode: click on pdf to insert a link annotation")	\
	XX(ModeDeleteAnnotation, "Delete annotation mode: click on annotation to delete it")	\
	XX(ModeEmitPosition, "Emit position mode: Pdf is waiting for position. Click somewhere in the pdf")	\
	XX(ImageMode," Not Implemented")	\
	XX(DrawMode,"NotImplemented, go away!")	\
	XX(ModeFindLastFont, "Click to select font and font size by text in PDF")\
	XX(DefaultMode,"Open an PDF")
	

#define ENUMS(a,b) a,

#define EARRAY(a,b) b,

enum Mode
{
	MODES(ENUMS)
	NModes
};
enum PageDrawMode
{
	ModeDrawRectangle,
	ModeDrawNothing
};
typedef boost::shared_ptr<pdfobjects::IProperty> PdfProperty;
typedef std::vector<boost::shared_ptr<pdfobjects::PdfOperator> > Ops;
typedef std::list<boost::shared_ptr<pdfobjects::PdfOperator> > OpsList;
typedef boost::shared_ptr<pdfobjects::PdfOperator> PdfOp;
typedef pdfobjects::PdfOperator::BBox BBox;
typedef boost::shared_ptr<pdfobjects::UnknownCompositePdfOperator> PdfComp;

#endif // __TYPEDEFS__
