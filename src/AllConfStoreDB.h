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
 *     Class for storing keys and corresponding vector of objects from
 *     all configurations
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: AllConfStoreDB.h,v $
 *   Revision 1.1  2009-02-20 20:44:47  chen
 *   initial import
 *
 *
 *
 */
#ifndef _ALL_CONF_DATA_STORE_DB_H
#define _ALL_CONF_DATA_STORE_DB_H

#include <vector>
#include <iterator>
#include <string>
#include "FFDB.h"
#include "DBCursor.h"
#include "DBFunc.h"
#include "ConfigInfo.h"
#include "ConfDataStoreDB.h"


namespace FFDB
{
  /**
   * Class that store keys and corresponding data for all configurations
   */
  template <typename K, typename D>
  class AllConfStoreDB : public ConfDataStoreDB <K, D>
  {
  private:
    /**
     * number of vectors in the assemble
     */
    int nbin_;

    /**
     * byte size of each element of a configuration
     */
    int bytesize_;

    /**
     * Configuration information
     */
    ffdb_all_config_info_t allcfgs_;

    /**
     * Current insertion record number
     */
    unsigned int curr_record_;

    /**
     * whether database is empty or not when the this object
     * is first created
     */
    int empty_;


    /**
     * Check whether data base is empty
     */
    int isEmpty (void) 
    {
      int ret;

      try {
	ret = isDatabaseEmpty<K> (this->dbh_);
      }
      catch (SerializeException& e) {
	std::cerr << "Check database empty failed with error : " << e.what () << std::endl;
	::exit (123);
      }
      return ret;
    }
   
  public:

    /**
     * Default constructor
     */
    AllConfStoreDB (void)
      :ConfDataStoreDB<K, D>(), nbin_(0), bytesize_(0), empty_(1)
    {
      allcfgs_.numconfigs = 0;
      allcfgs_.allconfigs = 0;
    }

    /**
     * Constructor taking a vector of configuration information
     */
    AllConfStoreDB (const std::vector<int>& configs)
      :ConfDataStoreDB<K, D>(), bytesize_ (0), empty_(1)
    {
      nbin_ = configs.size();

      allcfgs_.numconfigs = nbin_;
      allcfgs_.allconfigs = new ffdb_config_info_t[nbin_];
      for (int i = 0; i < nbin_; i++) {
	allcfgs_.allconfigs[i].config = configs[i];
	allcfgs_.allconfigs[i].index = i;
	allcfgs_.allconfigs[i].inserted = 0;
	allcfgs_.allconfigs[i].type = 0;
	allcfgs_.allconfigs[i].mtime = 0;
	allcfgs_.allconfigs[i].fname[0] = '\0';
      }

      // Set configuration maximum slot
      this->setMaxNumberConfigs (nbin_);
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
      this->dbh_ = openDatabase< K >(file,open_flags,mode, &(this->options_));
      if (!this->dbh_)
	return -1;
      
      // check whether this database is empty
      empty_ = this->isEmpty ();

      if (empty_) {
	if (nbin_ == 0) {
	  std::cerr << "One has to provide number of configurations." << std::endl;
	  return -1;
	}

	// update configuration information
	ffdb_set_all_configs (this->dbh_, &allcfgs_);
      }
      else {
	// update configuration information
	ffdb_get_all_configs (this->dbh_, &allcfgs_);
	nbin_ = allcfgs_.numconfigs;
	if (nbin_ == 0) {
	  std::cerr << "number of configurations = 0 on an existing database." << std::endl;
	  return -1;
	}
      }

      return 0;
    }
    

    /**
     * Destructor
     */
    virtual ~AllConfStoreDB (void)
    {
      if (allcfgs_.numconfigs > 0)
	delete []allcfgs_.allconfigs;
    }


   /**
     * Return a vector of complex numbers corresponding to a key
     * @param arg a key
     * @param vectors user supplied ensemble - here, an array single config corrs
     * @return 0 when there are something for this key. 1 wheen there are no
     */
    int get (K& key, std::vector< D >& vs)
    {
      int ret;
      std::string cdata;

      // get record number information from the database
      try {
	ret = getData <K, std::string> (this->dbh_, key, cdata);
      }
      catch (SerializeException& e) {
	std::cerr << "Retrieve record number information error : " << e.what () << std::endl;
	return -1;
      }

      if (ret != 0) {
	// no key is found
	return ret;
      }

      // this the first time I am calling this get
      if (cdata.size() % nbin_ != 0) {
	std::cerr << "Retreived data size " << cdata.size() << " is not multiple of number of configurations " << nbin_ << std::endl;
	return -1;
      }
      if (bytesize_ == 0) 
	bytesize_ = cdata.size()/nbin_;

      if (bytesize_ != cdata.size()/nbin_) {
	std::cerr << "Previous byte size " << bytesize_ << " is not the same as the current one " << cdata.size()/nbin_ << std::endl;
	return -1;
      }

      // now split this data blob into a vector
      char *rdata = (char *)&cdata[0];
      for (unsigned int i = 0; i < cdata.length(); i += bytesize_) {
	D elem;
	std::string tmp;
	tmp.assign (&rdata[i], bytesize_);
	try {
	  elem.readObject(tmp);
	}
	catch (SerializeException& e) {
	  std::cerr << "Serialize individual element error: " << e.what () << std::endl;
	  return -1;
	}
	// add to array
	vs.push_back (elem);
      }

      return 0;
    }


