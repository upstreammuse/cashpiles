#include "cashpiles.h"

#include <QRegularExpression>
#include <QTextStream>

[[noreturn]] void die(QString const& message)
{
   warn(message);
   exit(EXIT_FAILURE);
}

[[noreturn]] void die(QString const& fileName, unsigned int lineNum,
                      QString const& message)
{
   warn(fileName, lineNum, message);
   exit(EXIT_FAILURE);
}

void processArguments(QString& dateFormat, QStringList const& arguments)
{
   QRegularExpression const dateFormatRx("^--dateformat=(.*)$");
   foreach (QString const& arg, arguments)
   {
      QRegularExpressionMatch match;
      if ((match = dateFormatRx.match(arg)).hasMatch())
      {
         dateFormat = match.captured(1);
      }
   }
}

void warn(QString const& message)
{
   QTextStream err(stderr);
   err << message << endl;
}

void warn(QString const& fileName, unsigned int lineNum,
          QString const& message)
{
   warn(QString("File '%1', line %2: %3")
        .arg(fileName)
        .arg(lineNum)
        .arg(message));
}
