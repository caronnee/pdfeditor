#############################################################################
# Makefile for building: guiEva
# Generated by qmake (2.01a) (Qt 4.6.3) on: Sun Nov 7 16:04:03 2010
# Project:  guiEva.pro
# Template: app
# Command: /usr/bin/qmake -unix -o Makefile guiEva.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I. -I. -I/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/xpdf/ -I/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/
LINK          = g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/usr/lib/qt4
LIBS          = $(SUBLIBS)  -L/usr/lib/qt4 -lQtGui -L/usr/lib/qt4 -L/usr/X11R6/lib -lQtCore -lgthread-2.0 -lrt -lglib-2.0 -lpthread \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/kernel/  \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/utils/ \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/xpdf/fofi \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/xpdf/goo \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/xpdf/splash \
-L/afs/ms/u/p/peske5am/BIG/pdfGUI/pdfedit-0.4.5/src/xpdf/xpdf \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lkernel -lutils -lfofi -lGoo -lsplash -lxpdf -lxpdf-extra \
-lt1 -lfreetype
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		MyWidget.cpp \
		openpdf.cpp \
		pdfgui.cpp \
		TabPage.cpp moc_MyWidget.cpp \
		moc_openpdf.cpp \
		moc_pdfgui.cpp \
		moc_TabPage.cpp
OBJECTS       = main.o \
		MyWidget.o \
		openpdf.o \
		pdfgui.o \
		TabPage.o \
		moc_MyWidget.o \
		moc_openpdf.o \
		moc_pdfgui.o \
		moc_TabPage.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		guiEva.pro
QMAKE_TARGET  = guiEva
DESTDIR       = 
TARGET        = guiEva

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_OpenPdf.h ui_pdfgui.h ui_saveBrach.h ui_showPage.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: guiEva.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/qt4/libQtGui.prl \
		/usr/lib/qt4/libQtCore.prl
	$(QMAKE) -unix -o Makefile guiEva.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/qt4/libQtGui.prl:
/usr/lib/qt4/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile guiEva.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/guiEva1.0.0 || $(MKDIR) .tmp/guiEva1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/guiEva1.0.0/ && $(COPY_FILE) --parents MyWidget.h openpdf.h pdfgui.h TabPage.h .tmp/guiEva1.0.0/ && $(COPY_FILE) --parents main.cpp MyWidget.cpp openpdf.cpp pdfgui.cpp TabPage.cpp .tmp/guiEva1.0.0/ && $(COPY_FILE) --parents OpenPdf.ui pdfgui.ui saveBrach.ui showPage.ui .tmp/guiEva1.0.0/ && (cd `dirname .tmp/guiEva1.0.0` && $(TAR) guiEva1.0.0.tar guiEva1.0.0 && $(COMPRESS) guiEva1.0.0.tar) && $(MOVE) `dirname .tmp/guiEva1.0.0`/guiEva1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/guiEva1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_MyWidget.cpp moc_openpdf.cpp moc_pdfgui.cpp moc_TabPage.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_MyWidget.cpp moc_openpdf.cpp moc_pdfgui.cpp moc_TabPage.cpp
moc_MyWidget.cpp: MyWidget.h
	/usr/bin/moc $(DEFINES) $(INCPATH) MyWidget.h -o moc_MyWidget.cpp

moc_openpdf.cpp: openpdf.h
	/usr/bin/moc $(DEFINES) $(INCPATH) openpdf.h -o moc_openpdf.cpp

moc_pdfgui.cpp: openpdf.h \
		ui_pdfgui.h \
		pdfgui.h
	/usr/bin/moc $(DEFINES) $(INCPATH) pdfgui.h -o moc_pdfgui.cpp

moc_TabPage.cpp: ui_showPage.h \
		TabPage.h
	/usr/bin/moc $(DEFINES) $(INCPATH) TabPage.h -o moc_TabPage.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_OpenPdf.h ui_pdfgui.h ui_saveBrach.h ui_showPage.h
compiler_uic_clean:
	-$(DEL_FILE) ui_OpenPdf.h ui_pdfgui.h ui_saveBrach.h ui_showPage.h
ui_OpenPdf.h: OpenPdf.ui
	/usr/bin/uic OpenPdf.ui -o ui_OpenPdf.h

ui_pdfgui.h: pdfgui.ui \
		openpdf.h
	/usr/bin/uic pdfgui.ui -o ui_pdfgui.h

ui_saveBrach.h: saveBrach.ui
	/usr/bin/uic saveBrach.ui -o ui_saveBrach.h

ui_showPage.h: showPage.ui
	/usr/bin/uic showPage.ui -o ui_showPage.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

main.o: main.cpp pdfgui.h \
		openpdf.h \
		ui_pdfgui.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

MyWidget.o: MyWidget.cpp MyWidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MyWidget.o MyWidget.cpp

openpdf.o: openpdf.cpp openpdf.h \
		TabPage.h \
		ui_showPage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o openpdf.o openpdf.cpp

pdfgui.o: pdfgui.cpp pdfgui.h \
		openpdf.h \
		ui_pdfgui.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o pdfgui.o pdfgui.cpp

TabPage.o: TabPage.cpp TabPage.h \
		ui_showPage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TabPage.o TabPage.cpp

moc_MyWidget.o: moc_MyWidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_MyWidget.o moc_MyWidget.cpp

moc_openpdf.o: moc_openpdf.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_openpdf.o moc_openpdf.cpp

moc_pdfgui.o: moc_pdfgui.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_pdfgui.o moc_pdfgui.cpp

moc_TabPage.o: moc_TabPage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_TabPage.o moc_TabPage.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