   /**
     * Return a vector of complex numbers corresponding to a key in string form
     * @param key a key in string format
     * @param vectors user supplied ensemble in string format
     * @return 0 when there are something for this key. 1 wheen there are no
     */
    int getBinaryData (std::string& key, std::vector<std::string>& vs)
    {
      int ret;
      std::string cdata;

      // get record number information from the database
      ret = this->getBinary (key, cdata);
      if (ret != 0) {
	// no key is found
	return ret;
      }
	
      // this the first time I am calling this get
      if (cdata.size() % nbin_ != 0) {
	std::cerr << "Retreived data size " << cdata.size() << " is not multiple of number of configurations " << nbin_ << std::endl;
	return -1;
      }
      if (bytesize_ == 0) 
	bytesize_ = cdata.size()/nbin_;

      if (bytesize_ != cdata.size()/nbin_) {
	std::cerr << "Previous byte size " << bytesize_ << " is not the same as the current one " << cdata.size()/nbin_ << std::endl;
	return -1;
      }

      // now split this data blob into a vector
      char *rdata = (char *)&cdata[0];
      for (unsigned int i = 0; i < cdata.length(); i += bytesize_) {
	std::string tmp;
	tmp.assign (&rdata[i], bytesize_);
	// add to array
	vs.push_back (tmp);
      }
      return 0;
    }


    /**
     * Insert a pair into the secondary storage
     * @param key a key provided by caller
     * @param v a vector containing all values
     */
    int insert (K & key, std::vector < D > & v)
    {
      int ret;
      // sanity check
      if (v.size () != (unsigned int)nbin_) {
	std::cerr << "Fatal: Number of vector in ensemble is not the same as expected " << v.size () 
		  << " != " << nbin_ << std::endl;
	::exit (1);
      }

      // if bytesize_ is not set, we set it now
      if (bytesize_ == 0) {
	std::string tmpstr;
	try {
	  v[0].writeObject (tmpstr);
	}
	catch (SerializeException& e) {
	  std::cerr << "Serialize the first element in insert error: " << e.what () 
		    << std::endl;
	  return -1;
	}
	bytesize_ = tmpstr.size();
      }

      // combine every element in the array into a string
      std::string dstr;
      for (unsigned int i = 0; i < v.size(); i++) {
	std::string elem;
	try {
	  v[i].writeObject (elem);
	}
	catch (SerializeException& e) {
	  std::cerr << "Serialize insert individual element error: " << e.what() << std::endl;
	  return -1;
	}
	dstr.append (elem);
      }

      std::cerr << "dstr has size " << dstr.size () << std::endl;
      // now insert this string into database
      try {
	ret = insertData< K > (this->dbh_, key, dstr);
      }
      catch (SerializeException& e) {
	std::cerr << "InsertData key and string error: " << e.what () << std::endl;
	::exit (1);
      }
      return ret;
    }

    /**
     * Insert a pair into the secondary storage
     * @param key a key provided by caller in string format
     * @param v a vector containing all values
     */
    int insertBinaryData (std::string& key, 
			  std::vector < std::string > & v)
    {
      int ret = 0;
      // sanity check
      if (v.size () != (unsigned int)nbin_) {
	std::cerr << "Fatal: Number of vector in ensemble is not the same as expected " << v.size () 
		  << " != " << nbin_ << std::endl;
	::exit (1);
      }

      // if bytesize_ is not set, we set it now
      if (bytesize_ == 0) 
	bytesize_ = v[0].size();

      // combine the vector of string into one big string
      std::string dstr;
      for (unsigned int i = 0; i < v.size(); i++) 
	dstr.append (v[i]);

      ret = this->insertBinary (key, dstr);
      if (ret != 0) {
	std::cerr << __FILE__ << "cannot insert binary data" << std::endl;
      }
      return ret;
    }



