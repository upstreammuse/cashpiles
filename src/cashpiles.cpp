#include "cashpiles.h"

#include <cassert>
#include <clocale>
#include <iostream>
#include <regex>
#include <sys/stat.h>
#include "currency.h"
#include "filereader.h"
#include "filewriter.h"
#include "ipaccountbalancer.h"
#include "ipbudgetallocator.h"
#include "ipdatevalidator.h"
#include "iptransactioncategorizer.h"
#include "ledger.h"
#include "reporter.h"
#include "rphtmlreporter.h"
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
   std::string reportDir;
   Date today = Date::currentDate();
   processArguments(convertYnab, dateFormat, inFileName, outFileName, reportDir,
                    today, argc, argv);
   if (inFileName == "")
   {
      die("No input file specified");
   }
   if (!today.isValid())
   {
      std::stringstream ss;
      ss << "Today's date invalid, expected '" << dateFormat << "'";
      die(ss.str());
   }
   if (reportDir != "")
   {
      struct stat status;
      if (stat(reportDir.c_str(), &status) != 0)
      {
         die("Report directory does not exist");
      }
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

   Reporter reporter;

   IPDateValidator dv;
   ledger.processItems(dv);

   if (!convertYnab)
   {
      IPTransactionCategorizer tc;
      ledger.processItems(tc);

      IPAccountBalancer ab(reporter);
      ledger.processItems(ab);

      IPBudgetAllocator budAlloc;
      ledger.processItems(budAlloc);

      assert(ab.budgetable() == budAlloc.budgetable());
   }

   RPHtmlReporter hr(reportDir, dateFormat);
   reporter.processReports(hr);

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

void processArguments(bool& convertYnab, std::string& dateFormat,
                      std::string& inFileName, std::string& outFileName,
                      std::string& reportDir, Date& today,
                      int argc, char** argv)
{
   static std::regex const dateFormatRx("^--dateformat=(.*)$");
   static std::regex const inFileNameRx("^--file=(.*)$");
   static std::regex const outFileNameRx("^--rewrite=(.*)$");
   static std::regex const reportDirRx("^--reports=(.*)$");
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
      else if (std::regex_match(arg, match, reportDirRx))
      {
         reportDir = match[1];
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
         std::stringstream ss;
         ss << "Unrecognized option '" << arg << "'";
         die(ss.str());
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
