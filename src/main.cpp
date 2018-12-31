#include <iostream>
#include <QCoreApplication>
#include <QTimer>
#include "accountbalancer.h"
#include "budgetbalancer.h"
#include "csvreader.h"
#include "currency.h"
#include "datevalidator.h"
#include "ledger.h"
#include "nativereader.h"
#include "nativewriter.h"
#include "ynabregisterreader.h"

void usage(QString const& progName)
{
   std::cerr << "USAGE: " << qPrintable(progName)
             << "(--file <nativefile> | --ynab <registerfile>)";
   exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   Currency::initializeCurrencies();

   CsvReader* csvRegisterReader(nullptr);
   NativeReader* nativeReader(nullptr);
   for (int i(0); i < app.arguments().size(); ++i)
   {
      if (app.arguments()[i] == "--ynab")
      {
         if (i + 1 >= app.arguments().size())
         {
            usage(argv[0]);
         }
         if (!csvRegisterReader)
         {
            csvRegisterReader = new CsvReader(app.arguments()[i + 1], &app);
            i++;
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
            i++;
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
//   ledger->addProcessor(new NativeWriter(ledger));

   if (csvRegisterReader)
   {
      YnabRegisterReader* ynabRegisterReader = new YnabRegisterReader(&app);
      QObject::connect(csvRegisterReader,
                       SIGNAL(record(QHash<QString,QString>,QString,int)),
                       ynabRegisterReader,
                       SLOT(processRecord(QHash<QString,QString>,QString,int)));
      QObject::connect(csvRegisterReader, SIGNAL(finished()),
                       ynabRegisterReader, SLOT(stop()));

      QObject::connect(ynabRegisterReader, SIGNAL(item(LedgerItem*)),
                       ledger, SLOT(processItem(LedgerItem*)));
      QObject::connect(ynabRegisterReader, SIGNAL(finished()),
                       ledger, SLOT(stop()));;

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
