##
##  Set your AMULET_DIR environment variable to the top-level directory for
##  the Amulet hierarchy -- all other paths are computed from this.
##
##  Set your AMULET_VARS_FILE environment variable to the name of the file
##  that describes the compiler flags you want to use.  The value of this
##  variable is the name of a file in amulet/bin/, one of:
##    Makefile.vars.gcc.Sun   -  For gcc on a Sun
##    Makefile.vars.gcc.HP    -  For gcc on an HP
##    Makefile.vars.CC.Sun    -  For ObjectCenter's CC on a Sun
##    Makefile.vars.CC.HP     -  For ObjectCenter's CC on an HP
##


# Default build has these parameters.  Override these to get different
# versions of the library (see, for instance,  the "static:" target below)

# OP: chooses one of the sets of extra compiler flags:
# AM_DEVELOP, AM_RELEASE, AM_INHOUSE, AM_NODEBUGSYM
OP=$(AM_DEVELOP)

# AM_SAMPLE_FLAGS: chooses one of the sets of extra compiler flags for
# building all the Amulet sample applications and test programs.
# Use AM_NODEBUGSYM to decrease the size of the applications on
# platforms that produce large amounts of debugging information!
AM_SAMPLE_FLAGS=$(AM_DEVELOP)

# LIB_MAKE: chooses the platform-specific library makefile.  Used to 
# select static or shared linking.
# LIB_MAKE=$(AM_STATIC_LIB_MAKE)
LIB_MAKE=$(AM_SHARED_LIB_MAKE)

# LIB_MODIFIER: appended to library filename to indicate particular OP choice.
# e.g., blank for AM_DEVELOP, -release for AM_RELEASE, -inhouse for AM_INHOUSE,
LIB_MODIFIER=


## Sets pathname variables and loads $(AMULET_VARS_FILE) to set compiler
## variables
include $(AMULET_DIR)/bin/Makefile.vars

## abbreviations
OUT=$(AM_EXE_OUT)
O=$(AM_OBJ_SUFFIX)
E=$(AM_EXE_SUFFIX)



##
##  File sets
##

# Utility classes

UTILS_MODULE =  $(UTILS_DIR)/registry$(O) $(UTILS_DIR)/misc$(O)          \
                $(UTILS_DIR)/symbol_table$(O) \
		$(UTILS_DIR)/univ_lst$(O) $(UTILS_DIR)/univ_map$(O)  \
                $(UTILS_DIR)/types$(O) $(UTILS_DIR)/stdvalue$(O)


# ORE: prototype-instance object system

ORE_MODULE = $(ORE_DIR)/dynarray$(O) $(UTILS_DIR)/standard_slots$(O)          \
             $(ORE_DIR)/object$(O) $(UTILS_DIR)/value_list$(O)                \
             $(ORE_DIR)/formula$(O) $(ORE_DIR)/web$(O)


# Gem: platform-independent graphics and IO

include $(AMULET_DIR)/bin/$(AM_GEM_SOURCE)


# Amulet: everything else
            
OPAL_MODULE = $(OPAL_DIR)/opal$(O) $(OPAL_DIR)/windows$(O) $(OPAL_DIR)/text_fns$(O)

INTER_MODULE = $(UTILS_DIR)/priority_list$(O) $(INTER_DIR)/inter_basics$(O)     \
               $(INTER_DIR)/command_basics$(O) $(INTER_DIR)/inter_move_grow$(O) \
               $(INTER_DIR)/inter_choice$(O) $(INTER_DIR)/inter_new_points$(O)  \
               $(INTER_DIR)/inter_text$(O)

DEBUG_MODULE = $(DEBUG_DIR)/inspector$(O) $(DEBUG_DIR)/tracer$(O)

WIDGETS_MODULE = $(WIDGETS_DIR)/widgets$(O) $(WIDGETS_DIR)/button_widgets$(O) \
                 $(WIDGETS_DIR)/scroll_widgets$(O) \
                 $(WIDGETS_DIR)/text_widgets$(O) \
                 $(WIDGETS_DIR)/selection_widgets$(O) \
                 $(WIDGETS_DIR)/editing_commands$(O) \
                 $(WIDGETS_DIR)/dialog_widgets$(O) \
                 $(WIDGETS_DIR)/rich_text$(O) \
                 $(WIDGETS_DIR)/undo_dialog_box$(O)

GESTURE_MODULE = $(GESTURE_DIR)/inter_gesture$(O) \
                 $(GESTURE_DIR)/gest_trainer$(O) \
                 $(GESTURE_DIR)/gest_classifier$(O) \
                 $(GESTURE_DIR)/gest_features$(O) \
                 $(GESTURE_DIR)/gest_matrix$(O)

ANIM_MODULE = $(ANIM_DIR)/anim_animators$(O) $(ANIM_DIR)/anim_constraint$(O) \
	      $(ANIM_DIR)/anim_interactor$(O)


##
## Make the Amulet library
##

# Default build:
default: amulet
lib: amulet


#
# Include the platform-specific library makefile.  This file should define:
#
#    GEM_MODULE
#       object files in Gem
#
#    AMULET_LIB, ORE_LIB, GEM_LIB, UTILS_LIB
#       the library pathnames (with rules that build them)
#
#    NEEDS_ALL, NEEDS_ORE, NEEDS_GEM
#       list of files on which a test program must depend when it needs a
#       library module
#
#    USES_ALL, USES_ORE, USES_GEM
#       linker arguments for test programs that use a library module
#   
#    STATIC_ALL, STATIC_ORE, STATIC_GEM
#       list of files which should force relinking of the test program
#   
#    .cc$(O)
#       an inference rule for compiling source files to object files
#
#

