#include <QDebug>
#include "accountbalancer.h"
#include "currency.h"
#include "datevalidator.h"
#include "filereader.h"
#include "filewriter.h"
#include "itemprocessor.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgercomment.h"
#include "routineallocator.h"

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
   }

   DateValidator dv;
   ledger.processItems(dv);

   AccountBalancer ab;
   ledger.processItems(ab);

//   BudgetBalancer bb;
//   ledger.processItems(bb);

   RoutineAllocator routAlloc;
   ledger.processItems(routAlloc);

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
