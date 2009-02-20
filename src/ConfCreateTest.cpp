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
 *   $Log: ConfCreateTest.cpp,v $
 *   Revision 1.1  2009-02-20 20:44:47  chen
 *   initial import
 *
 *
 *
 */
#include <fstream>
#include <iostream>
#include "DBString.h"
#include "ConfDataStoreDB.h"

#define USER_STRING "Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCDHello threre I am the new user string for QCD  Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD Hello threre I am the new user string for QCD"

using namespace std;
using namespace FFDB;

int
main (int argc, char** argv)
{
  if (argc < 6) {
    cerr << "Usage: " << argv[0] << " pagesize numpages rearrange(0|1) dbasename stringfile" << endl;
    return -1;
  }
  
  unsigned int pagesize = atoi (argv[1]);
  unsigned int nbuckets = atoi (argv[2]);
  int rearrange = atoi (argv[3]);
  std::string dbase (argv[4]);
  std::string strfile(argv[5]);

  // open string file stream
  ifstream sf(argv[5]);
  if (sf.bad()) {
    cerr << "Cannot open file " << strfile << endl;
    return -1;
  }
  
  // Open database
  ConfDataStoreDB<StringKey, UserData> dbtest;

  dbtest.setCacheSize (100000000);
  dbtest.setPageSize (pagesize);
  dbtest.setNumberBuckets (nbuckets);
  if  (rearrange)
    dbtest.enablePageMove ();
  dbtest.setMaxUserInfoLen (4000);
  dbtest.setMaxNumberConfigs (2);

  if (dbtest.open (dbase, O_RDWR | O_TRUNC, 0664) != 0) {
    cerr << "cannot open database " << dbase << endl;
    sf.close ();
    return -1;
  }

  std::string userdata(USER_STRING);
  if (dbtest.insertUserdata (userdata) != 0) {
    cerr << "Cannot insert user string" << endl;
    sf.close ();
    dbtest.close ();
    return -1;
  }

  while (sf.good()) {
    string t1, t2;

    sf >> t1;
    sf >> t2;

    StringKey key(t1);
    UserData  data(t2);

    if (dbtest.insert(key, data) != 0) {
      cerr << "Insert data error " << endl;
      sf.close ();
      dbtest.close ();
      return -1;
    }
  }
  sf.close ();
  dbtest.close ();

  return 0;
}
