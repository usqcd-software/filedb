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
 *     Simple Test Program to Test ConfDataStoreDB Class for huge data item
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: AllConfCreateTest.cpp,v $
 *   Revision 1.3  2009-03-04 15:55:25  chen
 *   Change Namespace from FFDB to FILEDB
 *
 *   Revision 1.2  2009/03/01 01:03:56  chen
 *   Add missing O_CREAT flag
 *
 *   Revision 1.1  2009/02/20 20:44:47  chen
 *   initial import
 *
 *
 *
 */
#if defined(_FFDB_HUGE_DATA)
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "DBString.h"
#include "AllConfStoreDB.h"

#define USER_STRING "Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCDHello threre I am the new user string for QCD  Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello There I am Seasoned User"


using namespace std;
using namespace FILEDB;

int
main (int argc, char** argv)
{
  char keystr[80], datastr[128];
  char* end;
  if (argc < 8) {
    cerr << "Usage: " << argv[0] << " pagesize numpages numconfigs rearrange(0|1) dbasename numkeys datasize" << endl;
    return -1;
  }

  std::string tmp;
  fprintf (stderr, "Maximum string size = %ld \n", tmp.max_size());
  
  unsigned int pagesize = atoi (argv[1]);
  unsigned int nbuckets = atoi (argv[2]);
  unsigned int numconfigs = atoi (argv[3]);
  int rearrange = atoi (argv[4]);
  std::string dbase (argv[5]);

  int numkeys = atoi (argv[6]);
  long datasize = ::strtol (argv[7], &end, 10);

  vector<int> configs;

  for (int i = 0; i < numconfigs; i++)
    configs.push_back(i);
  
  // Open database
  AllConfStoreDB<StringKey, UserData> dbtest(configs);

  dbtest.setCacheSize (100000000);
  dbtest.setPageSize (pagesize);
  dbtest.setNumberBuckets (nbuckets);
  if  (rearrange)
    dbtest.enablePageMove ();
  dbtest.setMaxUserInfoLen (5000);
  dbtest.setMaxNumberConfigs (numconfigs);  

  if (dbtest.open (dbase, O_RDWR | O_TRUNC | O_CREAT, 0664) != 0) {
    cerr << "cannot open database " << dbase << endl;
    return -1;
  }

  std::string userdata(USER_STRING);
  if (dbtest.insertUserdata (userdata) != 0) {
    cerr << "Cannot insert user string" << endl;
    dbtest.close ();
    return -1;
  }

  for (int i = 0; i < numkeys; i++) {
    ::sprintf (keystr, "Key Test Loop %d", i);
    StringKey key(keystr);
    char** datav = new char*[numconfigs];
    for (int k = 0; k < numconfigs; k++) {
      datav[k] = new char[datasize];      
      for (long m = 0; m < datasize; m++)
        datav[k][m] = (i + k + m) % 127;
    }
    
    vector<UserData> tdata;
    for (int k = 0; k < numconfigs; k++) {
      UserData tmp(datav[k], datasize);
      tdata.push_back (tmp);
    }

    fprintf (stderr,"Insert data into database \n");
    if (dbtest.insert (key, tdata) != 0) {
      cerr << "Cannot insert vector data at " << i << endl;
      dbtest.close ();
      return -1;
    }

    for (int k = 0; k < numconfigs; k++) 
      delete []datav[k];
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
