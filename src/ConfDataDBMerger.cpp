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
 *     
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *
 * Revision History:
 *   $Log: ConfDataDBMerger.cpp,v $
 *   Revision 1.1  2009-02-20 20:44:48  chen
 *   initial import
 *
 *
 *
 */
#include <DBString.h>
#include "AllConfStoreDB.h"
#include "ConfDataDBMerger.h"

using namespace std;

namespace FFDB
{
  static void test_merge_code (void)
  {
    vector<ConfigInfo> configs;
    vector<string> dnames;
    vector<int> cfigs;
    vector<int> cfig_list;
    
    ConfDataDBMerger<StringKey, UserData> m1 (configs, 100000000);
    ConfDataDBMerger<StringKey, UserData> m2 (dnames, cfigs, cfig_list, 100000000);

    AllConfStoreDB<StringKey, UserData> dbh;

    m2.setDatabaseHandle (&dbh);
    m1.setDatabaseHandle (&dbh);

    m2.merge ();
  }
}

