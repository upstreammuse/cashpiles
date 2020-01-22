#include "cashpiles.h"

#include <QCoreApplication>
#include <QDate>
#include <QRegularExpression>
#include <QTextStream>
#include "currency.h"
#include "filereader.h"
#include "filewriter.h"
#include "ipaccountbalancer.h"
#include "ipbudgetallocator.h"
#include "ipdatevalidator.h"
#include "iptransactioncategorizer.h"
#include "iptransferbalancer.h"
#include "ledger.h"

// TODO category owners are silently created when '@' is used on a category name that otherwise doesn't exist
//   this made it tricky to spot the error with a badly formatted transfer transaction

// TODO if a category isn't a goal, then future transactions for it are ignored, so there is no check or prompt to save for them

// TODO need to add support for withholding category so income taxes don't break routine expense calculations

// TODO add validation to make sure categories don't exist in multiple places (two different types for the same name)

// TODO consider what to do if a future transaction for a goal category is positive
// - it gets processed, so it adds to the 'available' funds for the category, which is confusing
// - it might be a valid use case, though, if planning for spend, refund, spend, etc.

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

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   Currency::initializeCurrencies();

   QString dateFormat = QLocale::system().dateFormat(QLocale::ShortFormat);
   QString inFileName;
   QDate today = QDate::currentDate();
   processArguments(dateFormat, inFileName, today, app.arguments());
   if (inFileName.isNull())
   {
      die("No input file specified");
   }
   if (!today.isValid())
   {
      die(QString("Today's date invalid, expected '%1'").arg(dateFormat));
   }

   Ledger ledger;
   FileReader reader(inFileName, ledger);
   reader.setDateFormat(dateFormat);
   reader.readAll();

   IPDateValidator dv;
   ledger.processItems(dv);

   IPTransferBalancer tb;
   ledger.processItems(tb);

   IPTransactionCategorizer tc;
   ledger.processItems(tc);

   IPAccountBalancer ab(today);
   ledger.processItems(ab);

   IPBudgetAllocator budAlloc(today);
   ledger.processItems(budAlloc);

   FileWriter writer("~/cp-output-test.txt");
   writer.setDateFormat(dateFormat);
   ledger.processItems(writer);

   return EXIT_SUCCESS;
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
