#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "csvreader.h"
#include "ledger.h"
#include "nativereader.h"
#include "ynabbudgetreader.h"
#include "ynabconsolidator.h"
#include "ynabregisterreader.h"

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   CsvReader* csvBudgetReader(nullptr);
   CsvReader* csvRegisterReader(nullptr);
   NativeReader* nativeReader(nullptr);
   for (int i(0); i < app.arguments().size(); ++i)
   {
      if (app.arguments()[i] == "--ynab")
      {
         if (i + 2 >= app.arguments().size())
         {
            qFatal("Not enough arguments");
         }
         csvBudgetReader = new CsvReader(app.arguments()[i + 1], &app);
         csvRegisterReader = new CsvReader(app.arguments()[i + 2], &app);
         i += 2;
      }
      else if (app.arguments()[i] == "--file")
      {
         if (i + 1 >= app.arguments().size())
         {
            qFatal("Not enough arguments");
         }
         nativeReader = new NativeReader(app.arguments()[i + 1], &app);
         i += 1;
      }
   }

   // TODO for now implicitly allocate a basic processor
//   EchoProcessor* echo = new EchoProcessor(&app);
   Ledger* ledger = new Ledger(&app);
   QObject::connect(ledger, SIGNAL(finished()), &app, SLOT(quit()));
//   ledger->addProcessor(echo);

   // input depends on options
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
                       ynabConsolidator, SLOT(stopBudget()));
      QObject::connect(ynabBudgetReader, SIGNAL(item(LedgerItem*)),
                       ynabConsolidator, SLOT(processItem(LedgerItem*)));

      QObject::connect(csvRegisterReader,
                       SIGNAL(record(QHash<QString,QString>,QString,int)),
                       ynabRegisterReader,
                       SLOT(processRecord(QHash<QString,QString>,QString,int)));
      QObject::connect(csvRegisterReader, SIGNAL(finished()),
                       ynabRegisterReader, SLOT(stop()));
      QObject::connect(ynabRegisterReader, SIGNAL(item(LedgerItem*)),
                       ynabConsolidator, SLOT(processItem(LedgerItem*)));
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
      qFatal("file names not specifid correctly");
   }

   // And go
   QTimer::singleShot(0, ledger, SLOT(start()));
   return app.exec();
}
