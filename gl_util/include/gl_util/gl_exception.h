#pragma once

///
/// \brief Custom utility gl exception classes.
///

#include <exception>
#include <stdexcept>
#include <string>

namespace gl {

///
/// \brief The Error class is the base class for all OpenGL-related exceptions/errors.
///
class Error : public std::exception {
public:
    explicit Error(const std::string& whatArg) : whatArg(whatArg){}
    virtual const char* what() const noexcept override {return this->whatArg.c_str();}

private:
    std::string whatArg;
};

///
/// \brief The BuildError class deals with errors that may occur when compiling/linking shaders.
///
class BuildError : public Error {
public:
    explicit BuildError(const std::string& whatArg) : Error(whatArg){}
};

} // namespace gl