CFLAGS = $(OP) $(AM_CFLAGS) -DDEFAULT_AMULET_DIR=\"$(AMULET_ROOT)\"
.SUFFIXES: .cc

# include the platform-specific file
include $(AMULET_DIR)/bin/$(LIB_MAKE)

# Uncomment the following lines to keep the Amulet libraries from
# being rebuilt after the Amulet objects have been deleted
# AMULET_LIB_DEPENDENCIES =
# ORE_LIB_DEPENDENCIES =
# GEM_LIB_DEPENDENCIES =
# UTILS_LIB_DEPENDENCIES =

# 
# Targets for the Amulet library (and its sublibraries, if desired).
#

amulet: $(AMULET_LIB)
amobject: $(ORE_LIB)
amgem: $(GEM_LIB)
amutils: $(UTILS_LIB)

#
# can also build other versions of the Amulet library: optimized/static/combo
#

AS_DEVELOP = OP="$(AM_DEVELOP)" LIB_MODIFIER=""
AS_RELEASE = OP="$(AM_RELEASE)" LIB_MODIFIER=-release
AS_INHOUSE = OP="$(AM_INHOUSE)" LIB_MODIFIER=-inhouse
AS_STATIC =  LIB_MAKE="$(AM_STATIC_LIB_MAKE)"
AS_SHARED =  LIB_MAKE="$(AM_SHARED_LIB_MAKE)"

develop-shared:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_DEVELOP)

develop-static:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE)	$(AS_STATIC) $(AS_DEVELOP)

release-shared:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_RELEASE)

release-static:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_STATIC) $(AS_RELEASE)

inhouse-shared:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_INHOUSE)

inhouse-static:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_STATIC) $(AS_INHOUSE)

# build ALL the library versions, then clean up
# uses same object files for shared and static libraries
all-libs:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_DEVELOP)
	$(MAKE) $(AS_STATIC) $(AS_DEVELOP)
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_RELEASE)
	$(MAKE) $(AS_STATIC) $(AS_RELEASE)
	$(RM) $(ANY_OBJECT_FILE)

# build inhouse libraries using same objects for shared and static
inhouse:
	$(RM) $(ANY_OBJECT_FILE)
	$(MAKE) $(AS_SHARED) $(AS_INHOUSE)
	$(MAKE) $(AS_STATIC) $(AS_INHOUSE)

# build the libraries that make sense for a given platform
libs: $(AM_LIBS_TO_MAKE)
	$(RM) $(ANY_OBJECT_FILE)

##
##  Building test and sample programs that come with Amulet library
##

all: amulet
	$(MAKE) OP="$(AM_SAMPLE_FLAGS)" tests samples

##
##  Test programs
##

# need this in a macro so we can delete the executables when we make clean
TESTS= testdpy$(E) testgem$(E) testtrans$(E) \
	testsubwins$(E) testloop$(E) testclip$(E) testpoints$(E) \
	testwinsizes$(E) testinput$(E) testcopy$(E) \
	testobject$(E) testopal$(E) testpolyweb$(E) testlines$(E) \
	testgobs$(E) testmap$(E) testinter$(E) testaniminter$(E) \
	testwidgets$(E) testselectionwidget$(E) \
	testdragdrop$(E) testmaps$(E) testgest$(E) testanimators$(E) \
	testloadsave$(E) testfade$(E) testflip$(E) testrichtext$(E) \
	testpixmap$(E) testJIO$(E) testcoord$(E) testcursor$(E)

tests: $(TESTS)

