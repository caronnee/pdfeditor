#========================================================================
#
# Xpdf Makefile
#
# Copyright 1996-2003 Glyph & Cog, LLC
#
#========================================================================

# includes basic building rules
# REL_ADDR has to be defined, because Makefile.rules refers 
# to the Makefile.flags
REL_ADDR = ../../
include $(REL_ADDR)/Makefile.rules

CFLAGS   += $(EXTRA_XPDF_CFLAGS)
CXXFLAGS += $(EXTRA_XPDF_CXXFLAGS)

CXX_SRC = \
	Annot.cc \
	Array.cc \
	BuiltinFont.cc \
	BuiltinFontTables.cc \
	CMap.cc \
	Catalog.cc \
	CharCodeToUnicode.cc \
	Decrypt.cc \
	Dict.cc \
	Error.cc \
	FontEncodingTables.cc \
	Function.cc \
	Gfx.cc \
	GfxFont.cc \
	GfxState.cc \
	GlobalParams.cc \
	ImageOutputDev.cc \
	JArithmeticDecoder.cc \
	JBIG2Stream.cc \
	JPXStream.cc \
	Lexer.cc \
	Link.cc \
	NameToCharCode.cc \
	Object.cc \
	Outline.cc \
	OutputDev.cc \
	PDFDocEncoding.cc \
	PSOutputDev.cc \
	PSTokenizer.cc \
	Page.cc \
	Parser.cc \
	PreScanOutputDev.cc \
	SecurityHandler.cc \
	SplashOutputDev.cc \
	Stream.cc \
	TextOutputDev.cc \
	UnicodeMap.cc \
	UnicodeTypeTable.cc \
	XRef.cc \
	encrypt_utils.cc
HEADERS = \
	Annot.h \
	Array.h \
	BuiltinFont.h \
	BuiltinFontTables.h \
	CMap.h \
	Catalog.h \
	CharCodeToUnicode.h \
	CharTypes.h \
	CompactFontTables.h \
	Decrypt.h \
	Dict.h \
	Error.h \
	ErrorCodes.h \
	FontEncodingTables.h \
	Function.h \
	Gfx.h \
	GfxFont.h \
	GfxState.h \
	GlobalParams.h \
	ImageOutputDev.h \
	JArithmeticDecoder.h \
	JBIG2Stream.h \
	JPXStream.h \
	Lexer.h \
	Link.h \
	NameToCharCode.h \
	NameToUnicodeTable.h \
	Object.h \
	Outline.h \
	OutputDev.h \
	PDFCore.h \
	PDFDocEncoding.h \
	PSOutputDev.h \
	PSTokenizer.h \
	Page.h \
	Parser.h \
	PreScanOutputDev.h \
	SecurityHandler.h \
	SplashOutputDev.h \
	Stream-CCITT.h \
	Stream.h \
	TextOutputDev.h \
	UTF8.h \
	UnicodeMap.h \
	UnicodeMapTables.h \
	UnicodeTypeTable.h \
	XRef.h \
	XpdfPluginAPI.h \
	config.h \
#------------------------------------------------------------------------

all: all-no-x

all-no-x: libxpdf

#------------------------------------------------------------------------

#libxpdf

LIBXPDF_OBJS = \
Annot.o \
Array.o \
BuiltinFont.o \
BuiltinFontTables.o \
CMap.o \
Catalog.o \
CharCodeToUnicode.o \
Decrypt.o \
Dict.o \
Error.o \
FontEncodingTables.o \
Function.o \
Gfx.o \
GfxFont.o \
GfxState.o \
GlobalParams.o \
ImageOutputDev.o \
JArithmeticDecoder.o \
JBIG2Stream.o \
JPXStream.o \
Lexer.o \
Link.o \
NameToCharCode.o \
Object.o \
Outline.o \
OutputDev.o \
PDFDocEncoding.o \
PSOutputDev.o \
PSTokenizer.o \
Page.o \
Parser.o \
PreScanOutputDev.o \
SecurityHandler.o \
SplashOutputDev.o \
Stream.o \
TextOutputDev.o \
UnicodeMap.o \
UnicodeTypeTable.o \
XRef.o \
XpdfPluginAPI.o \
encrypt_utils.o

# All objects which are not a part of libxpdf but which are necessary for
# libxpdf-extra
LIBXPDF_EXTRA_OBJS = PDFDoc.o

TARGET = $(LIBPREFIX)xpdf.a
libxpdf: $(TARGET)

TARGET_EXTRA = $(LIBPREFIX)xpdf-extra.a

staticlib: libxpdf

extralib: $(TARGET_EXTRA)

$(TARGET): deps $(LIBXPDF_OBJS)
	$(DEL_FILE) $(TARGET)
	$(AR) $(TARGET) $(LIBXPDF_OBJS)
	$(RANLIB) $(TARGET)

# Extra library for internal purposes - not distributed
$(TARGET_EXTRA): $(LIBXPDF_EXTRA_OBJS)
	$(DEL_FILE) $(TARGET_EXTRA)
	$(AR) $(TARGET_EXTRA) $(LIBXPDF_EXTRA_OBJS)
	$(RANLIB) $(TARGET_EXTRA)

#------------------------------------------------------------------------

SecurityHandler.x.o: SecurityHandler.cc
	$(CXX) $(CXXFLAGS) -DHAVE_XPDFCORE -c -o SecurityHandler.x.o \
		SecurityHandler.cc

.PHONY: install-dev uninstall-dev
install-dev: staticlib $(DEV_CONFIG)
	$(MKDIR) $(INSTALL_ROOT)$(INCLUDE_PATH)/xpdf
	$(COPY_FILE) $(HEADERS) $(INSTALL_ROOT)$(INCLUDE_PATH)/xpdf
	$(MKDIR) $(INSTALL_ROOT)$(LIB_PATH)/xpdf
	$(COPY_FILE) $(TARGET) $(INSTALL_ROOT)$(LIB_PATH)/xpdf

uninstall-dev:
	cd $(INSTALL_ROOT)$(INCLUDE_PATH)/xpdf && $(DEL_FILE) $(HEADERS)
	$(DEL_DIR)  $(INSTALL_ROOT)$(INCLUDE_PATH)/xpdf/
	cd $(INSTALL_ROOT)$(LIB_PATH)/xpdf && $(DEL_FILE) $(TARGET)
	$(DEL_DIR)  $(INSTALL_ROOT)$(LIB_PATH)/xpdf/

#------------------------------------------------------------------------

clean:
	$(DEL_FILE) *.o
	$(DEL_FILE) $(TARGET) $(TARGET_EXTRA) deps

#------------------------------------------------------------------------

deps: $(HEADERS)
	$(CXX) $(MANDATORY_INCPATH) -M -MF deps $(CXX_SRC)

# this requires gnu make (or compatible) because deps file doesn't
# exist in time when invoked for the first time and thus has to
# be generated
include deps
