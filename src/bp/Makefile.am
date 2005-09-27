PROJNAME = bp
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/../bp/bp.h \
	@abs_srcdir@/../bp/rbp.h

HEADS =\
	bp_TA_type.h \
	bp_TA_inst.h \
	bp.h \
	rbp.h

SRCS =\
	bp.cpp \
	rbp.cpp \
	bp_TA.cpp \
	main.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am

bin_PROGRAMS = bp@PDP_SUFFIX@++
bp@PDP_SUFFIX@___SOURCES = $(HEADS) $(SRCS)
bp@PDP_SUFFIX@___LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/ta/libta@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/css/libcss@PDP_SUFFIX@.la

bp@PDP_SUFFIX@___LDFLAGS =
bp@PDP_SUFFIX@___CXXFLAGS =

bp@PDP_SUFFIX@___CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css

BUILT_SOURCES = bp_TA_type.h bp_TA_inst.h bp_TA.cpp

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI
bp@PDP_SUFFIX@___LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la

bp@PDP_SUFFIX@___LDADD +=\
	$(abs_top_builddir)/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la \
	@X_EXTRA_LIBS@ \
	@X_PRE_LIBS@ \
	@QT_LDADD@
	
bp@PDP_SUFFIX@___LDFLAGS += @X_LIBS@

bp@PDP_SUFFIX@___CPPFLAGS +=\
	-I@srcdir@/../taiqtso \
	@X_CFLAGS@ \
	-I/usr/include/freetype2 \
	@QT_CXXFLAGS@ \
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

bp@PDP_SUFFIX@___LDFLAGS += @X_LIBS@

endif
#**************************************************
#
#                      NOGUI
#
#**************************************************
if NO_TA_GUI

bp@PDP_SUFFIX@___CPPFLAGS += -DTA_NO_GUI

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

bp@PDP_SUFFIX@___CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1 \
	-DRETSIGTYPE=void

bp@PDP_SUFFIX@___CXXFLAGS += -Wno-non-virtual-dtor

endif
#**************************************************
#
#                      MPI
#
#**************************************************
if MPI

bp@PDP_SUFFIX@___CPPFLAGS += -DDMEM_COMPILE

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

bp@PDP_SUFFIX@___CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

bp@PDP_SUFFIX@___CPPFLAGS += -DLINUX -DDARWIN

endif

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~