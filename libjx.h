#define INFO(x...) fprintf(stderr, x)
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#if !defined(GTRACE)
#define GTRACE(c, rv)
#endif

#define G(x) do { int Grv=(x); if (Grv) { GTRACE(#x, Grv) ; return Grv; }} while (0)
#define NOOP
   #define EMSG(x...) INFO(x)
   #define EMSGRET(n, x...) {EMSG(x); return n; }
   #define RVEMSGRET(rv, n, x...) { if (-1 == rv) EMSGRET(n, x); }
   #define MSG(x...) INFO(x)
   #define DBG(x...) XDBG(__FILE__, __LINE__, x)
   #define DEBUG(x...) do { if (option(debug)) DBG(x); } while (0)
   #define DDEBUG(x...) do { if (1<option(debug)) DBG(x); } while (0)
   #define DDDEBUG(x...) do { if (2<option(debug)) DBG(x); } while (0)
   #define DDDDEBUG(x...) do { if (3<option(debug)) DBG(x); } while (0)
   #define VERBOSE(x...) (option(verbose) ? INFO(x) : 0)
   #define VVERBOSE(x...) (1<option(verbose) ? INFO(x) : 0)

   #define FIXME(x, ...) INFO(__FILE__ ".%d:  FIXME:  " x "\n", __LINE__, ## __VA_ARGS__)
struct buffer_control
{
   union                      /* Allow user to see either signedness */
   {
      char * buffer;
      unsigned char * ubuffer;
   };
   size_t size;
   size_t hiwater;
};

extern int appendbuffer(struct buffer_control * pbf, const void * data, ssize_t length);
#define loadbuffer(bf, d, l) ((bf)->hiwater = 0, appendbuffer((bf), (d), (l)))
#define appendpadbuffer(bf, ch, len) padbuffer(bg, ch, (bf)->hiwater + len)
/* This  is perhaps a misnomer.  It ensures that the buffer contains */
/* at  least lenght bytes.  See appendpadbuffer to append n bytes to */
/* the contents.                                                     */
extern int padbuffer(struct buffer_control * pbf, const char pad, const ssize_t length);
extern int getbuffer(struct buffer_control * pbf, const size_t length);
extern int jcalloc(const size_t count, const size_t size, const char * what, void ** where);
extern int jmalloc(const size_t size, const char * what, void ** where);
extern int jrealloc(void * old, const size_t size, const char * what, void ** where);
enum dumpflags
{
   df_relative = 0,                   /* Relative address            */
   df_absolute = 1,                   /* Show actual address         */
   df_showboth = 2,                   /* EBCDIC and ASCII            */
   df_showalt = 4,                    /* EBCDIC on ASCII and reverse */
};
extern char * xlate(char * s, const size_t len, const unsigned char tab[256]);
struct jfile
{
   /* If fd is no -1, file is null and vv                            */
   int fd;                            /* -1 if closed                */
   char * name;
   FILE * file;                       /* NULL if closed              */
};
typedef struct jfile JFILE;
#define JFILEINIT {.fd = -1 }

extern JFILE * jtempfile(FILE * f);
extern JFILE * jtempfd(int fd);
extern JFILE * jstdfile(int ix);
extern int jclose(JFILE * pf);
extern int jopen(JFILE * jf, const char * fn, const int flags, const mode_t mode);
extern int jfopen(JFILE * jf, const char * fn, const char * how);
extern int jread(JFILE * jf, void * area, size_t len, size_t * pgot);
extern int jwrite(JFILE * jf, void * area, size_t len);
extern int jreadbuffer(struct buffer_control * bf, const char * filename);

/*********************************************************************/
/* In jdump.c                                                        */
/*********************************************************************/

extern const unsigned char tab819to1047[256];
extern const unsigned char tab1047to819[256];
extern void jdumpcorefilemsg(JFILE * jf, const char * msg, const void * vp, size_t ln, enum dumpflags df);
#define jdumpcorefile(dmpf, vp, len, df) jdumpcorefilemsg(dmpf, NULL, vp, len, df)
#define jdumpcoremsg(msg, vp, len, df) jdumpcorefilemsg(NULL, msg, vp, len, df)
#define jdumpcore(vp, len, df) jdumpcorefilemsg(NULL, NULL, vp, len, df)
