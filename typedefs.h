#ifndef __TYPEDEFS__
#define __TYPEDEFS__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <kernel/pdfoperatorsbase.h>
#include <kernel/pdfoperators.h>
#include <kernel/cannotation.h>

#define BEGIN_ZOOM 50
#define MAX_ZOOM 500
#define ZOOM_STEP 50

#define MODES(XX) \
	XX(ModeDoNothing," Viewmode ","view.ico")	\
	XX(ModeInsertText,"InsertText mode : click on pdf to insert a text","unknown.jpg")	\
	XX(ModeSelectText,"Select text mode : Click and drag to select a text","unknown.jpg") \
	XX(ModeInsertImage,"InsertImage mode: Select a rectangle to insert an image","unknown.jpg") \
	XX(ModeSelectImage,"Select image mode: Click to select inline image","unknown.jpg")	\
	XX(ModeImageSelected, "Image selected. Click on buttons to change or to delete image","unknown.jpg")	\
	XX(ModeImagePartCopied,"Snapshot was copied to the clipboard","unknown.jpg")	\
	XX(ModeImagePart,"Extract pdf part as image mode: select a rectangle to be saved as image","unknown.jpg")	\
	XX(ModeExtractImage, "Extract image mode: click on inlineimage to extract an image","unknown.jpg")	\
	XX(ModeOperatorSelect,"Operator select mode: click on text operator to highligh it","unknown.jpg")	\
	XX(ModeInsertAnnotation,"Insert Text Annotation mode: click on pdf to insert an annotation","unknown.jpg")	\
	XX(ModeInsertLinkAnnotation,"Insert Link Annotation mode: click on pdf to insert a link annotation","unknown.jpg")	\
	XX(ModeDeleteAnnotation, "Delete annotation mode: click on annotation to delete it","unknown.jpg")	\
	XX(ModeEmitPosition, "Emit position mode: Pdf is waiting for position. Click somewhere in the pdf","unknown.jpg")	\
	XX(ImageMode," Not Implemented","unknown.jpg")	\
	XX(DrawMode,"NotImplemented, go away!","unknown.jpg")	\
	XX(ModeFindLastFont, "Click to select font and font size by text in PDF","unknown.jpg")\
	XX(ModeChangeAnntation, "Click on anotation to change it","unknown.jpg") \
	XX(DefaultMode,"Open an PDF","unknown.jpg") \
	XX(ModeHighlighComment,"After selecting text, fill please content","unknown.jpg") \
	

#define ENUMS(a,b,c) a,

#define EARRAY(a,b,c) { b,c },

enum Mode
{
	MODES(ENUMS)
	NModes
};
enum PageDrawMode
{
	ModeTrackCompleteRectangle,
	ModeDrawNothing,
	ModeTrackDrawingRect
};

typedef boost::shared_ptr<pdfobjects::CAnnotation> PdfAnnot;
typedef boost::shared_ptr<pdfobjects::CPdf> PdfInstance;
typedef boost::shared_ptr<pdfobjects::IProperty> PdfProperty;
typedef std::vector<boost::shared_ptr<pdfobjects::PdfOperator> > Ops;
typedef std::list<boost::shared_ptr<pdfobjects::PdfOperator> > OpsList;
typedef boost::shared_ptr<pdfobjects::PdfOperator> PdfOp;
typedef pdfobjects::PdfOperator::BBox BBox;
typedef boost::shared_ptr<pdfobjects::UnknownCompositePdfOperator> PdfComp;

#endif // __TYPEDEFS__
