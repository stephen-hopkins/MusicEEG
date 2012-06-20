#include "meexception.h"


MeException::MeException(string e) : error(e)
{
}

const char* MeException::what() const throw()
{
    return error.c_str();
}
