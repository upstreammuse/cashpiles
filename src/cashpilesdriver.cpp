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

// TODO consider what to do if a future transaction for a goal category is positive
// - it gets processed, so it adds to the 'available' funds for the category, which is confusing
// - it might be a valid use case, though, if planning for spend, refund, spend, etc.

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
   if (!reader.readAll())
   {
      die("Failed to read input file");
   }

   DateValidator dv;
   ledger.processItems(dv);

   AccountBalancer ab;
   ledger.processItems(ab);

   BudgetAllocator budAlloc(today);
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
