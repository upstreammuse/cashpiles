#ifndef CASHPILES_H
#define CASHPILES_H

#include <string>

class Date;
class QString;

[[noreturn]] void die(QString const& message);

[[noreturn]] void die(QString const& fileName, unsigned int lineNum,
                      QString const& message);

void processArguments(bool& convertYnab, std::string& dateFormat,
                      std::string& inFileName, std::string& outFileName,
                      Date& today, int argc, char** argv);

void warn(QString const& message);

void warn(QString const& fileName, unsigned int lineNum,
          QString const& message);

#endif
