PROJNAME = leabra
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/leabra.h

HEADS =\
	leabra_TA_type.h \
	leabra_TA_inst.h \
	leabra.h

SRCS =\
	leabra.cpp \
	leabra_TA.cpp \
	main.cpp

include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(leabra@PDP_SUFFIX@_la_CPPFLAGS)

bin_PROGRAMS = leabra@PDP_SUFFIX@++
leabra@PDP_SUFFIX@___SOURCES = $(HEADS) $(SRCS)
leabra@PDP_SUFFIX@___LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/ta/libta@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/css/libcss@PDP_SUFFIX@.la

leabra@PDP_SUFFIX@___CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css

leabra@PDP_SUFFIX@___CXXFLAGS =

AM_CXXFLAGS = -fno-implicit-templates

BUILT_SOURCES = leabra_TA_type.h leabra_TA_inst.h leabra_TA.cpp

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

leabra@PDP_SUFFIX@___LDADD +=\
	$(abs_top_builddir)/src/taiqtso/libtaiqtso.la \
	@X_EXTRA_LIBS@ \
	@X_PRE_LIBS@ \
	@QT_LDADD@
	
leabra@PDP_SUFFIX@___LDFLAGS = @X_LIBS@

leabra@PDP_SUFFIX@___CPPFLAGS +=\
	-I@srcdir@/../taiqtso \
	@X_CFLAGS@ \
	-I/usr/include/freetype2 \
	@QT_CXXFLAGS@ \
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

leabra@PDP_SUFFIX@___LDFLAGS += @X_LIBS@

endif
#**************************************************
#
#                      NOGUI
#
#**************************************************
if NO_TA_GUI

leabra@PDP_SUFFIX@___CPPFLAGS += -DTA_NO_GUI

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

leabra@PDP_SUFFIX@___CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1 \
	-DRETSIGTYPE=void

leabra@PDP_SUFFIX@___CXXFLAGS += -Wno-non-virtual-dtor

endif
#**************************************************
#
#                      MPI
#
#**************************************************
if MPI

leabra@PDP_SUFFIX@___CPPFLAGS += -DDMEM_COMPILE

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

leabra@PDP_SUFFIX@___CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

leabra@PDP_SUFFIX@___CPPFLAGS += -DLINUX -DDARWIN

endif

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~