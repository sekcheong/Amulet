$! MAKE_VMS_SHARED.COM
$! 29-MAY-1997, David Mathog, Biology Division, Caltech
$!
$! This builds Amulet 3.0B on OpenVMS 6.2 with DEC CXX 5.4 and
$! produces a SHAREABLE image.  Much better than the other way,
$! which makes .exe files that are huge.  Interestingly, the
$! shareable seems to work even though the compilers are NOT
$! set to /extern=common - presumably there are no PSECTS???
$!
$! need the first definition to find hideous nonportable includes, second
$! and third, so that amulet will run
$!
$! default is to GZIP all .exe, as they are huge.  Put something into P1
$! to suppress this.
$!
$ type sys$Input

You must have the SYMBOLS program installed for this to work.
You must be running on an ALPHA for this to work.

$!
$ define/nolog amulet prgdisk:[shared.programs.amulet.amulet_3b.include.amulet]
$ define/nolog amulet_lib prgdisk:[shared.programs.amulet.amulet_3b.lib]
$ define/nolog  AMULET_DIR "/prgdisk/shared/programs/amulet/amulet_3b"
$!
$ write sys$output "Before you run amulet always be sure this is defined:"
$ sho log amulet_dir
$!
$ mycxx :== cxx/standard=ms/nolis -
 /include=[-.include] -
 /define=(NEED_BOOL,NEED_TIME,DEBUG,NEED_SOCKET,AMULET2_CONVERSION,MEMORY) -
 /warnings=(disable=extrasemi)
