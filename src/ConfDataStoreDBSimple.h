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
 *   Revision 1.12  2009-08-28 15:42:22  edwards
 *   Added a fileExists function.
 *
 *   Revision 1.11  2009/03/11 13:56:09  edwards
 *   Changed default cache size back to 8192
 *
 *   Revision 1.10  2009/03/04 19:31:33  chen
 *   Change to 16384 default pagesize
 *
 *   Revision 1.9  2009/03/04 19:15:17  edwards
 *   Changed include guard to avoid collisions with ffdb-lite.
 *
 *   Revision 1.7  2009/03/04 15:55:25  chen
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
#ifndef _FILEDB_CONF_DATA_STORE_DB_SIMPLE_H
#define _FILEDB_CONF_DATA_STORE_DB_SIMPLE_H

#include "DBCursor.h"
#include "DBFunc.h"
#include <memory>
#include <string>

#define FILEDB_DEFAULT_PAGESIZE 8192
#define FILEDB_DEFAULT_NUM_BUCKETS 32

namespace FILEDB
{
  /**
   * Class that store one configuration worth of data and keys
   */
  class ConfDataStoreDBSimple
  {
  protected:

    struct DB {
      // database name
      std::string filename_;

      // all open options
      FFDB_HASHINFO options_;

      // opened database handle
      FFDB_DB *dbh_;

      DB() {
        dbh_ = nullptr;

        ::memset(&options_, 0, sizeof(FFDB_HASHINFO));
        options_.bsize = FILEDB_DEFAULT_PAGESIZE;
        options_.nbuckets = FILEDB_DEFAULT_NUM_BUCKETS;
        // the other elements will be arranged by file hash package
      }

      ~DB() { close(); }

      int open(const std::string &file, int open_flags, int mode) {
        close();
        dbh_ = openDatabaseSimple(file, open_flags, mode, &options_);
	if (!dbh_)
	  return -1;
	filename_ = file;
        return 0;
      }

      void close(void) {
        int ret = 0;
        if (dbh_) {
          ret = dbh_->close(dbh_);
          dbh_ = nullptr;
        }
        if (ret != 0)
          throw std::runtime_error("Error closing filedb database: " + filename_);
      }
    };

    // database reference
    std::shared_ptr<DB> db;

  public:

    /**
     * Empty constructor for a data store for one configuration
     */
    ConfDataStoreDBSimple (void)
    {
      crc32_init();
      db = std::make_shared<DB>();
    }

    /**
     * Destructor
     */
    ~ConfDataStoreDBSimple ()
    {
    }


    /**
     * How much data and keys should be kept in memory in bytes
     *
     * This should be called before the open is called
     * @param max_cache_size number of bytes of data and keys should be kept
     * in memory
     */
    void setCacheSize (const unsigned int size)
    {
      db->options_.cachesize = size;
    }

