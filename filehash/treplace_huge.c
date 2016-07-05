#ifdef _FFDB_HUGE_DATA
/**
 * Simple Test to replace huge data
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/file.h>
#include <ffdb_db.h>

#define MAX_LEN 32768

#define FIRST_CHAR (char)'0' 
#define LAST_CHAR  (char)'~' 

#define USER_STRING "Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCDHello threre I am the new user string for QCD  Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD"

static void
insert_user_info (FFDB_DB* dbp, char* string)
{
  ffdb_set_user_info (dbp, (unsigned char *)string, strlen (string) + 1);
}

static void
init_config_info (FFDB_DB* dbp, unsigned int numconfig)
{
  ffdb_all_config_info_t acf;
  unsigned int i;

  acf.numconfigs = numconfig;
  acf.allconfigs = (ffdb_config_info_t *)calloc(1, numconfig * sizeof(ffdb_config_info_t));
  
  for (i = 0; i < numconfig; i++) {
    acf.allconfigs[i].config = i;
    acf.allconfigs[i].index = i;
    acf.allconfigs[i].inserted = 0;
    acf.allconfigs[i].type = 0;
    acf.allconfigs[i].mtime = 0;
    strcpy(acf.allconfigs[i].fname, "N/A");
  }

  ffdb_set_all_configs (dbp, &acf);

  free (acf.allconfigs);
}

static void* random_data (long size)
{
  char* data;
  long l;
  char tmp;

  data = (char *)malloc(size * sizeof(char));
  if (!data) {
    fprintf (stderr, "Cannot allocate space for a larget data of size %ld bytes\n", size); 
    exit (1);
  }

  for (l = 0; l < size - 1; l++) {
    tmp = rand() % LAST_CHAR;
    if (tmp < FIRST_CHAR)
      tmp += FIRST_CHAR;
    data[l] = (char)tmp;
  }
  data[l] = '\0';

  return data;
}

char	wp1[MAX_LEN];

int main(int argc, char** argv)
{
  FFDB_DBT item, key;
  FFDB_DB	*dbp;
  FFDB_HASHINFO ctl;
  char *p1, *p2, *dbase;
  char *end;
  long datasize;
  struct timeval tv;

  if (argc < 5) {
    fprintf (stderr, "Usage: %s cachesize rearrange(0|1) dbasename datasize\n",
	     argv[0]);
    exit(1);
  }

  int i = 0;
  
  argv++;
  ctl.hash = NULL;
  ctl.cmp = 0;
  ctl.cachesize = atoi(*argv++);
  ctl.bsize = 0;
  ctl.nbuckets = 0;
  ctl.rearrangepages = atoi(*argv++);
  ctl.numconfigs = 100;
  dbase = *argv++;
  datasize = strtol (*argv++, &end, 10);

  if (!(dbp = ffdb_dbopen( dbase,
			   O_CREAT|O_RDWR, 0600, &ctl))){
    /* create table */
    fprintf(stderr, "cannot create: hash table with filename %s\n",
	    dbase);
    exit(1);
  }

  gettimeofday (&tv, 0);
  srand (tv.tv_usec);

  /* Set user and configuration information */
  insert_user_info (dbp, USER_STRING);
  init_config_info (dbp, 100);

  fprintf (stderr, "Enter key string : \n");
  scanf ("%s", wp1);
  key.data = wp1;
  key.size = strlen(wp1) + 1;

  item.data = random_data (datasize);
  item.size = datasize;

  /*
   * enter key/data pair into the table
   */
  if ((dbp->put)(dbp, &key, &item, 0) != 0) {
    fprintf(stderr, "cannot enter: key %s\n",
	    (char *)key.data);
    exit(1);
  }
  
  (dbp->close)(dbp);

  return 0;
}
#else
int main (int argc, char** argv)
{
  return 0;
}
#endif
