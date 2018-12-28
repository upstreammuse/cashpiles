#include <iostream>
#include <QCoreApplication>
#include <QTimer>
#include "accountbalancer.h"
#include "allocationminder.h"
#include "budgetbalancer.h"
#include "csvreader.h"
#include "currency.h"
#include "datevalidator.h"
#include "ledger.h"
#include "nativereader.h"
#include "nativewriter.h"
#include "ynabbudgetreader.h"
#include "ynabconsolidator.h"
#include "ynabregisterreader.h"

void usage(QString const& progName)
{
   std::cerr << "USAGE: " << qPrintable(progName)
             << "(--file <nativefile> | --ynab <budgetfile> <registerfile>)";
   exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   Currency::initializeCurrencies();

   CsvReader* csvBudgetReader(nullptr);
   CsvReader* csvRegisterReader(nullptr);
   NativeReader* nativeReader(nullptr);
   for (int i(0); i < app.arguments().size(); ++i)
   {
      if (app.arguments()[i] == "--ynab")
      {
         if (i + 2 >= app.arguments().size())
         {
            usage(argv[0]);
         }
         if (!csvBudgetReader && !csvRegisterReader)
         {
            csvBudgetReader = new CsvReader(app.arguments()[i + 1], &app);
            csvRegisterReader = new CsvReader(app.arguments()[i + 2], &app);
            i += 2;
         }
         else
         {
            usage(argv[0]);
         }
      }
      else if (app.arguments()[i] == "--file")
      {
         if (i + 1 >= app.arguments().size())
         {
            usage(argv[0]);
         }
         if (!nativeReader)
         {
            nativeReader = new NativeReader(app.arguments()[i + 1], &app);
            i += 1;
         }
         else
         {
            usage(argv[0]);
         }
      }
   }

   Ledger* ledger = new Ledger(&app);
   QObject::connect(ledger, SIGNAL(finished()), &app, SLOT(quit()));
   ledger->addProcessor(new DateValidator(ledger));
   ledger->addProcessor(new AccountBalancer(ledger));
   ledger->addProcessor(new BudgetBalancer(ledger));
   ledger->addProcessor(new AllocationMinder(ledger));
   ledger->addProcessor(new NativeWriter(ledger));

   if (csvBudgetReader && csvRegisterReader)
   {
      YnabBudgetReader* ynabBudgetReader = new YnabBudgetReader(&app);
      YnabRegisterReader* ynabRegisterReader = new YnabRegisterReader(&app);
      YnabConsolidator* ynabConsolidator = new YnabConsolidator(&app);
      QObject::connect(csvBudgetReader,
                       SIGNAL(record(QHash<QString,QString>,QString,int)),
                       ynabBudgetReader,
                       SLOT(processRecord(QHash<QString,QString>,QString,int)));
      QObject::connect(csvBudgetReader, SIGNAL(finished()),
                       ynabBudgetReader, SLOT(stop()));
      QObject::connect(ynabBudgetReader, SIGNAL(item(QDate,LedgerItem*)),
                       ynabConsolidator, SLOT(processItem(QDate,LedgerItem*)));
      QObject::connect(ynabBudgetReader, SIGNAL(finished()),
                       ynabConsolidator, SLOT(stopBudget()));;

      QObject::connect(csvRegisterReader,
                       SIGNAL(record(QHash<QString,QString>,QString,int)),
                       ynabRegisterReader,
                       SLOT(processRecord(QHash<QString,QString>,QString,int)));
      QObject::connect(csvRegisterReader, SIGNAL(finished()),
                       ynabRegisterReader, SLOT(stop()));
      QObject::connect(ynabRegisterReader, SIGNAL(item(QDate,LedgerItem*)),
                       ynabConsolidator, SLOT(processItem(QDate,LedgerItem*)));
      QObject::connect(ynabRegisterReader, SIGNAL(finished()),
                       ynabConsolidator, SLOT(stopRegister()));;

      QObject::connect(ynabConsolidator, SIGNAL(item(LedgerItem*)),
                       ledger, SLOT(processItem(LedgerItem*)));
      QObject::connect(ynabConsolidator, SIGNAL(finished()),
                       ledger, SLOT(stop()));

      QObject::connect(ledger, SIGNAL(started()),
                       csvBudgetReader, SLOT(readAll()));
      QObject::connect(ledger, SIGNAL(started()),
                       csvRegisterReader, SLOT(readAll()));
   }
   else if (nativeReader)
   {
      QObject::connect(nativeReader, SIGNAL(item(LedgerItem*)),
                       ledger, SLOT(processItem(LedgerItem*)));
      QObject::connect(nativeReader, SIGNAL(finished()),
                       ledger, SLOT(stop()));

      QObject::connect(ledger, SIGNAL(started()),
                       nativeReader, SLOT(readAll()));
   }
   else
   {
      usage(argv[0]);
   }

   QTimer::singleShot(0, ledger, SLOT(start()));
   return app.exec();
}
