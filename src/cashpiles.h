#ifndef CASHPILES_H
#define CASHPILES_H

class QString;
class QStringList;

[[noreturn]] void die(QString const& message);

[[noreturn]] void die(QString const& fileName, unsigned int lineNum,
                      QString const& message);

void processArguments(QString& dateFormat, QStringList const& arguments);

void warn(QString const& message);

void warn(QString const& fileName, unsigned int lineNum,
          QString const& message);

#endif
