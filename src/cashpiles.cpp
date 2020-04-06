#include "cashpiles.h"

#include <clocale>
#include <iostream>
#include <regex>
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

[[noreturn]] void die(std::string const& message)
{
   warn(message);
   exit(EXIT_FAILURE);
}

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      std::string const& message)
{
   warn(fileName, lineNum, message);
   exit(EXIT_FAILURE);
}

[[noreturn]] void die(std::string const& fileName, size_t lineNum,
                      char const* message)
{
   die(fileName, lineNum, std::string(message));
}

int main(int argc, char** argv)
{
   setlocale(LC_ALL,"");
   bool convertYnab = false;
   std::string dateFormat = "yyyy-MM-dd";
   std::string inFileName;
   std::string outFileName;
   Date today = Date::currentDate();
   processArguments(convertYnab, dateFormat, inFileName, outFileName, today,
                    argc, argv);
   if (inFileName == "")
   {
      die("No input file specified");
   }
   if (!today.isValid())
   {
      die(QString("Today's date invalid, expected '%1'").arg(QString::fromStdString(dateFormat)).toStdString());
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
                      Date& today, int argc, char** argv)
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
         today = Date::fromString(match.str(1), dateFormat);
      }
      else if (std::regex_match(arg, match, ynabRx))
      {
         convertYnab = true;
      }
      else
      {
         die(QString("Unrecognized option '%1'").arg(QString::fromStdString(arg)).toStdString());
      }
   }
}

void warn(std::string const& message)
{
   std::cerr << message << std::endl;
}

void warn(std::string const& fileName, size_t lineNum,
          std::string const& message)
{
   std::stringstream ss;
   ss << "File '" << fileName << "', line " << lineNum << ": " << message;
   warn(ss.str());
}