testdpy$(E):: $(NEEDS_GEM)
testdpy$(E):: $(GEM_DIR)/testdpy$(O) $(GEM_DIR)/test_utils$(O) $(GEM_DIR)/testbits$(O)  \
    $(GEM_DIR)/testcolor$(O) $(GEM_DIR)/testfonts$(O) $(GEM_DIR)/testcolor$(O)     \
    $(GEM_DIR)/testwinprops$(O) \
    $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testdpy$(O) \
        $(GEM_DIR)/test_utils$(O) $(GEM_DIR)/testbits$(O)    \
	$(GEM_DIR)/testcolor$(O) $(GEM_DIR)/testfonts$(O)    \
	$(GEM_DIR)/testwinprops$(O)                        \
	$(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testdpy$(E)

testgem$(E):: $(NEEDS_GEM)
testgem$(E):: $(GEM_DIR)/testgem$(O) $(GEM_DIR)/test_utils$(O)  \
    $(GEM_DIR)/testlineprops$(O)     $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testgem$(O) \
        $(GEM_DIR)/test_utils$(O) $(GEM_DIR)/testlineprops$(O) \
	$(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testgem$(E)

testtrans$(E):: $(NEEDS_GEM)
testtrans$(E):: $(GEM_DIR)/testtrans$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testtrans$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testtrans$(E)

testsubwins$(E):: $(NEEDS_GEM)
testsubwins$(E):: $(GEM_DIR)/testsubwins$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testsubwins$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testsubwins$(E)

testloop$(E):: $(NEEDS_GEM)
testloop$(E):: $(GEM_DIR)/testloop$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testloop$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testloop$(E)

testclip$(E):: $(NEEDS_GEM)
testclip$(E):: $(GEM_DIR)/testclip$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testclip$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testclip$(E)

testpoints$(E):: $(NEEDS_GEM)
testpoints$(E):: $(GEM_DIR)/testpoints$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testpoints$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testpoints$(E)

testwinsizes$(E):: $(NEEDS_GEM)
testwinsizes$(E):: $(GEM_DIR)/testwinsizes$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testwinsizes$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testwinsizes$(E)

testinput$(E):: $(NEEDS_GEM)
testinput$(E):: $(GEM_DIR)/testinput$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testinput$(O) \
	$(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testinput$(E)

testcopy$(E):: $(NEEDS_GEM)
testcopy$(E):: $(GEM_DIR)/testcopy$(O) $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testcopy$(O) \
	$(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testcopy$(E)

testobject$(E):: $(NEEDS_ORE)
testobject$(E):: $(ORE_DIR)/testobject$(O) $(STATIC_ORE)
	$(PURIFY) $(CC) $(CFLAGS) $(ORE_DIR)/testobject$(O) \
	$(USES_ORE) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testobject$(E)

testopal$(E):: $(NEEDS_ALL)
testopal$(E):: $(OPAL_DIR)/testopal$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testopal$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testopal$(E)

testcoord$(E):: $(NEEDS_ALL)
testcoord$(E):: $(OPAL_DIR)/testcoord$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testcoord$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testcoord$(E)

testcursor$(E):: $(NEEDS_ALL)
testcursor$(E):: $(OPAL_DIR)/testcursor$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testcursor$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testcursor$(E)

testJIO$(E):: $(NEEDS_ALL)
testJIO$(E):: $(OPAL_DIR)/testJIO$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testJIO$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testJIO$(E)

testpolyweb$(E):: $(NEEDS_ALL)
testpolyweb$(E):: $(OPAL_DIR)/testpolyweb$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testpolyweb$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testpolyweb$(E)

testlines$(E):: $(NEEDS_ALL)
testlines$(E):: $(OPAL_DIR)/testlines$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testlines$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testlines$(E)

testoffset$(E):: $(NEEDS_ALL)
testoffset$(E):: $(OPAL_DIR)/testoffset$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testoffset$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testoffset$(E)

testgobs$(E):: $(NEEDS_ALL)
testgobs$(E):: $(OPAL_DIR)/testgobs$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testgobs$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testgobs$(E)

testmap$(E):: $(NEEDS_ALL)
testmap$(E):: $(OPAL_DIR)/testmap$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testmap$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testmap$(E)

testloadsave$(E):: $(NEEDS_ALL)
testloadsave$(E):: $(UTILS_DIR)/testloadsave$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) \
        $(UTILS_DIR)/testloadsave$(O) $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testloadsave$(E)

testinter$(E):: $(NEEDS_ALL)
testinter$(E):: $(INTER_DIR)/testinter$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(INTER_DIR)/testinter$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testinter$(E)

testaniminter$(E):: $(NEEDS_ALL)
testaniminter$(E):: $(ANIM_DIR)/testaniminter$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) \
        $(ANIM_DIR)/testaniminter$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testaniminter$(E)

testwidgets$(E):: $(NEEDS_ALL)  
testwidgets$(E):: $(WIDGETS_DIR)/testwidgets$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(WIDGETS_DIR)/testwidgets$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testwidgets$(E)

testselectionwidget$(E):: $(NEEDS_ALL)
testselectionwidget$(E):: $(WIDGETS_DIR)/testselectionwidget$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) \
	$(WIDGETS_DIR)/testselectionwidget$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testselectionwidget$(E)

testdragdrop$(E):: $(NEEDS_ALL)
testdragdrop$(E):: $(WIDGETS_DIR)/testdragdrop$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) \
	$(WIDGETS_DIR)/testdragdrop$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testdragdrop$(E)

testmaps$(E):: $(NEEDS_GEM)
testmaps$(E):: $(UTILS_DIR)/testmaps$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(UTILS_DIR)/testmaps$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testmaps$(E)

testgest$(E):: $(NEEDS_ALL)
testgest$(E):: $(GESTURE_DIR)/testgest$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(GESTURE_DIR)/testgest$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testgest$(E)

testfade$(E):: $(NEEDS_ALL)
testfade$(E):: $(OPAL_DIR)/testfade$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testfade$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testfade$(E)

testflip$(E):: $(NEEDS_ALL)
testflip$(E):: $(OPAL_DIR)/testflip$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(OPAL_DIR)/testflip$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testflip$(E)

testanimators$(E):: $(NEEDS_ALL)
testanimators$(E):: $(ANIM_DIR)/testanimators$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(ANIM_DIR)/testanimators$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testanimators$(E)

testrichtext$(E):: $(NEEDS_ALL)
testrichtext$(E):: $(WIDGETS_DIR)/testrichtext$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(WIDGETS_DIR)/testrichtext$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testrichtext$(E)

testpixmap$(E):: $(NEEDS_ALL)
testpixmap$(E):: $(GEM_DIR)/testpixmap$(O) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testpixmap$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testpixmap$(E)

# General test procedure which can be used for testbugs.cc in the
# current directory

testbugs$(E):: $(NEEDS_ALL)
testbugs$(E):: testbugs$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) testbugs$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testbugs$(E)