$ mycc :== cc/standard=vaxc/nolis
$ mylink :== cxxlink/nomap
$!
$!
$ start = f$time()
$ mycxx [-.src.opal]opal.cc 
$ mycxx [-.src.opal]windows.cc 
$ mycxx [-.src.opal]text_fns.cc 
$ mycxx [-.src.inter]inter_basics.cc 
$ mycxx [-.src.inter]command_basics.cc 
$ mycxx [-.src.inter]inter_move_grow.cc 
$ mycxx [-.src.inter]inter_choice.cc 
$ mycxx [-.src.inter]inter_new_points.cc 
$ mycxx [-.src.inter]inter_text.cc 
$ mycxx [-.src.debug]inspector.cc 
$ mycxx [-.src.debug]tracer.cc 
$ mycxx [-.src.widgets]widgets.cc 
$ mycxx [-.src.widgets]button_widgets.cc 
$ mycxx [-.src.widgets]scroll_widgets.cc 
$ mycxx [-.src.widgets]text_widgets.cc 
$ mycxx [-.src.widgets]selection_widgets.cc 
$ mycxx [-.src.widgets]editing_commands.cc 
$ mycxx [-.src.widgets]dialog_widgets.cc 
$ mycxx [-.src.widgets]rich_text.cc 
$ mycxx [-.src.widgets]undo_dialog_box.cc 
$ mycxx [-.src.gesture]inter_gesture.cc 
$ mycxx [-.src.gesture]gest_trainer.cc 
$ mycxx [-.src.gesture]gest_classifier.cc 
$ mycxx [-.src.gesture]gest_features.cc 
$ mycxx [-.src.gesture]gest_matrix.cc 
$ mycxx [-.src.anim]anim_animators.cc 
$ mycxx [-.src.anim]anim_constraint.cc 
$ mycxx [-.src.anim]anim_interactor.cc 
$ library/create/object [-.lib]libamulet.olb *.obj
$ delete *.obj.
$! 
$ mycxx [-.src.gem]gem_points.cc 
$ mycxx [-.src.gem]gem_GIF.cc 
$ mycxx [-.src.gem]gemX_points.cc 
$ mycxx [-.src.gem]gemX_windows.cc 
$ mycxx [-.src.gem]gemX_styles.cc 
$ mycxx [-.src.gem]gemX_draw.cc 
$ mycxx [-.src.gem]gemX_text.cc 
$ mycxx [-.src.gem]gemX_image.cc 
$ mycxx [-.src.gem]gemX_cursor.cc 
$ mycxx [-.src.gem]gemX_time.cc 
$ mycxx [-.src.gem]gemX_input.cc 
$ mycxx [-.src.gem]gem_keytrans.cc 
$ mycxx [-.src.gem]gemX_regions.cc 
$! get time of day implementation, needed for VMS <7.0 only
$!
$ vmsver = f$getsyi("VERSION")
$ vmsver = f$element(0,".",vmsver)
$ vmsver = vmsver - "V"
$ if(vmsver .lt. 7)then mycc  [.OPENVMS]gettimeofday.c
$!
$!
$ library/object [-.lib]libamulet.olb *.obj
$ delete *.obj.
$!
$ mycxx [-.src.object]dynarray.cc 
$ mycxx [-.src.object]object.cc 
$ mycxx [-.src.object]formula.cc 
$ mycxx [-.src.object]web.cc 
$ library/object [-.lib]libamulet.olb *.obj
$ delete *.obj.
$!
$ mycxx [-.src.utils]priority_list.cc 
$ mycxx [-.src.utils]standard_slots.cc 
$ mycxx [-.src.utils]value_list.cc 
$ mycxx [-.src.utils]registry.cc 
$ mycxx [-.src.utils]misc.cc 
$ mycxx [-.src.utils]symbol_table.cc 
$ mycxx [-.src.utils]univ_lst.cc 
$ mycxx [-.src.utils]univ_map.cc 
$ mycxx [-.src.utils]types.cc 
$ mycxx [-.src.utils]stdvalue.cc 
$ library/object [-.lib]libamulet.olb *.obj
$ delete *.obj.
$!
$ mycxx [-.src.gem]test_utils.cc 
$ mycxx [-.src.gem]testbits.cc 
$ mycxx [-.src.gem]testcolor.cc 
$ mycxx [-.src.gem]testfonts.cc 
$ mycxx [-.src.gem]testwinprops.cc 
$ library/create/object [-.lib]testlib.olb *.obj
$ delete *.obj.
$!
$ write sys$output "Now making shareable images"
$ set def [-.lib]
$ @[-.bin]MAKE_OPT_FROM_OLB libamulet.olb "motif, xt, x11"
$ create chunk.txt
PRGDISK:[SHARED.PROGRAMS.AMULET.AMULET_3B.LIB]libamulet_shr.exe/share
$ @[-.bin]MAKE_OPT_FROM_OLB testlib.olb "motif, xt, x11" chunk.txt
$ link/share=libamulet_shr.exe libamulet.opt/opt
$ link/share=testlib_shr.exe testlib.opt/opt
$ define wherelib 'f$environment("DEFAULT")
$!
$ set def [-.bin]
$ type sys$input

In order to make the shareables work properly you must either define
the logicals LIBAMULET_SHR and TESTLIB_SHR to point to the
libraries, or you must place the libraries in SYS$SHARE.

This build procedure does NOT do either action for you!!!

$!
$!
$ buildtest: subroutine
$ start = f$time()
$ list = f$edit(P1,"UPCASE,COLLAPSE")
$ write sys$output "Building ''LIST'"
$ mycxx 'list'
$ list = list - ".CC"
$ llist = f$element(1,"]",list)
$ if(llist .eqs. "]")then llist = list
$ mylink 'llist', sys$Input/opt
wherelib:testlib_shr.exe/share
wherelib:libamulet_shr.exe/share
SYS$SHARE:DECW$XLIBSHR.EXE/SHARE
$ delete/since="''start'" *.obj. 
$ exit
$ endsubroutine
$!
$ buildsample: subroutine
$ start = f$time()
$ list = f$edit(P1,"UPCASE,COLLAPSE")
$ write sys$output "Building ''LIST'"
$ mycxx 'list'
$ list = list - ".CC"
$ llist = f$element(1,"]",list)
$ if(llist .eqs. "]")then llist = list
$ mylink 'llist', sys$Input/opt
wherelib:libamulet_shr.exe/share
SYS$SHARE:DECW$XLIBSHR.EXE/SHARE
$ delete/since="''start'" *.obj. 
$ exit
$ endsubroutine
$!
$! when passing a list ONLY the first may have a ".CC" !
$!
$ type sys$Input

