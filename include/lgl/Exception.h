#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace lgl {

class Error : public std::exception {
public:
    explicit Error(const std::string &whatArg) : whatArg(whatArg) {}
    const char *what() const noexcept override { return this->whatArg.c_str(); }

private:
    std::string whatArg;
};

class BuildError : public Error {
public:
    explicit BuildError(const std::string &whatArg) : Error(whatArg) {}
};

class LoadError : public Error {
public:
    explicit LoadError(const std::string &whatArg) : Error(whatArg) {}
};

} // namespace lgl
