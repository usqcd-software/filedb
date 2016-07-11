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
 *     Simple Test Program to Test ConfDataStoreDB Class With Huge Data
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: AllConfReadTest.cpp,v $
 *   Revision 1.2  2009-03-04 15:55:25  chen
 *   Change Namespace from FFDB to FILEDB
 *
 *   Revision 1.1  2009/02/20 20:44:47  chen
 *   initial import
 *
 *
 *
 */
#if defined (_FFDB_HUGE_DATA)
#include <cstdio>
#include <fstream>
#include <iostream>
#include "DBString.h"
#include "AllConfStoreDB.h"

using namespace std;
using namespace FILEDB;

int
main (int argc, char** argv)
{
  char keystr[80], datastr[128];
  if (argc < 6) {
    cerr << "Usage: " << argv[0] << " cachesize numconfigs rearrange(0|1) dbasename numkeys" << endl;
    return -1;
  }
  
  unsigned int cachesize = atoi (argv[1]);
  unsigned int numconfigs = atoi (argv[2]);
  int rearrange = atoi (argv[3]);
  std::string dbase (argv[4]);
  int numkeys = atoi(argv[5]);

  // Open database
  AllConfStoreDB<StringKey, UserData> dbtest;

  dbtest.setCacheSize (cachesize);
  if  (rearrange)
    dbtest.enablePageMove ();

  if (dbtest.open (dbase, O_RDONLY, 0400) != 0) {
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

    vector<UserData> rdatav;
    if (dbtest.get (key, rdatav) != 0) {
      cerr << "Cannot get vector data at " << i << endl;
      dbtest.close ();
      return -1;
    }
    else {
      for (int k = 0; k < numconfigs; k++)
        fprintf (stderr, "Found data of size %ld \n", ((string)(rdatav[k])).size());
    }
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

