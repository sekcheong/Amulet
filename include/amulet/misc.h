// misc.h
// Misc. machine dependant utility routines.  

void Am_Break_Into_Debugger ();
void Am_Wait(int milliseconds);

char *Am_Get_Amulet_Pathname ();
char *Am_Merge_Pathname(char *name);

#ifdef _MACINTOSH
void Am_Init_Pathname();
#endif
