#include <stdiostr.h>
#include <iostream.h>

class SetupConsole
{
public:
  SetupConsole();
  ~SetupConsole();
  void setFile(const char* fileName);
private:
  stdiostream *myStreamIn;
  stdiostream *myStreamOut;
  stdiostream *myStreamErr;
};