testbug2$(E):: $(NEEDS_ALL)
testbug2$(E):: testbug2$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) testbug2$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testbug2$(E)


# obsolete, broken, or never-checked-in tests

testMS1$(E):: $(NEEDS_GEM)
testMS1$(E):: $(GEM_DIR)/testMS1$(O)  $(STATIC_GEM)
	$(PURIFY) $(CC) $(CFLAGS) $(GEM_DIR)/testMS1$(O) \
        $(USES_GEM) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testMS1$(E)

testpos$(E):: $(NEEDS_ALL)
testpos$(E):: $(UTILS_DIR)/testpos$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(UTILS_DIR)/testpos$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testpos$(E)

testname$(E):: $(NEEDS_ALL)
testname$(E):: $(UTILS_DIR)/testname$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(UTILS_DIR)/testname$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testname$(E)

testtiming$(E):: $(NEEDS_ALL)
testtiming$(E):: $(ANIM_DIR)/testtiming$(O)   $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(ANIM_DIR)/testtiming$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testtiming$(E)

testinplacetext$(E):: $(NEEDS_ALL)
testinplacetext$(E):: $(INTER_DIR)/testinplacetext$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) \
        $(INTER_DIR)/testinplacetext$(O) \
	$(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)testinplacetext$(E)


##  
## Sample programs
##

# need this in a macro so we can delete the executables when we make clean
SAMPLES=agate$(E) av$(E) checkers$(E) circuit$(E) color$(E) hsv$(E) \
	hello$(E) goodbye_inter$(E) goodbye_button$(E)     \
	space$(E) space2$(E) timing$(E) tutorial$(E)        \
	tree$(E) gilt$(E) example1$(E) example2$(E) 

samples: $(SAMPLES)

agate$(E):: $(NEEDS_ALL)
agate$(E):: $(SAMPLES_DIR)/agate/agate$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/agate/agate$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)agate$(E)

av$(E):: $(NEEDS_ALL)
av$(E):: $(SAMPLES_DIR)/av$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/av$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)av$(E)

checkers$(E):: $(NEEDS_ALL)
checkers$(E):: $(SAMPLES_DIR)/checkers$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/checkers$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)checkers$(E)

circuit$(E):: $(NEEDS_ALL)
circuit$(E):: $(SAMPLES_DIR)/circuit/circuit$(O) \
 	 $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/circuit/circuit$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)circuit$(E)

color$(E):: $(NEEDS_ALL)
color$(E):: $(SAMPLES_DIR)/color$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/color$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)color$(E)

hsv$(E):: $(NEEDS_ALL)
hsv$(E):: $(SAMPLES_DIR)/hsv$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/hsv$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)hsv$(E)

hello$(E):: $(NEEDS_ALL)
hello$(E):: $(SAMPLES_DIR)/examples/hello$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/examples/hello$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)hello$(E)

goodbye_inter$(E):: $(NEEDS_ALL)
goodbye_inter$(E):: $(SAMPLES_DIR)/examples/goodbye_inter$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/examples/goodbye_inter$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)goodbye_inter$(E)

goodbye_button$(E):: $(NEEDS_ALL)
goodbye_button$(E):: $(SAMPLES_DIR)/examples/goodbye_button$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/examples/goodbye_button$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)goodbye_button$(E)

space$(E):: $(NEEDS_ALL)
space$(E):: $(SAMPLES_DIR)/space$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/space$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)space$(E)

space2$(E):: $(NEEDS_ALL)
space2$(E):: $(SAMPLES_DIR)/space2$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/space2$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)space2$(E)

timing$(E):: $(NEEDS_ALL)
timing$(E):: $(SAMPLES_DIR)/timing/timing$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/timing/timing$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)timing$(E)
$(SAMPLES_DIR)/timing/timing$(O): $(SAMPLES_DIR)/timing/timing.cc
	$(CC) $(AM_RELEASE) $(AM_CFLAGS) $(AM_OBJ_OUT)$(SAMPLES_DIR)/timing/timing$(O) -c $(AM_SRC_IN)$(SAMPLES_DIR)/timing/timing.cc

tutorial$(E):: $(NEEDS_ALL)
tutorial$(E):: $(SAMPLES_DIR)/tutorial/tutorial$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/tutorial/tutorial$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)tutorial$(E)

tree$(E):: $(NEEDS_ALL)
tree$(E):: $(SAMPLES_DIR)/tree$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/tree$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)tree$(E)

example1$(E):: $(NEEDS_ALL)
example1$(E):: $(SAMPLES_DIR)/examples/example1$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/examples/example1$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)example1$(E)

example2$(E):: $(NEEDS_ALL)
example2$(E):: $(SAMPLES_DIR)/examples/example2$(O) \
	       $(SAMPLES_DIR)/examples/example2db$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/examples/example2$(O) \
	$(SAMPLES_DIR)/examples/example2db$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)example2$(E)

GILT_MODULE =	$(SAMPLES_DIR)/gilt/gilt$(O) 		\
		$(SAMPLES_DIR)/gilt/namelabel$(O) 	\
		$(SAMPLES_DIR)/gilt/nameonly$(O)	\
		$(SAMPLES_DIR)/gilt/numberinput$(O)	\
		$(SAMPLES_DIR)/gilt/savecpp$(O)	\
		$(SAMPLES_DIR)/gilt/borderrect$(O)	\
		$(SAMPLES_DIR)/gilt/panel$(O)		\
		$(SAMPLES_DIR)/gilt/scrollgroup$(O)	\
		$(SAMPLES_DIR)/gilt/externs$(O)		\
		$(SAMPLES_DIR)/gilt/aboutgilt$(O)	\
		$(SAMPLES_DIR)/gilt/cpp_out$(O)

