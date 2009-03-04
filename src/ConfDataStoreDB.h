// -*- C++ -*-
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
 *     A Base Class for storing one configuration of key objects 
 *     and associated vector in a hashed file
 * 
 *     This class is used by individual configuration generator to store
 *     data into a single database
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: ConfDataStoreDB.h,v $
 *   Revision 1.7  2009-03-04 15:55:25  chen
 *   Change Namespace from FFDB to FILEDB
 *
 *   Revision 1.6  2009/03/01 22:28:43  edwards
 *   Bug fix. Changed the "close" member function to actually return something
 *   since it is declared an int.
 *
 *   Revision 1.5  2009/02/28 20:45:51  edwards
 *   Bug fix. Changed the arguments to  binaryKeysAndData to use a reference
 *   so data can be returned.
 *
 *   Revision 1.4  2009/02/27 03:37:54  edwards
 *   Moved the "exist" function from AllConfStore to the base class ConfDataStore
 *
 *   Revision 1.3  2009/02/25 18:16:30  edwards
 *   Add 1 to user data length to account for null terminators.
 *
 *   Revision 1.2  2009/02/25 15:49:12  edwards
 *   Changed insertUserdata to take a const string.
 *
 *   Revision 1.1  2009/02/20 20:44:48  chen
 *   initial import
 *
 *
 *
 */
#ifndef _FFDB_CONF_DATA_STORE_DB_H
#define _FFDB_CONF_DATA_STORE_DB_H

#include <string>
#include "DBCursor.h"
#include "DBFunc.h"

#define FILEDB_DEFAULT_PAGESIZE 8192
#define FILEDB_DEFAULT_NUM_BUCKETS 32

namespace FILEDB
{
  /**
   * Class that store one configuration worth of data and keys
   */
  template <typename K, typename D>
  class ConfDataStoreDB 
  {
  protected:
    // database name
    std::string filename_;

    // all open options
    FFDB_HASHINFO options_;

    // opened database handle
    FFDB_DB* dbh_;

  public:

    /**
     * Empty constructor for a data store for one configuration
     */
    ConfDataStoreDB (void)
    :filename_(), dbh_()
    {
      crc32_init();
    
      ::memset (&options_, 0, sizeof(FFDB_HASHINFO));
      options_.bsize = FILEDB_DEFAULT_PAGESIZE;
      options_.nbuckets = FILEDB_DEFAULT_NUM_BUCKETS;
      // the other elements will be arranged by file hash package
    }

    /**
     * Destructor
     */
    virtual ~ConfDataStoreDB (void)
    {
      this->close ();
    }


    /**
     * How much data and keys should be kept in memory in bytes
     *
     * This should be called before the open is called
     * @param max_cache_size number of bytes of data and keys should be kept
     * in memory
     */
    virtual void setCacheSize (const unsigned int size)
    {
      options_.cachesize = size;
    }

    
    /**
     * Page size used when a new data based is created
     * This only effects a new database
     *
     * @param pagesize the pagesize used in hash database. This value
     * should be power of 2. The minimum value is 512 and maximum value
     * is 262144
     *
     */
    virtual void setPageSize (const unsigned int size)
    {
      options_.bsize = size;
    }


    /**
     * Set initial number of buckets
     *
     * This should be called before the open is called
     *
     * @param num the number of buckets should be used
     *
     */
    virtual void setNumberBuckets (const unsigned int num)
    {
      options_.nbuckets = num;
    }


    /**
     * Set whether to move pages when close to save disk space
     *
     * This only effective on writable database
     *
     */
    virtual void enablePageMove (void)
    {
      options_.rearrangepages = 1;
    }

    virtual void disablePageMove (void)
    {
      options_.rearrangepages = 0;
    }

    /**
     * Set and get maximum user information length
     */
    virtual void setMaxUserInfoLen (unsigned int len)
    {
      options_.userinfolen = len + 1;  // account for possible null terminator on string
    }

    virtual unsigned int getMaxUserInfoLen (void) const
    {
      if (!dbh_) 
	return options_.userinfolen;
    
      return ffdb_max_user_info_len (dbh_);
    }

    /**
     * Set and get maximum number of configurations
     */
    virtual void setMaxNumberConfigs (unsigned int num)
    {
      options_.numconfigs = num;
    }

    virtual unsigned int getMaxNumberConfigs (void) const
    {
      if (!dbh_) 
	return options_.numconfigs;
    
      return ffdb_num_configs (dbh_);
    }

    /**
     * Open
     * @param file filename holding all data and keys
     * @param open_flags: can be regular UNIX file open flags such as: O_RDONLY,
     * O_RDWR, O_TRUNC
     * @param mode regular unix file mode
     *
     * @return 0 on success, -1 on failure with proper errno set
     * 
     */
    virtual int open (const std::string& file, int open_flags, int mode)
    {
      this->dbh_ = openDatabase< K > (file, open_flags, mode, &options_);
      if (!this->dbh_)
	return -1;

      return 0;
    }


