/* Manage a JFILE structure.                                         */
/*                                 John Hartmann 7 Jun 2016 13:03:35 */

/*********************************************************************/
/* Change activity:                                                  */
/* 7 Jun 2016  New module.                                           */
/*********************************************************************/

#define JDEBUG

#include "libjx.h"
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

static JFILE stdfiles[3] =
{
   {.fd = -1, .name = "<standard input>", },
   {.fd = -1, .name = "<standard output>", },
   {.fd = -1, .name = "<standard error>", },
};

static JFILE tempfile = {.fd = -1, .name = "<unknown name>",};

/* Forward declarations:                                             */
static int veropen(JFILE * jf, const char * fn);
static int getfd(const JFILE * jf);
/* End of forward declarations.                                      */

/*********************************************************************/
/* JFILE handling                                                    */
/*********************************************************************/

JFILE *
jtempfile(FILE * f)
{
   JFILE * jf = &tempfile;

   jf->fd = -1;
   jf->file = f;
   return jf;
}

JFILE *
jtempfd(int fd)
{
   JFILE * jf = &tempfile;

   jf->fd = fd;
   jf->file = NULL;
   return jf;
}

JFILE *
jstdfile(int ix)
{
   JFILE * jf;

   assert(0 <= ix && 3 > ix);
   jf = stdfiles + ix;
   if (jf->file) return jf;
   switch (ix)
   {
      case 0:
         jf->file = stdin;
         break;
      case 1:
         jf->file = stdout;
         break;
      case 2:
         jf->file = stderr;
         break;
   }
   return jf;
}

int
jclose(JFILE * jf)
{
   static JFILE init = JFILEINIT;

   assert(-1 == jf->fd || !jf->file);

   if (0 <= jf->fd) close(jf->fd);
   else if (jf->file) fclose(jf->file);
   if (jf->name) free(jf->name);

   *jf = init;
   return 0;
}

static int
veropen(JFILE * jf, const char * fn)
{
   if (-1 == jf->fd && ! jf->file)
      EMSGRET(16, "Cannot open file %s:  %s\n", fn, strerror(errno));
   jf->name = strdup(fn);
   return 0;
}

int
jopen(JFILE * jf, const char * fn, const int flags, const mode_t mode)
{
   jclose(jf);
   jf->fd = open(fn, flags, mode);
   return veropen(jf, fn);
}

int
jfopen(JFILE * jf, const char * fn, const char * how)
{
   jclose(jf);
   jf->file = fopen(fn, how);
   return veropen(jf, fn);
}

static int
getfd(const JFILE * jf)
{
   int fd = jf->file ? fileno(jf->file) : jf->fd;

   assert(0 <= fd);
   return fd;
}

int
jread(JFILE * jf, void * area, size_t len, size_t * pgot)
{
   char * s = area;
   int fd = getfd(jf);

   if (pgot) *pgot = 0;
   while (0 < len)
   {
      ssize_t rv = read(fd, s, len);

      RVEMSGRET(rv, 16, "Cannot read %zd bytes from %s:  %s\n", len, jf->name, strerror(errno));
      if (!rv) return 0;              /* EOF                         */

      if (pgot) *pgot += rv;
      len -= rv;
      s += rv;
   }
   return 0;
}

int
jwrite(JFILE * jf, void * area, size_t len)
{
   char * s = area;
   int fd = getfd(jf);
   if (jf->file) fflush(jf->file);    /* synchronise                 */

   while (0 < len)
   {
      ssize_t rv = write(fd, s, len);

      RVEMSGRET(rv, 16, "Cannot write %zd bytes from %s:  %s\n", len, jf->name, strerror(errno));

      len -= rv;
      s += rv;
   }
   return 0;
}

/*********************************************************************/
/* Read file into buffer.                                            */
/*********************************************************************/

int
jreadbuffer(struct buffer_control * bf, const char * filename)
{
   struct stat st;
   JFILE jf = JFILEINIT;
   int rv = stat(filename, &st);

   RVEMSGRET(rv, 16, "File %s not found:  %s\n", filename, strerror(errno));

   if (!(S_IFREG & st.st_mode))
      EMSGRET(16, "%s is not a regular file.\n", filename);

   G(getbuffer(bf, st.st_size + 2));
   G(jopen(&jf, filename, O_RDONLY, 0));

   rv = jread(&jf, bf->buffer, st.st_size, &bf->hiwater);
   jclose(&jf);
   return 0;

}
