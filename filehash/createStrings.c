/**
 * Create Test Strings in random size
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define MIN_LEN 5
#define FIRST_CHAR (char)'0'
#define LAST_CHAR  (char)'~'

int
main (int argc, char** argv)
{
  struct timeval tv;
  int num, maxksize, maxdsize, i, tmp, len, k;
  char *kstr, *dstr;

  if (argc < 4) {
    fprintf (stderr, "Usage: %s numstring largestkey largestdata\n", argv[0]);
    return 1;
  }

  num = atoi  (argv[1]);
  maxksize = atoi (argv[2]);
  maxdsize = atoi (argv[3]);
  
  gettimeofday (&tv, 0);
  srand (tv.tv_sec);

  kstr = (char *)malloc(maxksize + 1 + 32);
  dstr = (char *)malloc(maxdsize + 1 + 32);

  for (i = 0; i < num; i++) {
    /* create key */
    memset (kstr, 0, maxksize + 1 + 32);
    len = rand() % maxksize;
    if (len < MIN_LEN)
      len = MIN_LEN;
    

    for (k = 0; k < len; k++) {
      tmp = rand() % LAST_CHAR;
      if (tmp < FIRST_CHAR)
	tmp += FIRST_CHAR;
      kstr[k] = (char)tmp;
    }
    printf ("%s-%d\n",kstr, i);


    /* create data */
    memset (dstr, 0, maxdsize + 1 + 32);
    len = rand() % maxdsize;
    if (len < MIN_LEN)
      len = MIN_LEN;
    

    for (k = 0; k < len; k++) {
      tmp = rand() % LAST_CHAR;
      if (tmp < FIRST_CHAR)
	tmp += FIRST_CHAR;
      dstr[k] = (char)tmp;
    }
    printf ("%s-%d\n",dstr, i);
  }
  
  free (kstr);
  free (dstr);
    
  return 0;
}
