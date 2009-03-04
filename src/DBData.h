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
 *     Interface for database data class
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: DBData.h,v $
 *   Revision 1.2  2009-03-04 15:55:25  chen
 *   Change Namespace from FFDB to FILEDB
 *
 *   Revision 1.1  2009/02/20 20:44:48  chen
 *   initial import
 *
 *
 *
 */
#ifndef _FILE_DB_DATA_H
#define _FILE_DB_DATA_H

#include "Serializable.h"

namespace FILEDB
{
  class DBData : public Serializable
  {
  public:
    /**
     * Destructor
     */
    ~DBData (void) {;}

  protected:
    /**
     * Constructor
     */
    DBData (void) {;}

  };
}
#endif
