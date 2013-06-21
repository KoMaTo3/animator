/*
--------------------
  �������� ������
--------------------
*/

#include "memory.h"
//#include "ktypes.h"
//#include "aFile.h"
//#include "defines.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

memory::memory()
:data(0), length(0) //init()
{
}

memory::memory(unsigned int len)
{
  this->init();
  this->alloc(len);
}//constructor

memory::memory( const memory &src )
{
  this->init();
  this->operator =( (memory&) src);
}//constructor


memory::~memory()
{
  this->free();
}//destructor


/*
----------
  init
  ��������� ������������� ������
----------
*/
void memory::init()
{
  this->data = 0;
  this->length = 0;
}//init


/*
----------
  operator char*
  ���������� � ���� char*
----------
*/
memory::operator char*()
{
  return this->data;
}//operator char*


/*
----------
  alloc
  ��������� ������. ���� ��� �������� - ������������� � ����� ���������
----------
*/
HRESULT memory::alloc(unsigned int len)
{
  this->free();

  if(!len)
  {
    //__log.printInfo(aFile_level_WARNING, "memory::alloc(0)");
    this->length = 0;
    return -1;
  }

  this->length = len;
  this->data = new char[this->length];
  if(!this->data)
  {
    //__log.printInfo(aFile_level_CRITICALERROR, "can't allocate memory, %d byte[s]\n", len);
    return -1;
  }

  return 0;
}//alloc


/*
----------
  realloc
  ������������� ������
----------
*/
HRESULT memory::realloc(unsigned int len)
{
  if(this->data)//delete old mem
  {
    this->free();
  }
  return this->alloc(len);
}//realloc


/*
----------
  Free
  ������������ ������
----------
*/
void memory::free()
{
  if(this->data)
  {
    delete[] this->data;
    this->data = 0;
  }
  this->length = 0;
  this->data = 0;
}//Free


/*
----------
  getLength
  ���������� ������ ���������� ������
----------
*/
unsigned int memory::getLength() const
{
  return this->length;
}//getLength


/*
----------
  operator = memory
----------
*/
void memory::operator=(memory &src)
{
  if(&src == this)
    return;
  if(!src.length)
  {
    this->free();
    return;
  }

  if(this->length != src.length)
    this->alloc(src.length);

  memcpy(this->data, (char*) src, this->length);
}//operator = memory


/*
----------
  operator = char*
----------
*/
void memory::operator=(const char *str)
{
  if(!str)
  {
    this->free();
    return;
  }

  unsigned int len = (unsigned int) strlen(str) + 1;
  if(this->length != len)
  {
    this->free();
    this->alloc(len);
  }
  memcpy(this->data, str, len - 1);
  data[len - 1] = 0;
}//operator = char*


/*
----------
  getData
  ���������� ��������� �� ������ ����� ������
----------
*/
char* memory::getData()
{
  return this->data;
}//getData


/*
----------
  operator []
----------
*/
char& memory::operator[](int num)
{
  static char tmp = 0;
  if(num < 0 || num >= (int) this->length)
  {
    //__log.printInfo(aFile_level_ERROR, "memory::operator[](%d) this->length = %d", num, this->length);
    return tmp;
  }
  if(this->data == 0)
  {
    //__log.printInfo(aFile_level_ERROR, "memory::operator[]() this->data = 0", num);
    return tmp;
  }
  return this->data[num];
}//operator []


/*
----------
  operator += memory
----------
*/
void memory::operator+=(memory &src)
{
  if(this->length < 1)
    return this->operator=(src);

  memory tmp(this->length);
  memcpy((char*)tmp, this->getData(), this->length);

  this->alloc(tmp.length + src.getLength());
  memcpy(this->getData(), tmp.getData(), tmp.getLength());
  memcpy(this->getData() + tmp.getLength(), src.getData(), src.getLength());
}//operator += memory

