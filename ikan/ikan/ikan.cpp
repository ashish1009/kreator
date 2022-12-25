//
//  ikan.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include <iostream>
#include "ikan.hpp"
#include "ikanPriv.hpp"

void ikan::HelloWorld(const char * s)
{
    ikanPriv *theObj = new ikanPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void ikanPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

