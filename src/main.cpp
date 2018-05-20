#include <QApplication>
#include <QTimer>
#include "ledger.h"
#include "nativemodel.h"
#include "nativereader.h"
#include "settingsdialog.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   Ledger ledger;
   NativeModel model;
   NativeReader reader;
   SettingsDialog sd;

   QObject::connect(&sd, SIGNAL(nativeFileSelected(QString)),
                    &reader, SLOT(readAll(QString)));

   QObject::connect(&reader, SIGNAL(account(NativeAccountCommand)),
                    &model, SLOT(parseAccount(NativeAccountCommand)));
   QObject::connect(&reader, SIGNAL(budget(NativeBudgetCommand)),
                    &model, SLOT(parseBudget(NativeBudgetCommand)));
   QObject::connect(&reader, SIGNAL(comment(NativeComment)),
                    &model, SLOT(parseComment(NativeComment)));
   QObject::connect(&reader, SIGNAL(transaction(NativeTransaction)),
                    &model, SLOT(parseTransaction(NativeTransaction)));

   QObject::connect(&model, SIGNAL(accountChanged(LedgerAccountCommand)),
                    &ledger, SLOT(changeAccount(LedgerAccountCommand)));
   QObject::connect(&model, SIGNAL(budgetAllocated(LedgerBudgetAllocation)),
                    &ledger, SLOT(allocateBudget(LedgerBudgetAllocation)));
   QObject::connect(&model, SIGNAL(commented(LedgerComment)),
                    &ledger, SLOT(comment(LedgerComment)));
   QObject::connect(&model, SIGNAL(transacted(LedgerTransaction)),
                    &ledger, SLOT(transact(LedgerTransaction)));

   QTimer::singleShot(0, &sd, SLOT(syncSettings()));
   sd.show();
   return app.exec();
}
