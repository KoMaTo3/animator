#ifndef __memory_h__
#define __memory_h__

//### includes ###//

#include "windows.h"
//#include "types.h"
//#include "tools.h"


class memory
{
private:
  unsigned int length;
  char *data;
  void init();

public:

  memory();
  memory(unsigned int len);
  memory( const memory &src );
  virtual ~memory();

  HRESULT alloc     (unsigned int len);
  HRESULT realloc   (unsigned int len);
  void   free      ();
  unsigned int  getLength () const;
  char*  getData   ();

  void operator=   (const char *str);
  void operator=   (memory &src);
  char& operator[] (int num);
  void operator+=  (memory &src);
  operator char*   ();
};


#endif