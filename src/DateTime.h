//
//  DateTime.h
//  guiFromParametersExample
//
//  Created by Jason Rasmussen on 7/26/14.
//
//

#ifndef guiFromParametersExample_DateTime_h
#define guiFromParametersExample_DateTime_h

#include "ofMain.h"

class DateTime
{
    public :
    DateTime() { }
    DateTime( int h , int m , int s, int v )
    {
        hour = h ;
        minute = m ;
        second = s ;
        value = v ; 
    }
    int hour , minute, second , value ;
    
    string toString()
    {
        stringstream ss ;
        ss << "@ " << hour << " : " << minute << " : " << second ;
        return ss.str() ;
    }
};

#endif
