// TODO add last reconciled date/amount for accounts

// TODO native file format can ensure splits add up correctly

#include <QApplication>
#include <QTimer>
#include "accountant.h"
#include "csvreader.h"
#include "mainwindow.h"
#include "ynabbudget.h"
#include "ynabregister.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   MainWindow mw;
   mw.show();

   Accountant accounts;

   CsvReader registerReader("register.csv");
   YnabRegister reg;
   QObject::connect(&registerReader, SIGNAL(record(QHash<QString,QString>)),
                    &reg, SLOT(appendRecord(QHash<QString,QString>)));
   QObject::connect(&registerReader, SIGNAL(done()), &reg, SLOT(showTrans()));
   QObject::connect(&reg, SIGNAL(transaction(Transaction)),
                    &accounts, SLOT(appendTransaction(Transaction)));
   QObject::connect(&reg, SIGNAL(finished()), &accounts, SLOT(checkAccounts()));
   QObject::connect(&reg, SIGNAL(transaction(Transaction)),
                    &mw, SLOT(showTransaction(Transaction)));

   CsvReader budgetReader("budget.csv");
   YnabBudget budget;
   QObject::connect(&budgetReader, SIGNAL(record(QHash<QString,QString>)),
                    &budget, SLOT(appendRecord(QHash<QString,QString>)));
   QObject::connect(&budget, SIGNAL(transaction(Transaction)),
                    &mw, SLOT(showBudget(Transaction)));

   QTimer::singleShot(0, &registerReader, SLOT(read()));
   QTimer::singleShot(0, &budgetReader, SLOT(read()));
   return app.exec();
}
