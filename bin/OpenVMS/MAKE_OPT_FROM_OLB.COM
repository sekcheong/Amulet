$!make_opt_from_olb.com
$! 29-MAY-1997, David Mathog, Biology Division, Caltech
$!
$! program which analyzes all .OLB files specified, produces from it
$! a .OPT file, and then uses that to make a shareable image of that
$! object library.  The shareable images are produced in the directory
$! from which this procedure is run.  This requires the SYMBOLS program,
$! available from Ferry Bolhár-Nordenkampf, ftp://ftp.wku.edu/
$!
$ if (P1 .eqs. "")
$ then
$   type sys$Input

This procedure converts a list of .OLB object libraries into a set
of option files for linking shareable images.

  Syntax:
      @make_opt_from_olb  P1 P2 P3
  
  P1 = [dir]lib1.olb [,[dir2]lib2.olb, etc.]

  P2 = [X11 ,Xt, Xmu, Motif, GL, GLu]
    optional shares to include (not case sensitive), these are:
      GLu      sys$share:decw$openglushr.exe/share
      Gl       sys$share:decw$openglshr.exe/share
      Motif    sys$share:decw$xmlibshr/share
      Xmu      sys$share:decw$xmulibshr/share
      Xt       sys$share:decw$xtshr/share
      X11      sys$share:decw$xlibshr/share

  P3 = name of a file to append to the .opt

The option files take their names from the object library names, and 
are created in the directory from which this program is run.

Note that this procedure cannot, and does not, produce the actual
shareable, if the library refers to other libraries, this routine could
not define the appropriate flags in the options file - that must be
done manually.  Also, you might want to remove some of the modules from the
object library, for instance, if the library has GL pieces, but you don't
have that on your system (obviously then programs requiring GL wouldn't
work...).  Here is the final command to make the shareable: 

  $ link/share=[]whatever.exe whatever.opt/opt

$ exit
$ endif
$ list  = P1
$ list2 = P2
$ extra = P3
$!
$ i = 0
$ top:
$    target = f$element(i,",",list)
$    if(target .nes. ",")
$    then
$       target = f$edit(target,"TRIM,COLLAPSE")
$       write sys$output "Now processing object library ''target'"
$       filename = f$parse(target,,,"NAME")
$       open/write ofil: []'filename'.opt.
$       write ofil: "GSMATCH=lequal,1,1000"
$       symbols/out=[]scratch1.tmp 'target'/include
$       open/read  ifil: []scratch1.tmp
$       loop:
$          read/error=done/end=done ifil: string
$          string = f$edit(string,"COMPRESS,TRIM")
$          if(string .eqs. "")then goto loop
$          flags = F$EDIT(F$ELEMENT(2," ",string),"UPCASE")
$          if(flags .eqs. "LRGS")
$          then
$             symout = f$elemeent(1," ",string)
$             write ofil: "SYMBOL_VECTOR = (",symout,"=DATA)"
$             goto loop
$          endif
$          if(flags .eqs. "PRGS")
$          then
$             symout = f$elemeent(1," ",string)
$             write ofil: "SYMBOL_VECTOR = (",symout,"=PROCEDURE)"
$             goto loop
$          endif
$          if(F$EDIT(F$ELEMENT(0," ",string),"UPCASE") .eqs. "MODULE")
$          then
$             write sys$output string
$          endif
$          goto loop
$       done:
$       close ifil:
$       write ofil: target,"/lib"
$!
$! now add any shareable libraries the user specified,
$! do them in a "top down" order
$!
$  list2 = f$edit(list2,"COLLAPSE,UPCASE")
$  list2 = list2 + ","
$  full = f$length(list2)
$  if(f$locate("GLU,",list2) .nes. full)then   write ofil: "sys$share:decw$openglushr.exe/share
$  if(f$locate("GL,",list2) .nes. full)then    write ofil: "sys$share:decw$openglshr.exe/share
$  if(f$locate("MOTIF,",list2) .nes. full)then write ofil: "sys$share:decw$xmlibshr12/share"
$  if(f$locate("XMU,",list2) .nes. full)then   write ofil: "sys$share:decw$xmulibshrr5/share
$  if(f$locate("XT,",list2) .nes. full)then    write ofil: "sys$share:decw$xtlibshrr5/share
$  if(f$locate("X11,",list2) .nes. full)then   write ofil: "sys$share:decw$xlibshr/share
$!
$!
$       close ofil:
$!
$!  the following is a very odd way of doing an append that works no matter 
$!  what file types the two in question are, so long as they are both text
$!
$       if(extra .nes. "")
$           then
$             open/append ofil 'filename'.opt
$             search/out=ofil/match=nor 'extra' "this string is not in this file!!!!"
$             close ofil
$           endif
$!
$       delete scratch1.tmp.
$       i = i + 1
$       goto top
$    endif
$! done
