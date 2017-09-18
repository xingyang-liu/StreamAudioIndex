//
// Created by watson on 10/08/17.
//

#ifndef HASH_0E_IDFCODE_H
#define HASH_0E_IDFCODE_H

class IdfCode
{
public:
    double idf;
    int code;
    IdfCode(){idf=0;code=-1;}
    IdfCode(double idf_tmp, int cod):idf(idf_tmp),code(cod){}
};


#endif //HASH_0E_IDFCODE_H
