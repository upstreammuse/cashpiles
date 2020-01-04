#include "cashpiles.h"

#include <QDate>
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

void processArguments(QString& dateFormat, QString& inFileName, QDate& today,
                      QStringList const& arguments)
{
   QRegularExpression const dateFormatRx("^--dateformat=(.*)$");
   QRegularExpression const inFileNameRx("^--file=(.*)$");
   QRegularExpression const todayRx("^--today=(.*)$");
   foreach (QString const& arg, arguments)
   {
      QRegularExpressionMatch match;
      if ((match = dateFormatRx.match(arg)).hasMatch())
      {
         dateFormat = match.captured(1);
      }
      else if ((match = inFileNameRx.match(arg)).hasMatch())
      {
         inFileName = match.captured(1);
      }
      else if ((match = todayRx.match(arg)).hasMatch())
      {
         today = QDate::fromString(match.captured(1), dateFormat);
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
