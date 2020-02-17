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
#include "ynabregisterreader.h"

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

   bool convertYnab = false;
   QString dateFormat = QLocale::system().dateFormat(QLocale::ShortFormat);
   QString inFileName;
   QString outFileName;
   QDate today = QDate::currentDate();
   processArguments(convertYnab, dateFormat, inFileName, outFileName, today,
                    app.arguments());
   if (inFileName.isNull())
   {
      die("No input file specified");
   }
   if (!today.isValid())
   {
      die(QString("Today's date invalid, expected '%1'").arg(dateFormat));
   }

   Ledger ledger;
   if (convertYnab)
   {
      YnabRegisterReader reader(inFileName, ledger);
      reader.setDateFormat(dateFormat);
      reader.readAll();
   }
   else
   {
      FileReader reader(inFileName, ledger);
      reader.setDateFormat(dateFormat);
      reader.readAll();
   }

   IPDateValidator dv;
   ledger.processItems(dv);

   IPTransferBalancer tb;
   ledger.processItems(tb);

   if (!convertYnab)
   {
      IPTransactionCategorizer tc;
      ledger.processItems(tc);

      IPAccountBalancer ab(today);
      ledger.processItems(ab);

      IPBudgetAllocator budAlloc(today);
      ledger.processItems(budAlloc);
   }

   if (outFileName != "")
   {
      if (outFileName == inFileName)
      {
         die("Refusing to overwrite original input file");
      }
      FileWriter writer(outFileName);
      writer.setDateFormat(dateFormat);
      ledger.processItems(writer);
   }

   return EXIT_SUCCESS;
}

void processArguments(bool& convertYnab, QString& dateFormat,
                      QString& inFileName, QString& outFileName, QDate& today,
                      QStringList const& arguments)
{
   QRegularExpression const dateFormatRx("^--dateformat=(.*)$");
   QRegularExpression const inFileNameRx("^--file=(.*)$");
   QRegularExpression const outFileNameRx("^--rewrite=(.*)$");
   QRegularExpression const todayRx("^--today=(.*)$");
   QRegularExpression const ynabRx("^--ynab$");
   foreach (QString const& arg, arguments.mid(1))
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
      else if ((match = outFileNameRx.match(arg)).hasMatch())
      {
         outFileName = match.captured(1);
      }
      else if ((match = todayRx.match(arg)).hasMatch())
      {
         today = QDate::fromString(match.captured(1), dateFormat);
      }
      else if ((match = ynabRx.match(arg)).hasMatch())
      {
         convertYnab = true;
      }
      else
      {
         die(QString("Unrecognized option '%1'").arg(arg));
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
