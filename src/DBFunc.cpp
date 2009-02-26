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
 *     Template Functions for Database Storage and Retrieve
 *
 * 
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: DBFunc.cpp,v $
 *   Revision 1.2  2009-02-26 20:22:15  edwards
 *   Changed an error message to show the correct function name - binaryAllPairs.
 *
 *   Revision 1.1  2009/02/20 20:44:48  chen
 *   initial import
 *
 *
 *
 */
#include <string>
#include "DBFunc.h"

using namespace std;

namespace FFDB
{
  void flushDatabase (FFDB_DB* dbh)
  {
    dbh->sync (dbh, 0);
  }

  std::string unixDirectoryName (const std::string& filename)
  {
    string ret;
    size_t pos = filename.find_last_of ('/');
    if (pos != string::npos) 
      ret = filename.substr (0, pos);

    return ret;
  }

  void binaryAllPairs (FFDB_DB* dbh, 
		       std::vector<std::string>& keys, 
		       std::vector<std::string>& data) 
    throw (FileHashDBException)
  {
    FFDB_DBT  dbkey, dbdata;
    ffdb_cursor_t* crp;
    int  ret;
    
    // create cursor
    ret = dbh->cursor (dbh, &crp, 0);
    if (ret != 0) 
      throw FileHashDBException ("DBFunc binaryAllPairs", "Create Cursor Error");

    // get everything from meta data
    dbkey.data = dbdata.data = 0;
    dbkey.size = dbdata.size = 0;
    while ((ret = crp->get (crp, &dbkey, &dbdata, FFDB_NEXT)) == 0) {
      // convert into key object
      std::string keyObj;
      keyObj.assign((char*)dbkey.data, dbkey.size);

      // convert into data object
      std::string dataObj;
      dataObj.assign((char*)dbdata.data, dbdata.size);

      // put this new key into the vector
      keys.push_back (keyObj);
	
      // put this data into the other vector
      data.push_back (dataObj);

      // free memory
      free (dbkey.data); free (dbdata.data);
      dbkey.data = dbdata.data = 0;
      dbkey.size = dbdata.size = 0;
    }
    if (ret != FFDB_NOT_FOUND) 
      throw FileHashDBException ("DBFunc AllPairs", "Cursor Next Error");
    
    // close cursor
    if (crp != NULL)
      crp->close(crp);
  }

  int getBinaryData (FFDB_DB* dbh, std::string& key, std::string& data) 
  {
    int ret = 0;

    // create key
    FFDB_DBT dbkey;
    dbkey.data = &key[0];
    dbkey.size = key.size();

    // create and empty dbt data object
    FFDB_DBT dbdata;
    dbdata.data = 0;
    dbdata.size = 0;

    // now retrieve data from database
    ret = dbh->get (dbh, &dbkey, &dbdata, 0);
    if (ret == 0) {
      // convert object into a string
      data.assign((char*)dbdata.data, dbdata.size);
      // I have to use free since I use malloc in c code
      free(dbdata.data);
    }
    return ret;
  }  
  

  int insertBinaryData (FFDB_DB* dbh, std::string& key, std::string& data)
  {
    int ret;
       
    // create key
    FFDB_DBT dbkey;
    dbkey.data = &key[0];
    dbkey.size = key.size();
          
    // create DBt object
    FFDB_DBT dbdata;
    dbdata.data = &data[0];
    dbdata.size = data.size();

    // now it is time to insert
    ret = dbh->put (dbh, &dbkey, &dbdata, 0);

    return ret;
  }
}
