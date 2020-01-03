#include <QCoreApplication>
#include <QDebug>
#include "accountbalancer.h"
#include "budgetallocator.h"
#include "cashpiles.h"
#include "currency.h"
#include "datevalidator.h"
#include "filereader.h"
#include "filewriter.h"
#include "itemprocessor.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgercomment.h"

// TODO need to add support for withholding category so income taxes don't break routine expense calculations

// TODO add validation to make sure categories don't exist in multiple places (two different types for the same name)

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   Currency::initializeCurrencies();

   QString dateFormat = QLocale::system().dateFormat(QLocale::ShortFormat);
   processArguments(dateFormat, app.arguments());

   Ledger ledger;

   FileReader reader("~/cashpiles.txt", ledger);
   reader.setDateFormat(dateFormat);
   if (!reader.readAll())
   {
      die("Failed to read input file");
   }

   DateValidator dv;
   ledger.processItems(dv);

   AccountBalancer ab;
   ledger.processItems(ab);

   BudgetAllocator budAlloc;
   ledger.processItems(budAlloc);

   FileWriter writer("~/cp-output-test.txt");
   writer.setDateFormat(dateFormat);
   ledger.processItems(writer);
   if (writer.success())
   {
      qDebug() << "successfully wrote file";
   }
   else
   {
      qDebug() << "failed to write file";
   }

   return EXIT_SUCCESS;
}
