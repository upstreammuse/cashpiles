#include <QCoreApplication>
#include <QTimer>
#include "kernel/ledger.h"
#include "processors/nativewriter.h"
#include "readers/csvreader.h"
#include "readers/ynabregisterreader.h"

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   Currency::initializeCurrencies();
   QString fileName = app.arguments().last();

   CsvReader* csvReader = new CsvReader(fileName, &app);
   YnabRegisterReader* ynabReader = new YnabRegisterReader(&app);
   Ledger* ledger = new Ledger(&app);
   NativeWriter* writer = new NativeWriter(ledger);
   ledger->addProcessor(writer);

   QObject::connect(ledger, SIGNAL(started()), csvReader, SLOT(readAll()));
   QObject::connect(csvReader, SIGNAL(record(QHash<QString,QString>,QString,int)), ynabReader, SLOT(processRecord(QHash<QString,QString>,QString,int)));
   QObject::connect(csvReader, SIGNAL(finished()), ynabReader, SLOT(stop()));
   QObject::connect(ynabReader, SIGNAL(item(LedgerItem*)), ledger, SLOT(processItem(LedgerItem*)));
   QObject::connect(ynabReader, SIGNAL(finished()), ledger, SLOT(stop()));
   QObject::connect(ledger, SIGNAL(finished()), &app, SLOT(quit()));

   QTimer::singleShot(0, ledger, SLOT(start()));
   return app.exec();
}
