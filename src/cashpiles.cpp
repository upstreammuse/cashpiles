#include "cashpiles.h"

#include <clocale>
#include <iostream>
#include <regex>
#include <conio.h>
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

enum class Option
{
   NO_ACTION,
   QUIT,
   REFRESH
};

Option menu()
{
   std::cout << "Welcome to Cashpiles" << std::endl;
   std::cout << "(R)efresh | (Q)uit" << std::endl;
   Option option;
   do
   {
      int c = getch();
      switch (c)
      {
         case 'r':
         case 'R':
            option = Option::REFRESH;
            break;
         case 'Q':
            option = Option::QUIT;
            break;
         default:
            option = Option::NO_ACTION;
            break;
      }
   }
   while (option == Option::NO_ACTION);
   return option;
}

int main(int argc, char** argv)
{
   setlocale(LC_ALL, "");

   bool convertYnab = false;
   std::string dateFormat = "yyyy-MM-dd";
   std::string inFileName;
   std::string outFileName;
   Date today = Date::currentDate();
   processArguments(convertYnab, dateFormat, inFileName, outFileName, today,
                    argc, argv);
   if (!today.isValid())
   {
      today = Date::currentDate();
      std::stringstream ss;
      ss << "Today's date invalid, expected '" << dateFormat
         << "', using today's date " << today.toString(dateFormat);
      warn(ss.str());
   }

   Ledger ledger;
   Option option;
   do
   {
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

         IPBudgetAllocator budAlloc(today, ledger);
         ledger.processItems(budAlloc);
      }

      if (outFileName != "")
      {
         if (outFileName == inFileName)
         {
            warn("Refusing to overwrite original input file");
         }
         else
         {
            FileWriter writer(outFileName);
            writer.setDateFormat(dateFormat);
            ledger.processItems(writer);
         }
      }

      do
      {
         option = menu();
         // TODO
//         std::cout << *ledger.display(option); << std::endl;
      }
      while (option != Option::REFRESH && option != Option::QUIT);
   }
   while (option != Option::QUIT);

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
         std::cerr << "Unrecognized option '" << arg << "'" << std::endl;
         exit(EXIT_FAILURE);
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