    /**
     * How much data and keys should be kept in memory in megabytes
     *
     * This should be called before the open is called
     * @param max_cache_size number of bytes of data and keys should be kept
     * in memory
     */
    void setCacheSizeMB (const unsigned int size)
    {
      if (sizeof(unsigned long) == sizeof(int)) {
	// this is a 32 bit machine, we need to make sure
	// we do not overflow the 32 bit integer here
	int i = 1;
	unsigned int tsize = size;
	while (i <= 20) {
	  tsize = (tsize << 1);
	  if (i < 20 && tsize >= (unsigned int)0x80000000) {
	    std::cerr << "Database cache size exceeds maximum unsigned int" << std::endl;
	    tsize = 0xFFFFFFFF;
	    break;
	  }
	  i++;
	}
	db->options_.cachesize = tsize;
      }
      else 
	db->options_.cachesize = ((unsigned long)size) << 20;
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
    void setPageSize (const unsigned int size)
    {
      db->options_.bsize = size;
    }


    /**
     * Set initial number of buckets
     *
     * This should be called before the open is called
     *
     * @param num the number of buckets should be used
     *
     */
    void setNumberBuckets (const unsigned int num)
    {
      db->options_.nbuckets = num;
    }


    /**
     * Set whether to move pages when close to save disk space
     *
     * This only effective on writable database
     *
     */
    void enablePageMove (void)
    {
      db->options_.rearrangepages = 1;
    }

    void disablePageMove (void)
    {
      db->options_.rearrangepages = 0;
    }

    /**
     * Set and get maximum user information length
     */
    void setMaxUserInfoLen (unsigned int len)
    {
      db->options_.userinfolen = len + 1;  // account for possible null terminator on string
    }

    unsigned int getMaxUserInfoLen (void) const
    {
      if (!db->dbh_) 
	return db->options_.userinfolen;
    
      return ffdb_max_user_info_len (db->dbh_);
    }

    /**
     * Set and get maximum number of configurations
     */
    void setMaxNumberConfigs (unsigned int num)
    {
      db->options_.numconfigs = num;
    }

    unsigned int getMaxNumberConfigs (void) const
    {
      if (!db->dbh_) 
	return db->options_.numconfigs;
    
      return ffdb_num_configs (db->dbh_);
    }


    /**
     * Check if a DB file exists before opening.
     */
    bool static fileExists (const std::string& file)
    {
      return FILEDB::fileExists(file);
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
    int open (const std::string& file, int open_flags, int mode)
    {
      return db->open(file, open_flags, mode);
    }


    int close (void)
    {
      db->close();
      return 0;
    }

    /**
     * Insert a pair of data and key into the database
     * data is not ensemble, but a vector of complex.
     * @param key a key
     * @param data a user provided data
     *
     * @return 0 on successful write, -1 on failure with proper errno set
     */
    template <typename K, typename D> int insert(const K &key, const D &data) {
      int ret = 0;

      try {
	ret = insertData<K, D>(db->dbh_, key, data);
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
    template <typename K, typename D> int get(const K &key, D &data) const {
      int ret = 0;

      try {
	ret = getData<K, D>(db->dbh_, key, data);
      }
      catch (SerializeException& e) {
	std::cerr << "ConfDataStoreDB get error: " << e.what () << std::endl;
	ret = -1;
      }
      return ret;
    }

    /**
     * Does this key exist in the store
     * @param key a key object
     * @return true if the answer is yes
     */
    template <typename K> bool exist(const K &key) const {
      int ret;

      try {
	ret = keyExist< K > (db->dbh_, key);
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
    template <typename K, typename D> void keys(std::vector<K> &keys) const {
      allKeys< K, D >(db->dbh_, keys);
    }

    /**
     * Return all pairs of keys and data
     * @param keys user supplied empty vector to hold all keys
     * @param data user supplied empty vector to hold data
     * @return keys and data in the vectors having the same size
     */
    template <typename K, typename D>
    void keysAndData(std::vector<K> &keys, std::vector<D> &values) const {
      allPairs<K, D> (db->dbh_, keys, values);
    }

    /**
     * Flush database in memory to disk
     */
    void flush (void)
    {
      flushDatabase (db->dbh_);
    }

    /**
     * Name of database associated with this Data store
     *
     * @return database name
     */
    std::string storageName (void) const
    {
      return db->filename_;
    }

    
    /**
     * Insert user data into the  metadata database
     *
     * @param user_data user supplied data
     * @return returns 0 if success, else failure
     */
    int insertUserdata (const std::string& user_data)
    {
      return ffdb_set_user_info (db->dbh_, 
				 (unsigned char *)user_data.c_str(), user_data.length());
    }
    
    /**
     * Get user user data from the metadata database
     *
     * @param user_data user supplied buffer to store user data
     * @return returns 0 if success. Otherwise failure.
     */
    int getUserdata (std::string& user_data)
    {
      int ret;
      unsigned int len;
      unsigned char* data;

      len = ffdb_max_user_info_len (db->dbh_);
      data = new unsigned char[len];
      ret = ffdb_get_user_info (db->dbh_, data, &len);
      if (ret == 0) 
	user_data.assign((char *)data, len);

      // free memory
      delete []data;

      return ret;
    }
  };
}
#endif
