#For Debian compilation, uncomment the 2 lines below 
#HELPFILE= -DDEFAULT_HELPFILE=\"/usr/share/doc/seaview/seaview.html\"
#PHYMLNAME= -DPHYMLNAME=\"phyml\"


#to compile with PDF (through PDFlib Lite) rather than PostScript output, 
# uncomment next 4 lines and adapt next 2 to your local file system 
#PDF_INC = $(HOME)/PDFlib-Lite-6.0.1/libs #customize by setting to place of pdflib.h
#PDF_LIB = $(HOME)/PDFlib-Lite-6.0.1/libs #customize by setting to place of libpdf.*
#PDF_PS_FLAGS = -I$(PDF_INC)
#LPDF = -L$(PDF_LIB) -lpdf


#to use your own, uninstalled FLTK library,
#uncomment the next 3 lines and adapt the 1st to your local file system 
#FLTK = $(HOME)/fltk-1.3
#IFLTK = -I$(FLTK)
#CONFIG = $(FLTK)/fltk-config


CONFIG ?= fltk-config
PDF_PS_FLAGS ?= -DNO_PDF

EXTRALIBS = `$(CONFIG) --use-images --ldflags` -ldl -lpthread -lz

CC = gcc
CXX = g++
CSRC = csrc

#DEBUG = -g
#OPT = -O0

OPT ?= -O3

SVFLAGS = $(CPPFLAGS) $(CFLAGS) -Dunix $(OPT) -I. $(IFLTK) -I$(CSRC) $(DEBUG) $(HELPFILE) $(PHYMLNAME) $(PDF_PS_FLAGS)

OBJECTS = seaview.o custom.o use_mase_files.o regions.o load_seq.o align.o xfmatpt.o \
comlines.o resource.o nexus.o \
viewasprots.o racnuc_fetch.o concatenate.o statistics.o \
trees.o treedraw.o addbootstrap.o least_squares_brl.o \
pseudoterminal.o unrooted.o pdf_or_ps.o svg.o threads.o tbe.o Fl_Native_File_Chooser_FLTK.o
 
COBJECTS = raa_acnuc.o parser.o md5.o zsockr.o misc_acnuc.o dnapars.o protpars.o seq.o phylip.o lwl.o bionj.o phyml_util.o

seaview : $(OBJECTS) $(COBJECTS)
	$(CXX) -o $@ $(DEBUG) $(LDFLAGS) $(OBJECTS) $(COBJECTS) $(LPDF) $(EXTRALIBS) 

$(COBJECTS) : $(CSRC)/$<
	$(CC) -c $(DEBUG) $(OPT) $(CFLAGS) -Dunix -I$(CSRC) $(CSRC)/$*.c

Fl_Native_File_Chooser_FLTK.o : FL/Fl_Native_File_Chooser_FLTK.cxx
	$(CXX)  -c -I. $(SVFLAGS) FL/Fl_Native_File_Chooser_FLTK.cxx

.SUFFIXES:	.c .cxx .h .o

.cxx.o :
	$(CXX) -c  $(SVFLAGS) $<
