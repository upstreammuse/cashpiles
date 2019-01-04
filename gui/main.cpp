#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include "processors/accountbalancer.h"
#include "processors/budgetbalancer.h"
#include "kernel/currency.h"
#include "processors/datevalidator.h"
#include "kernel/ledger.h"
#include "readers/nativereader.h"
#include "processors/nativewriter.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   Currency::initializeCurrencies();

   QSettings* settings = new QSettings("upstreammuse", "CashPiles", &app);
   QString filePath = settings->value("filePath").toString();
   QString fileName = QFileDialog::getOpenFileName(0, "Open CashPiles File",
                                                   filePath);
   settings->setValue("filePath",
                      QFileInfo(fileName).absoluteDir().canonicalPath());

   MainWindow mw;
   mw.show();
   Ledger* ledger = new Ledger(&app);

   NativeReader* nativeReader = new NativeReader(fileName, &app);
   QObject::connect(nativeReader, SIGNAL(message(QString)),
                    &mw, SLOT(appendMessage(QString)));
   QObject::connect(nativeReader, SIGNAL(message(LedgerItem,QString)),
                    &mw, SLOT(appendMessage(LedgerItem,QString)));

   auto accountBalancer = new AccountBalancer(ledger);
   ledger->addProcessor(accountBalancer);
   QObject::connect(accountBalancer, SIGNAL(balance(QString,bool,Currency)),
                    &mw, SLOT(setAccountBalance(QString,bool,Currency)));
   QObject::connect(accountBalancer, SIGNAL(message(QString)),
                    &mw, SLOT(appendMessage(QString)));
   QObject::connect(accountBalancer, SIGNAL(message(LedgerItem,QString)),
                    &mw, SLOT(appendMessage(LedgerItem,QString)));

   auto budgetBalancer = new BudgetBalancer(ledger);
   ledger->addProcessor(budgetBalancer);
   QObject::connect(budgetBalancer, SIGNAL(message(QString)),
                    &mw, SLOT(appendMessage(QString)));
   QObject::connect(budgetBalancer, SIGNAL(message(LedgerItem,QString)),
                    &mw, SLOT(appendMessage(LedgerItem,QString)));

   auto dateValidator = new DateValidator(ledger);
   ledger->addProcessor(dateValidator);
   QObject::connect(dateValidator, SIGNAL(message(LedgerItem,QString)),
                    &mw, SLOT(appendMessage(LedgerItem,QString)));

   QObject::connect(ledger, SIGNAL(started()),
                    nativeReader, SLOT(readAll()));
   QObject::connect(nativeReader, SIGNAL(item(LedgerItem*)),
                    ledger, SLOT(processItem(LedgerItem*)));
   QObject::connect(nativeReader, SIGNAL(finished()),
                    ledger, SLOT(stop()));

   QObject::connect(ledger, SIGNAL(finished()), &mw, SLOT(beautify()));

   QTimer::singleShot(0, ledger, SLOT(start()));
   return app.exec();
}