gilt$(E):: $(NEEDS_ALL)
gilt$(E):: $(GILT_MODULE) $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(GILT_MODULE) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)gilt$(E)


# unreleased sample programs

map$(E):: $(NEEDS_ALL)
map$(E):: $(SAMPLES_DIR)/map/map$(O)  $(STATIC_ALL)
	$(PURIFY) $(CC) $(CFLAGS) $(SAMPLES_DIR)/map/map$(O) \
        $(USES_ALL) $(AM_LIB_LDFLAGS) $(AM_LIBS) $(OUT)map$(E)



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# Clean up non-source files
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# remove executables, object files, and libraries
clean: noobj noexe nolib

# remove only $(O) files, leaving executables and libraries
noobj: 
	-$(RM) $(ANY_OBJECT_FILE)
nodoto: noobj

# remove only executables
noexe: 
	-$(RM) $(ANY_TEST_PROGRAM)

#remove only lib files
nolib:
	-$(RM) $(ANY_LIBRARY)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# Update dependencies
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


# This rule updates the dependencies appearing after the "Do Not Delete"
# line below.
# makedepend produces full local pathnames, but a little perl makes them
# refer to $(TOP_DIR) instead.

depend:
	../scripts/makedepend -- $(CFLAGS) -- -o'$$(O)' -C'$(CC) -E' \
	   $(SRC_DIR)/*/*.cc $(SAMPLES_DIR)/*.cc $(SAMPLES_DIR)/*/*.cc \
	   > /dev/null 2>&1
	
