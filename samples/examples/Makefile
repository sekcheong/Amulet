##
##  Set your AMULET_DIR environment variable to the top-level directory for
##  the Amulet hierarchy -- all other paths are computed from this.
##
##  Set your AMULET_VARS_FILE environment variable to the name of the file
##  that describes the compiler flags you want to use.  The value of this
##  variable is the name of a file in amulet/bin
##    Makefile.vars.<compiler>.<platform>
##
##  If an appropriate Makefile does not exist for your compiler/platform
##  customize Makefile.vars.custom

## The default build has two parameters: OP and LIB_MODIFIER
## used to specify the variant of the Amulet library that will be used
## OP chooses one of the sets of compiler flags, defined in
## AMULET_VARS_FILE: AM_DEVELOP, AM_RELEASE, AM_INHOUSE, AM_NODEBUGSYM
OP=$(AM_DEVELOP)

## LIB_MODIFIER is appended to library filename to indicate particular
## OP choice: blank for AM_DEVELOP, -release for AM_RELEASE
##	      -inhouse for AM_INHOUSE 
LIB_MODIFIER=

## LIB_MAKE: chooses the platform-specific library makefile.  Used to 
## select static or shared linking.
LIB_MAKE=$(AM_SHARED_LIB_MAKE)

## To link with the release library, invoke make as follows
##	Make OP='$(AM_RELEASE)' LIB_MODIFIER=-release

## Sets pathname variables and loads $(AMULET_VARS_FILE) to set compiler
## variables
include $(AMULET_DIR)/bin/Makefile.vars

## abbreviations
OUT=$(AM_EXE_OUT)
O=$(AM_OBJ_SUFFIX)
E=$(AM_EXE_SUFFIX)

default:all

CFLAGS = $(OP) $(AM_CFLAGS)
.SUFFIXES: .cc

# include the platform-specific file
include $(AMULET_DIR)/bin/$(LIB_MAKE)

############################################################

all: example1$(E) example2$(E) hello$(E) goodbye_inter$(E) goodbye_button$(E)

clean:
	$(RM) example1$(E) example2$(E) hello$(E) goodbye_inter$(E) goodbye_button$(E) *$(O)

############################################################

example1$(E): example1$(O) $(STATIC_ALL)
	$(CC) $(CFLAGS) example1$(O) $(USES_ALL) \
	$(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)example1$(E)

example1$(O): example1.cc


############################################################

example2$(E): example2$(O) example2db$(O)
	$(CC) $(CFLAGS) example2$(O) example2db$(O) $(USES_ALL) \
	$(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)example2$(E)	

example2$(O): example2.cc

example2db$(O): example2db.cc

############################################################

hello$(E): hello$(O)
	$(CC) $(CFLAGS) hello$(O) $(USES_ALL)\
	$(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)hello$(E)

hello$(O): hello.cc

############################################################

goodbye_inter$(E): goodbye_inter$(O)
	$(CC) $(CFLAGS) goodbye_inter$(O) $(USES_ALL) \
	$(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)goodbye_inter$(E)

goodbye_inter$(O): goodbye_inter.cc

############################################################

goodbye_button$(E): goodbye_button$(O)
	$(CC) $(CFLAGS) goodbye_button$(O) $(USES_ALL) \
	$(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)goodbye_button$(E)

goodbye_button$(O): goodbye_button.cc

############################################################

