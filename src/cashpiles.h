#ifndef CASHPILES_H
#define CASHPILES_H

#include <string>

class Date;

[[noreturn]] void die(std::string const& message);

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      std::string const& message);

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      char const* message);

void processArguments(bool& convertYnab, std::string& dateFormat,
                      std::string& inFileName, std::string& outFileName,
                      Date& today, int argc, char** argv);

void warn(std::string const& message);

void warn(std::string const& fileName, size_t lineNum,
          std::string const& message);

#endif
