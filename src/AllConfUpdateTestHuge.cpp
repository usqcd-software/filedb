/*----------------------------------------------------------------------------
 * Copyright (c) 2007      Jefferson Science Associates, LLC               
 *                         Under U.S. DOE Contract No. DE-AC05-06OR23177  
 *                                                                      
 *                         Thomas Jefferson National Accelerator Facility
 *
 *                         Jefferson Lab 
 *                         Scientific Computing Group,
 *                         12000 Jefferson Ave.,      
 *                         Newport News, VA 23606 
 *----------------------------------------------------------------------------
 *  
 * Description:
 *     Simple Test Program to Test ConfDataStoreDB Class
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: AllConfUpdateTest.cpp,v $
 *   Revision 1.2  2009-03-04 15:55:25  chen
 *   Change Namespace from FFDB to FILEDB
 *
 *   Revision 1.1  2009/02/20 20:44:47  chen
 *   initial import
 *
 *
 *
 */
#if defined(_FFDB_HUGE_DATA)
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include "DBString.h"
#include "AllConfStoreDB.h"


using namespace std;
using namespace FILEDB;

int
main (int argc, char** argv)
{
  char keystr[80], datastr[128];
  if (argc < 7) {
    cerr << "Usage: " << argv[0] << " cachesize numconfigs rearrange(0|1) dbasename numkeys datasize" << endl;
    return -1;
  }

  // generate random number
  struct timeval tv;
  gettimeofday (&tv, 0);
  srand (tv.tv_sec);
  
  unsigned int cachesize = atoi (argv[1]);
  unsigned int numconfigs = atoi (argv[2]);
  int rearrange = atoi (argv[3]);
  std::string dbase (argv[4]);
  int numkeys = atoi(argv[5]);

  char* end;
  long datasize = ::strtol (argv[6], &end, 10);

  // Open database
  AllConfStoreDB<StringKey, UserData> dbtest;

  dbtest.setCacheSize (cachesize);
  if  (rearrange)
    dbtest.enablePageMove ();

  if (dbtest.open (dbase, O_RDWR, 0664) != 0) {
    cerr << "cannot open database " << dbase << endl;
    return -1;
  }

  std::string userdata;
  if (dbtest.getUserdata (userdata) != 0) {
    cerr << "Cannot get user string" << endl;
    dbtest.close ();
    return -1;
  }
  cerr << userdata << endl;

  for (int i = 0; i < numkeys; i++) {
    ::sprintf (keystr, "Key Test Loop %d", i);
    StringKey key(keystr);
    char *datav = new char[datasize];      
    for (long m = 0; m < datasize; m++)
      datav[m] = (i + m + m) % 127;
    
    UserData rep (datav, datasize);

    int idx = rand() % numconfigs;
    fprintf (stderr, "Update data for configuration %d of %d\n",
             idx, numconfigs);
    if (dbtest.update (key, rep, numconfigs, idx) != 0) {
      cerr << "Cannot update data at " << idx << endl;
      dbtest.close ();
      return -1;
    }
    delete []datav;
  }

  dbtest.close ();

  return 0;
}
#else
int main (int argc, char** argv)
{
  return 0;
}
#endif

