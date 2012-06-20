#ifndef MEEXCEPTION_H
#define MEEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class MeException : public exception
{
public:
    MeException(string);
    const char* what() const throw();
private:
    string error;
};

#endif // MEEXCEPTION_H
