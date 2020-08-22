#pragma once

#include <string>

class Date;

[[noreturn]] void die(std::string const& message);

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      std::string const& message);

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      char const* message);

void warn(std::string const& message);

void warn(std::string const& fileName, size_t lineNum,
          std::string const& message);
