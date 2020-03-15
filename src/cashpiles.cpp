#include "cashpiles.h"

#include <clocale>
#include <regex>
#include <QDate>
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
   setlocale(LC_ALL,"");
   bool convertYnab = false;
   std::string dateFormat = QLocale::system().dateFormat(QLocale::ShortFormat).toStdString();
   std::string inFileName;
   std::string outFileName;
   QDate today = QDate::currentDate();
   processArguments(convertYnab, dateFormat, inFileName, outFileName, today,
                    argc, argv);
   if (inFileName == "")
   {
      die("No input file specified");
   }
   if (!today.isValid())
   {
      die(QString("Today's date invalid, expected '%1'").arg(QString::fromStdString(dateFormat)));
   }

   Ledger ledger;
   if (convertYnab)
   {
      YnabRegisterReader reader(QString::fromStdString(inFileName), ledger);
      reader.setDateFormat(QString::fromStdString(dateFormat));
      reader.readAll();
   }
   else
   {
      FileReader reader(QString::fromStdString(inFileName), ledger);
      reader.setDateFormat(QString::fromStdString(dateFormat));
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
      FileWriter writer(QString::fromStdString(outFileName));
      writer.setDateFormat(QString::fromStdString(dateFormat));
      ledger.processItems(writer);
   }

   return EXIT_SUCCESS;
}

void processArguments(bool& convertYnab, std::string& dateFormat,
                      std::string& inFileName, std::string& outFileName,
                      QDate& today, int argc, char** argv)
{
   static std::regex const dateFormatRx("^--dateformat=(.*)$");
   static std::regex const inFileNameRx("^--file=(.*)$");
   static std::regex const outFileNameRx("^--rewrite=(.*)$");
   static std::regex const todayRx("^--today=(.*)$");
   static std::regex const ynabRx("^--ynab$");
   std::smatch match;
   for (int i = 1; i < argc; ++i)
   {
      std::string arg(argv[i]);
      if (std::regex_match(arg, match, dateFormatRx))
      {
         dateFormat = match.str(1);
      }
      else if (std::regex_match(arg, match, inFileNameRx))
      {
         inFileName = match.str(1);
      }
      else if (std::regex_match(arg, match, outFileNameRx))
      {
         outFileName = match.str(1);
      }
      else if (std::regex_match(arg, match, todayRx))
      {
         today = QDate::fromString(QString::fromStdString(match.str(1)),
                                   QString::fromStdString(dateFormat));
      }
      else if (std::regex_match(arg, match, ynabRx))
      {
         convertYnab = true;
      }
      else
      {
         die(QString("Unrecognized option '%1'").arg(QString::fromStdString(arg)));
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