Expect and ignore these duplicate symbol warnings:

  Testtrans    BLACK,GREEN,FONT1,FONT2,FONT3
  Testloop     BLACK,GREEN,FONT1,FONT2,FONT3
  TESTCLIP     RED, GREEN, BLUE, WHITE, BLACK,YELLOW,PURPLE
  TESTPOINTS   BLACK,WHITE,RED
  TESTWINSIZES RED,GREEN,BLUE,WHITE,BLACK
  TESTCOPY     BLACK,GREEN,WHITE
  TESTGOBS     DASH15,DASH8,DASH20

$!
$ call buildtest [-.src.gem]testdpy.cc
$ call buildtest [-.src.gem]testgem.cc,testlineprops
$ call buildtest [-.src.gem]testtrans.cc
$ call buildtest [-.src.gem]testsubwins.cc
$ call buildtest [-.src.gem]testloop.cc
$ call buildtest [-.src.gem]testclip.cc 
$ call buildtest [-.src.gem]testpoints.cc 
$ call buildtest [-.src.gem]testwinsizes.cc 
$ call buildtest [-.src.gem]testinput.cc 
$ call buildtest [-.src.gem]testcopy.cc 
$ call buildtest [-.src.object]testobject.cc 
$ call buildtest [-.src.opal]testopal.cc 
$ call buildtest [-.src.opal]testpolyweb.cc 
$ call buildtest [-.src.opal]testlines.cc 
$ call buildtest [-.src.opal]testgobs.cc 
$ call buildtest [-.src.opal]testmap.cc 
$ call buildtest [-.src.inter]testinter.cc 
$ call buildtest [-.src.anim]testaniminter.cc 
$ call buildtest [-.src.anim]testanimators.cc 
$ call buildtest [-.src.widgets]testwidgets.cc 
$ call buildtest [-.src.widgets]testselectionwidget.cc 
$ call buildtest [-.src.widgets]testdragdrop.cc 
$ call buildtest [-.src.utils]testmaps.cc 
$ call buildtest [-.src.gesture]testgest.cc 
$ call buildtest [-.src.utils]testloadsave.cc 
$ call buildtest [-.src.opal]testfade.cc 
$ call buildtest [-.src.widgets]testrichtext.cc 
$ call buildtest [-.src.gem]testpixmap.cc 
$ call buildtest [-.src.opal]testJIO.cc 
$!
$ call buildsample [-.samples.agate]agate.cc 
$ call buildsample [-.samples]av.cc 
$ call buildsample [-.samples]checkers.cc 
$ call buildsample [-.samples.circuit]circuit.cc 
$ call buildsample [-.samples]color.cc 
$ call buildsample [-.samples]hsv.cc 
$ call buildsample [-.samples.examples]hello.cc 
$ call buildsample [-.samples.examples]goodbye_inter.cc 
$ call buildsample [-.samples.examples]goodbye_button.cc 
$ call buildsample [-.samples]space.cc 
$ call buildsample [-.samples]space2.cc 
$ call buildsample [-.samples.timing]timing.cc
$ call buildsample [-.samples.tutorial]tutorial.cc 
$ call buildsample [-.samples]tree.cc 
$ call buildsample [-.samples.gilt]gilt.cc,namelabel,nameonly,numberinput,savecpp,borderrect,panel,scrollgroup,externs,aboutgilt,cpp_out
$ call buildsample [-.samples.examples]example1.cc 
$ call buildsample [-.samples.examples]example2.cc,example2db
