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
 *     Interface for database key class
 *     Subclasses should at least provide "=" and "==" operators
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: DBKey.h,v $
 *   Revision 1.1  2009-02-20 20:44:48  chen
 *   initial import
 *
 *
 *
 */
#ifndef _DB_KEY_H
#define _DB_KEY_H

#include "Serializable.h"
#include <ffdb_db.h>

namespace FFDB
{
  /**
   * Database Hash function definition
   */
  typedef unsigned int (*ffdb_key_hash) (const void* bytes, 
					 unsigned int len); 

  /**
   * Database Key compare function definition
   */
  typedef int (*ffdb_key_compare) (const FFDB_DBT* k1, const FFDB_DBT* k2);

  class DBKey : public Serializable
  {
  public:
    /**
     * Destructor
     */
    ~DBKey (void) {;}

    /**
     * Does this key provide its own hash function
     * If this class is going to provide the hash function, use the
     * above hash function definition to implement a static function
     * with name hash
     *
     * @return 1 this class provide hash function, 0 otherwise
     */
    virtual int hasHashFunc (void) const = 0;

    /**
     * Does this key provide its own btree key compare function
     * If this class is going to provide the compare function, use the
     * above compare function definition to implement a static function
     * with name compare
     *
     * @return 1 this class provide compare function, 0 otherwise
     */
    virtual int hasCompareFunc (void) const = 0;

  protected:
    /**
     * Constructor
     */
    DBKey (void) {;}

  };
}
#endif