    virtual int close (void)
    {
      int ret = 0;
      if (this->dbh_) {
	ret = this->dbh_->close (this->dbh_);
	this->dbh_ = 0;
      }
      return ret;
    }

    /**
     * Insert a pair of data and key into the database
     * data is not ensemble, but a vector of complex.
     * @param key a key
     * @param data a user provided data
     *
     * @return 0 on successful write, -1 on failure with proper errno set
     */
    int insert (K& key, D& data)
    {
      int ret = 0;

      try {
	ret = insertData<K, D>(dbh_, key, data);
      }
      catch (SerializeException& e) {
	std::cerr << "ConfDataStoreDB insert error: " << e.what() << std::endl;
	ret = -1;
      }
      return ret;
    }

    /**
     * Insert a pair of data and key into the database in string format
     * @param key a key
     * @param data a user provided data
     *
     * @return 0 on successful write, -1 on failure with proper errno set
     */
    int insertBinary (std::string& key, std::string& data)
    {
      int ret = 0;

      try {
	ret = insertBinaryData(dbh_, key, data);
      }
      catch (SerializeException& e) {
	std::cerr << "ConfDataStoreDB insert error: " << e.what() << std::endl;
	ret = -1;
      }
      return ret;
    }


    /**
     * Get data for a given key
     * @param key user supplied key
     * @param data after the call data will be populated
     * @return 0 on success, otherwise the key not found
     */
    int get (K& key, D& data)
    {
      int ret = 0;

      try {
	ret = getData<K, D>(dbh_, key, data);
      }
      catch (SerializeException& e) {
	std::cerr << "ConfDataStoreDB get error: " << e.what () << std::endl;
	ret = -1;
      }
      return ret;
    }


    /**
     * Get data for a given key in binary form
     *
     * @param key user supplied key in string format
     * @param data after the call data will be populated
     * @return 0 on success, otherwise the key not found
     */
    int getBinary (std::string& key, std::string& data)
    {
      return getBinaryData (dbh_, key, data);
    }

    /**
     * Does this key exist in the store
     * @param key a key object
     * @return true if the answer is yes
     */
    virtual bool exist(K& key)
    {
      int ret;

      try {
	ret = keyExist< K > (dbh_, key);
      }
      catch (SerializeException& e) {
	std::cerr << "Key check exist error: " << e.what () << std::endl;
	ret = 0;
      }
      
      return ret;
    }

    /**
     * Return all available keys to user
     * @param keys user suppled an empty vector which is populated
     * by keys after this call.
     */
    virtual void keys (std::vector<K>& keys)
    {
      allKeys< K, D >(dbh_, keys);
    }

    /**
     * Return all pairs of keys and data
     * @param keys user supplied empty vector to hold all keys
     * @param data user supplied empty vector to hold data
     * @return keys and data in the vectors having the same size
     */
    virtual void keysAndData (std::vector<K>& keys, std::vector<D>& values)
    {
      allPairs<K, D> (dbh_, keys, values);
    }


    /**
     * Return all pairs of keys and data in binary string form
     * @param keys user supplied empty vector to hold all keys
     * @param data user supplied empty vector to hold data
     * @return keys and data in the vectors having the same size     
     */
    virtual void binaryKeysAndData (std::vector<std::string>& keys,
				    std::vector<std::string>& values)
    {
      binaryAllPairs (dbh_, keys, values);
    }

    /**
     * Flush database in memory to disk
     */
    virtual void flush (void)
    {
      flushDatabase (dbh_);
    }

    /**
     * Name of database associated with this Data store
     *
     * @return database name
     */
    virtual std::string storageName (void) const
    {
      return filename_;
    }

    
    /**
     * Insert user data into the  metadata database
     *
     * @param user_data user supplied data
     * @return returns 0 if success, else failure
     */
    virtual int insertUserdata (const std::string& user_data)
    {
      return ffdb_set_user_info (dbh_, 
				 (unsigned char *)user_data.c_str(), user_data.length());
    }
    
    /**
     * Get user user data from the metadata database
     *
     * @param user_data user supplied buffer to store user data
     * @return returns 0 if success. Otherwise failure.
     */
    virtual int getUserdata (std::string& user_data)
    {
      int ret;
      unsigned int len;
      unsigned char* data;

      len = ffdb_max_user_info_len (dbh_);
      data = new unsigned char[len];
      ret = ffdb_get_user_info (dbh_, data, &len);
      if (ret == 0) 
	user_data.assign((char *)data, len);

      // free memory
      delete []data;

      return ret;
    }
  };
}
#endif