    /**
     * Update a pair of data and key into the database
     * data is not ensemble, but a single data
     * @param key a key object
     * @param v a single data object corresponding to a configuration
     * @param config configuration number of this pair
     * @param index when this data should be inside its ensemble
     */
    int update (K& key, D& v, const int config, const int index)
    {
      int ret = 0;

      if (empty_) {
	std::vector < D > thr;
	thr.resize (nbin_);
	thr[index] = v;
	ret = this->insert (key, thr);
      }
      else {
	// first find out where this record is going to be located
	std::string cdata;
	
	try {
	  ret = getData<K, std::string> (this->dbh_, key, cdata);
	}
	catch (SerializeException& e) {
	  std::cerr << "Get data error for update " << e.what () << std::endl;
	  return -1;
	}
      
	if (ret != 0) {
	  std::cerr << "Fatal in update: this key is not found here " << std::endl;
	  return -1;
	}
	

	if (cdata.size() % nbin_ != 0) {
	  std::cerr << "Retreived data size " << cdata.size() << " is not multiple of number of configurations " << nbin_ << std::endl;
	  return -1;
	}
	// this the first time I am calling this get
	if (bytesize_ == 0) 
	  bytesize_ = cdata.size()/nbin_;

	if (bytesize_ != cdata.size()/nbin_) {
	  std::cerr << "Previous byte size " << bytesize_ << " is not the same as the current one " << cdata.size()/nbin_ << std::endl;
	  return -1;
	}

	// convert the value into string
	std::string tmp;
	try {
	  v.writeObject (tmp);
	}
	catch (SerializeException& e) {
	  std::cerr << "Serialize a single value error: " << e.what() << std::endl;
	  return -1;
	}
	if (tmp.size() != bytesize_) {
	  std::cerr << "Update a single item has length " << tmp.size() << " not equal to the length of the other " << bytesize_ << std::endl;
	  return -1;
	}

	// Now update the content of this string
	char *rdata = (char *)&cdata[0];
	::memcpy (&rdata[index*bytesize_], &tmp[0], bytesize_);

	// Now insert this one into the database again
	try {
	  ret = insertData< K > (this->dbh_, key, cdata);
	}
	catch (SerializeException& e) {
	  std::cerr << "Update single item error: " << e.what() << std::endl;
	  ret = -1;
	}

	return ret;
      }
    }


    /**
     * Erase an entry from the storage
     * This routine is always successful. If there is no this entry,
     * this routine will return quietly
     * @param key a provided key object
     */
    void erase (K& key)
    {
      std::cerr << "erease not yet implemented " << std::endl;
    }

    /**
     * Does this key exist in the store
     * @param key a key object
     * @return true if the answer is yes
     */
    bool exist(K& key)
    {
      int ret;

      try {
	ret = keyExist< K > (this->dbh_, key);
      }
      catch (SerializeException& e) {
	std::cerr << "Key check exist error: " << e.what () << std::endl;
	ret = 0;
      }
      
      return ret;
    }


    /**
     * Tell the storage handler updating process is finished
     * This routine is for performance purpose to avoid
     * close underlying database during multiple updates.
     *
     * This routine should be called after one configuration
     * is finished.
     */
    void updateDone (void)
    {
      empty_ = 0;
    }


    /**
     * Number of configuration (bins) of this ensemble
     */
    int size (void) const {return nbin_;}


    /**
     * Update configuration information
     */
    int setConfigInfo (const ConfigInfo& config)
    {
      ffdb_config_info_t info;
      
      info.config = config.configNumber();
      info.index = config.index();
      info.inserted = config.inserted ();
      info.mtime = config.modifiedTime();
      strncpy (info.fname, config.urlname().c_str(), _FFDB_MAX_FNAME);

      return ffdb_set_config (this->dbh_, &info);
    }

    /**
     * Get configuration information
     * @param confignum configuration number we are interesed
     * @param config configuration information for this confignum. Caller
     * allocates the memory
     *
     * @return 0 on success, -1 otherwise
     */
    int getConfigInfo (unsigned int confignum,
		       ConfigInfo& info)
    {
      int stat;
      ffdb_config_info_t cf;

      stat = ffdb_get_config (this->dbh_, confignum, &cf);
      if (stat == 0) {
	info.configNumber (confignum);
	info.index (cf.index);
	info.insert (cf.inserted);
	info.type (cf.type);
	info.modifiedTime (cf.mtime);
	info.urlname (cf.fname);
      }
      return stat;
    }


    /**
     * Get all configuration information
     *
     */
    int getAllConfigInfo (std::vector<ConfigInfo> configs)
    {
      ffdb_all_config_info_t allcfigs;
      ConfigInfo cf;
      int stat, i;
      stat = ffdb_get_all_configs (this->dbh_, &allcfigs);

      if (stat == 0) {
	for (i = 0; i < allcfigs.numconfigs; i++) {
	  cf.configNumber (allcfigs.allconfigs[i].config);
	  cf.index (allcfigs.allconfigs[i].index);
	  cf.insert (allcfigs.allconfigs[i].inserted);
	  cf.type (allcfigs.allconfigs[i].type);
	  cf.modifiedTime (allcfigs.allconfigs[i].mtime);
	  cf.urlname (allcfigs.allconfigs[i].fname);

	  configs.push_back (cf);
	}
      }
      return stat;
    }


    /**
     * Define Key iterator
     */
    typedef DBKeyIterator < K, D > key_iterator;


    /**
     * Return key iterator
     */
    key_iterator begin (void) const
    {
      return DBKeyIterator < K, D >::init (this->dbh_);
    }

    key_iterator end   (void) const
    {
      K tmp;
      return key_iterator (DBCursor(), tmp, 1);
    }      

  };
}


#endif