tags:
	etags -T -o $(SRC_DIR)/TAGS $(SRC_DIR)/*/*.cc $(SRC_DIR)/*/*.h $(INC_DIR)/*.h

### Patch since these dependencies exist only when compiled with gcc
### (so running "make depend" with Makefile.vars.CC.* won't find them)

$(TOP_DIR)/src/utils/univ_map$(O): $(TOP_DIR)/include/amulet/types.h
$(TOP_DIR)/src/utils/symbol_table$(O): $(TOP_DIR)/include/amulet/types.h


# DO NOT DELETE THIS LINE -- make depend depends on it.
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/anim/anim_animators$(O): $(AMULET_DIR)/src/anim/anim_animators.cc
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/anim_constraint$(O): $(AMULET_DIR)/src/anim/anim_constraint.cc
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/anim_interactor$(O): $(AMULET_DIR)/src/anim/anim_interactor.cc
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/anim/testanimators$(O): $(AMULET_DIR)/src/anim/testanimators.cc
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/testaniminter$(O): $(AMULET_DIR)/src/anim/testaniminter.cc
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/anim/testtiming$(O): $(AMULET_DIR)/src/anim/testtiming.cc
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/formula_advanced.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/debug/inspector$(O): $(AMULET_DIR)/src/debug/inspector.cc
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/debug/tracer$(O): $(AMULET_DIR)/src/debug/tracer.cc
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_cursor$(O): $(AMULET_DIR)/src/gem/gemM_cursor.cc
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_draw$(O): $(AMULET_DIR)/src/gem/gemM_draw.cc
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/src/gem/gemM_image.cc
$(AMULET_DIR)/src/gem/gemM_image$(O): $(AMULET_DIR)/src/gem/gem_GIF.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_input$(O): $(AMULET_DIR)/src/gem/gemM_input.cc
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_points$(O): $(AMULET_DIR)/src/gem/gemM_points.cc
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_regions$(O): $(AMULET_DIR)/src/gem/gemM_regions.cc
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_styles$(O): $(AMULET_DIR)/src/gem/gemM_styles.cc
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_text$(O): $(AMULET_DIR)/src/gem/gemM_text.cc
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_time$(O): $(AMULET_DIR)/src/gem/gemM_time.cc
$(AMULET_DIR)/src/gem/gemM_wdef$(O): $(AMULET_DIR)/src/gem/gemM_wdef.cc
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemM_windows$(O): $(AMULET_DIR)/src/gem/gemM_windows.cc
$(AMULET_DIR)/src/gem/gemW_bmpimage$(O): $(AMULET_DIR)/src/gem/gemW_32.h
$(AMULET_DIR)/src/gem/gemW_bmpimage$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.cc
$(AMULET_DIR)/src/gem/gemW_bmpimage$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.h
$(AMULET_DIR)/src/gem/gemW_bmpimage$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_clean$(O): $(AMULET_DIR)/src/gem/gemW_clean.cc
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_cursor.cc
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_cursor.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_image.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_pattern.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_styles.h
$(AMULET_DIR)/src/gem/gemW_cursor$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_cursor.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_draw.cc
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_draw.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_image.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_input.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_line.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_points.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_regions.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_styles.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_text.h
$(AMULET_DIR)/src/gem/gemW_draw$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_gifimage$(O): $(AMULET_DIR)/src/gem/gemW_32.h
$(AMULET_DIR)/src/gem/gemW_gifimage$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.cc
$(AMULET_DIR)/src/gem/gemW_gifimage$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.h
$(AMULET_DIR)/src/gem/gemW_gifimage$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_image.cc
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_image.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_pattern.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_styles.h
$(AMULET_DIR)/src/gem/gemW_image$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/src/gem/gemW_32.h
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/src/gem/gemW_input.cc
$(AMULET_DIR)/src/gem/gemW_input$(O): $(AMULET_DIR)/src/gem/gemW_input.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/src/gem/gemW_32.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/src/gem/gemW_line.cc
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/src/gem/gemW_line.h
$(AMULET_DIR)/src/gem/gemW_line$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/src/gem/gemW_misc.cc
$(AMULET_DIR)/src/gem/gemW_misc$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/src/gem/gemW_points.cc
$(AMULET_DIR)/src/gem/gemW_points$(O): $(AMULET_DIR)/src/gem/gemW_points.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/src/gem/gemW_regions.cc
$(AMULET_DIR)/src/gem/gemW_regions$(O): $(AMULET_DIR)/src/gem/gemW_regions.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_bmpimage.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_gifimage.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_image.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_styles.cc
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_styles.h
$(AMULET_DIR)/src/gem/gemW_styles$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/src/gem/gemW.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/src/gem/gemW_clean.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/src/gem/gemW_misc.h
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/src/gem/gemW_text.cc
$(AMULET_DIR)/src/gem/gemW_text$(O): $(AMULET_DIR)/src/gem/gemW_text.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/src/gem/gemW_time.cc
$(AMULET_DIR)/src/gem/gemW_time$(O): $(AMULET_DIR)/src/gem/gemW_time.h
$(AMULET_DIR)/src/gem/gemW_winimage$(O): $(AMULET_DIR)/src/gem/gemW_32.h
$(AMULET_DIR)/src/gem/gemW_winimage$(O): $(AMULET_DIR)/src/gem/gemW_winimage.cc
$(AMULET_DIR)/src/gem/gemW_winimage$(O): $(AMULET_DIR)/src/gem/gemW_winimage.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_cursor$(O): $(AMULET_DIR)/src/gem/gemX_cursor.cc
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_draw$(O): $(AMULET_DIR)/src/gem/gemX_draw.cc
$(AMULET_DIR)/src/gem/gemX_file$(O): $(AMULET_DIR)/src/gem/gemX_file.cc
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/src/gem/gemX_image.cc
$(AMULET_DIR)/src/gem/gemX_image$(O): $(AMULET_DIR)/src/gem/gem_GIF.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/src/gem/gemX_input.cc
$(AMULET_DIR)/src/gem/gemX_input$(O): $(AMULET_DIR)/src/gem/gemX_time.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_points$(O): $(AMULET_DIR)/src/gem/gemX_points.cc
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_regions$(O): $(AMULET_DIR)/src/gem/gemX_regions.cc
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_styles$(O): $(AMULET_DIR)/src/gem/gemX_styles.cc
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_text$(O): $(AMULET_DIR)/src/gem/gemX_text.cc
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/src/gem/gemX_time.cc
$(AMULET_DIR)/src/gem/gemX_time$(O): $(AMULET_DIR)/src/gem/gemX_time.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/gemX.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/src/gem/gemX_time.h
$(AMULET_DIR)/src/gem/gemX_windows$(O): $(AMULET_DIR)/src/gem/gemX_windows.cc
$(AMULET_DIR)/src/gem/gem_GIF$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gem_GIF$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gem_GIF$(O): $(AMULET_DIR)/include/amulet/gem_image.h
$(AMULET_DIR)/src/gem/gem_GIF$(O): $(AMULET_DIR)/src/gem/gem_GIF.cc
$(AMULET_DIR)/src/gem/gem_GIF$(O): $(AMULET_DIR)/src/gem/gem_GIF.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gem_keytrans$(O): $(AMULET_DIR)/src/gem/gem_keytrans.cc
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/gem_points$(O): $(AMULET_DIR)/src/gem/gem_points.cc
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testMS1$(O): $(AMULET_DIR)/src/gem/testMS1.cc
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/src/gem/test_utils.cc
$(AMULET_DIR)/src/gem/test_utils$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testbits$(O): $(AMULET_DIR)/src/gem/testbits.cc
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testclip$(O): $(AMULET_DIR)/src/gem/testclip.cc
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testcolor$(O): $(AMULET_DIR)/src/gem/testcolor.cc
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testcopy$(O): $(AMULET_DIR)/src/gem/testcopy.cc
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testdpy$(O): $(AMULET_DIR)/src/gem/testdpy.cc
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testfonts$(O): $(AMULET_DIR)/src/gem/testfonts.cc
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testgem$(O): $(AMULET_DIR)/src/gem/testgem.cc
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testinput$(O): $(AMULET_DIR)/src/gem/testinput.cc
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testlineprops$(O): $(AMULET_DIR)/src/gem/testlineprops.cc
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testloop$(O): $(AMULET_DIR)/src/gem/testloop.cc
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/gem/testpixmap$(O): $(AMULET_DIR)/src/gem/testpixmap.cc
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testpoints$(O): $(AMULET_DIR)/src/gem/testpoints.cc
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testsubwins$(O): $(AMULET_DIR)/src/gem/testsubwins.cc
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testtrans$(O): $(AMULET_DIR)/src/gem/testtrans.cc
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/src/gem/test_utils.h
$(AMULET_DIR)/src/gem/testwinprops$(O): $(AMULET_DIR)/src/gem/testwinprops.cc
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gem/testwinsizes$(O): $(AMULET_DIR)/src/gem/testwinsizes.cc
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/src/gesture/gest_classifier.cc
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/src/gesture/gest_impl.h
$(AMULET_DIR)/src/gesture/gest_classifier$(O): $(AMULET_DIR)/src/gesture/gest_matrix.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/src/gesture/gest_features.cc
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/src/gesture/gest_impl.h
$(AMULET_DIR)/src/gesture/gest_features$(O): $(AMULET_DIR)/src/gesture/gest_matrix.h
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/src/gesture/gest_matrix.cc
$(AMULET_DIR)/src/gesture/gest_matrix$(O): $(AMULET_DIR)/src/gesture/gest_matrix.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/src/gesture/gest_impl.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/src/gesture/gest_matrix.h
$(AMULET_DIR)/src/gesture/gest_trainer$(O): $(AMULET_DIR)/src/gesture/gest_trainer.cc
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gesture/inter_gesture$(O): $(AMULET_DIR)/src/gesture/inter_gesture.cc
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/gesture/testgest$(O): $(AMULET_DIR)/src/gesture/testgest.cc
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/command_basics$(O): $(AMULET_DIR)/src/inter/command_basics.cc
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/priority_list.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/inter_basics$(O): $(AMULET_DIR)/src/inter/inter_basics.cc
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/inter_choice$(O): $(AMULET_DIR)/src/inter/inter_choice.cc
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/inter_move_grow$(O): $(AMULET_DIR)/src/inter/inter_move_grow.cc
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/inter_new_points$(O): $(AMULET_DIR)/src/inter/inter_new_points.cc
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/inter_text$(O): $(AMULET_DIR)/src/inter/inter_text.cc
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/inter/testinplacetext$(O): $(AMULET_DIR)/src/inter/testinplacetext.cc
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/inter/testinter$(O): $(AMULET_DIR)/src/inter/testinter.cc
$(AMULET_DIR)/src/object/dynarray$(O): $(AMULET_DIR)/src/object/dynarray.cc
$(AMULET_DIR)/src/object/dynarray$(O): $(AMULET_DIR)/src/object/dynarray.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/formula_advanced.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/stdvalue.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/src/object/dynarray.h
$(AMULET_DIR)/src/object/formula$(O): $(AMULET_DIR)/src/object/formula.cc
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/src/object/dynarray.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/src/object/impl.h
$(AMULET_DIR)/src/object/object$(O): $(AMULET_DIR)/src/object/object.cc
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/formula_advanced.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/object/testobject$(O): $(AMULET_DIR)/src/object/testobject.cc
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/src/object/dynarray.h
$(AMULET_DIR)/src/object/web$(O): $(AMULET_DIR)/src/object/web.cc
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/example$(O): $(AMULET_DIR)/src/opal/example.cc
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/opal/opal$(O): $(AMULET_DIR)/src/opal/opal.cc
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/src/opal/testJIO.cc
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/src/opal/testJIO_EUC.h
$(AMULET_DIR)/src/opal/testJIO$(O): $(AMULET_DIR)/src/opal/testJIO_JIS.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testcoord$(O): $(AMULET_DIR)/src/opal/testcoord.cc
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testcursor$(O): $(AMULET_DIR)/src/opal/testcursor.cc
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testfade$(O): $(AMULET_DIR)/src/opal/testfade.cc
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testflip$(O): $(AMULET_DIR)/src/opal/testflip.cc
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testgobs$(O): $(AMULET_DIR)/src/opal/testgobs.cc
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testlines$(O): $(AMULET_DIR)/src/opal/testlines.cc
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testmap$(O): $(AMULET_DIR)/src/opal/testmap.cc
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/opal/testoffset$(O): $(AMULET_DIR)/src/opal/testoffset.cc
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testopal$(O): $(AMULET_DIR)/src/opal/testopal.cc
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/testpolyweb$(O): $(AMULET_DIR)/src/opal/testpolyweb.cc
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/src/opal/romkantab.h
$(AMULET_DIR)/src/opal/text_fns$(O): $(AMULET_DIR)/src/opal/text_fns.cc
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/formula_advanced.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/priority_list.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/opal/windows$(O): $(AMULET_DIR)/src/opal/windows.cc
$(AMULET_DIR)/src/utils/misc$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/misc$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/misc$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/misc$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/utils/misc$(O): $(AMULET_DIR)/src/utils/misc.cc
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/amulet/priority_list.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/priority_list$(O): $(AMULET_DIR)/src/utils/priority_list.cc
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/registry$(O): $(AMULET_DIR)/src/utils/registry.cc
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/standard_slots$(O): $(AMULET_DIR)/src/utils/standard_slots.cc
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/include/amulet/stdvalue.h
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/stdvalue$(O): $(AMULET_DIR)/src/utils/stdvalue.cc
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/symbol_table$(O): $(AMULET_DIR)/src/utils/symbol_table.cc
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/utils/testloadsave$(O): $(AMULET_DIR)/src/utils/testloadsave.cc
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/amulet/symbol_table.h
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/testmaps$(O): $(AMULET_DIR)/src/utils/testmaps.cc
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/stdvalue.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/types$(O): $(AMULET_DIR)/src/utils/types.cc
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/amulet/univ_lst.h
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/include/amulet/univ_lst.tpl
$(AMULET_DIR)/src/utils/univ_lst$(O): $(AMULET_DIR)/src/utils/univ_lst.cc
$(AMULET_DIR)/src/utils/univ_map$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/univ_map$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/univ_map$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/utils/univ_map$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/utils/univ_map$(O): $(AMULET_DIR)/src/utils/univ_map.cc
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/amulet/stdvalue.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/utils/value_list$(O): $(AMULET_DIR)/src/utils/value_list.cc
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/button_widgets$(O): $(AMULET_DIR)/src/widgets/button_widgets.cc
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/dialog_widgets$(O): $(AMULET_DIR)/src/widgets/dialog_widgets.cc
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/editing_commands$(O): $(AMULET_DIR)/src/widgets/editing_commands.cc
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/rich_text$(O): $(AMULET_DIR)/src/widgets/rich_text.cc
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/scroll_widgets$(O): $(AMULET_DIR)/src/widgets/scroll_widgets.cc
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/selection_widgets$(O): $(AMULET_DIR)/src/widgets/selection_widgets.cc
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/testdragdrop$(O): $(AMULET_DIR)/src/widgets/testdragdrop.cc
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/testrichtext$(O): $(AMULET_DIR)/src/widgets/testrichtext.cc
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/testselectionwidget$(O): $(AMULET_DIR)/src/widgets/testselectionwidget.cc
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/testwidgets$(O): $(AMULET_DIR)/src/widgets/testwidgets.cc
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/registry.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/text_fns.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/text_widgets$(O): $(AMULET_DIR)/src/widgets/text_widgets.cc
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/undo_dialog_box$(O): $(AMULET_DIR)/src/widgets/undo_dialog_box.cc
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/web.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/include/amulet/widgets_advanced.h
$(AMULET_DIR)/src/widgets/widgets$(O): $(AMULET_DIR)/src/widgets/widgets.cc
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/av$(O): $(AMULET_DIR)/samples/av.cc
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/checkers$(O): $(AMULET_DIR)/samples/checkers.cc
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/color$(O): $(AMULET_DIR)/samples/color.cc
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/hsv$(O): $(AMULET_DIR)/samples/hsv.cc
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/space$(O): $(AMULET_DIR)/samples/space.cc
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/space2$(O): $(AMULET_DIR)/samples/space2.cc
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/tree$(O): $(AMULET_DIR)/samples/tree.cc
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/debugger.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/agate/agate$(O): $(AMULET_DIR)/samples/agate/agate.cc
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/circuit/circuit$(O): $(AMULET_DIR)/samples/circuit/circuit.cc
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/example1$(O): $(AMULET_DIR)/samples/examples/example1.cc
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/samples/examples/example2.cc
$(AMULET_DIR)/samples/examples/example2$(O): $(AMULET_DIR)/samples/examples/example2db.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/example2db$(O): $(AMULET_DIR)/samples/examples/example2db.cc
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/goodbye_button$(O): $(AMULET_DIR)/samples/examples/goodbye_button.cc
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/goodbye_inter$(O): $(AMULET_DIR)/samples/examples/goodbye_inter.cc
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/examples/hello$(O): $(AMULET_DIR)/samples/examples/hello.cc
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/aboutgilt$(O): $(AMULET_DIR)/samples/gilt/aboutgilt.cc
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/samples/gilt/borderrect.cc
$(AMULET_DIR)/samples/gilt/borderrect$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/samples/gilt/cpp_out.cc
$(AMULET_DIR)/samples/gilt/cpp_out$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/samples/gilt/externs.cc
$(AMULET_DIR)/samples/gilt/externs$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/inter_advanced.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/samples/gilt/gilt.cc
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/samples/gilt/panel.h
$(AMULET_DIR)/samples/gilt/gilt$(O): $(AMULET_DIR)/samples/gilt/savecpp.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/namelabel$(O): $(AMULET_DIR)/samples/gilt/namelabel.cc
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/nameonly$(O): $(AMULET_DIR)/samples/gilt/nameonly.cc
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/numberinput$(O): $(AMULET_DIR)/samples/gilt/numberinput.cc
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/samples/gilt/panel.cc
$(AMULET_DIR)/samples/gilt/panel$(O): $(AMULET_DIR)/samples/gilt/panel.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/savecpp$(O): $(AMULET_DIR)/samples/gilt/savecpp.cc
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/samples/gilt/externs.h
$(AMULET_DIR)/samples/gilt/scrollgroup$(O): $(AMULET_DIR)/samples/gilt/scrollgroup.cc
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/samples/map/map.cc
$(AMULET_DIR)/samples/map/map$(O): $(AMULET_DIR)/samples/map/map.hh
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/object_advanced.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/opal_advanced.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/include/amulet/widgets.h
$(AMULET_DIR)/samples/timing/timing$(O): $(AMULET_DIR)/samples/timing/timing.cc
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/am_inc.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/am_io.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/am_strstream.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/amulet2_conversion.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/anim.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/formula.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/gdefs.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/gem.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/gesture.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/idefs.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/inter.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/misc.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/object.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/opal.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/rich_text.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/standard_slots.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/types.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/undo_dialog.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/univ_map.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/univ_map.tpl
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/value_list.h
$(AMULET_DIR)/samples/tutorial/tutorial$(O): $(AMULET_DIR)/include/amulet/widgets.h
