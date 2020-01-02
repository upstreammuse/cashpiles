#include <QDebug>
#include "accountbalancer.h"
#include "budgetallocator.h"
#include "currency.h"
#include "datevalidator.h"
#include "filereader.h"
#include "filewriter.h"
#include "itemprocessor.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgercomment.h"

// TODO need to add support for withholding category so income taxes don't break routine expense calculations

int main(int, char**)
{
   Currency::initializeCurrencies();
   Ledger ledger;

   FileReader reader("~/cashpiles.txt", ledger);
   reader.setDateFormat("M/d/yyyy");
   if (reader.readAll())
   {
      qDebug() << "successfully read file";
   }
   else
   {
      qDebug() << "failed to read file";
      return EXIT_FAILURE;
   }

   DateValidator dv;
   ledger.processItems(dv);

   AccountBalancer ab;
   ledger.processItems(ab);

   BudgetAllocator budAlloc;
   ledger.processItems(budAlloc);

   FileWriter writer("~/cp-output-test.txt");
   writer.setDateFormat("M/d/yyyy");
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
