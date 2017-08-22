
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "bsp.h"

#undef errno
//extern int errno;
extern int  _end;

#if 1
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&_end;
  }
  prev_heap = heap;

  heap += incr;

  return (caddr_t) prev_heap;
}
#endif

#if 0
int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _read(int file, char *ptr, int len)
{
    *ptr = BSP_fgetc();

    return 1;
}

int _write(int file, char *ptr, int len)
{
    int t_TX_counter;

    (void) file;

    for( t_TX_counter = 0; t_TX_counter < len; t_TX_counter++ )
    {
        BSP_fputc( *ptr++ );
    }

    return len;
}
#endif
