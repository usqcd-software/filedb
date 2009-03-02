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
 *     Implementation for Vector Of floats For Testing Purpose
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: VFloatData.h,v $
 *   Revision 1.1  2009-03-02 23:27:26  chen
 *   Test DBMerge Code
 *
 *
 */
#ifndef _VFLOAT_DATA_H
#define _VFLOAT_DATA_H

#include <vector>
#include "DBData.h"

namespace FFDB
{
  class VFloatData : public DBData 
  {
  private:

    std::vector<float> data_;

  public:
    /**
     * Constructor
     */
    VFloatData (void);
    VFloatData (unsigned int capacity, float defvalue);
    VFloatData (const VFloatData& d);
    
    /**
     * Assignment operator
     */
    VFloatData& operator = (const VFloatData& d);


    /**
     * Equal and not eqaul operators
     */
    int operator == (const VFloatData& d);
    int operator != (const VFloatData& d);
    
    /**
     * Destructor
     */
    ~VFloatData (void) {;}


    /**
     * Convertion operator to retrieve internal data
     */
    operator const std::vector<float>& (void);

    /**
     * Return number of elements inside this vector
     */
    const int numberOfElements (void) const;


    /**
     * Get the serial id of this class
     */
    const unsigned short serialID (void) const {return 0x6677;}
    

    /**
     * Convert this object to a buffer
     *
     */
    void writeObject (std::string& output) throw (SerializeException);


    /**
     * Convert a buffer into an object
     *
     */
    void readObject (const std::string& input) throw (SerializeException);

  };
}
#endif
