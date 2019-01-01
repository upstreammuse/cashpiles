#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include "processors/accountbalancer.h"
#include "ui/accountbalancerui.h"
#include "processors/budgetbalancer.h"
#include "readers/csvreader.h"
#include "model/currency.h"
#include "processors/datevalidator.h"
#include "model/ledger.h"
#include "readers/nativereader.h"
#include "processors/nativewriter.h"
#include "readers/ynabregisterreader.h"
#include "ui/mainwindow.h"
#include "ui/messageui.h"

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
   NativeReader* nativeReader = new NativeReader(fileName, &app);

   MainWindow mw;
   mw.show();

   Ledger* ledger = new Ledger(&app);
   ledger->addProcessor(new DateValidator(ledger));

   auto accountBalancer = new AccountBalancer(mw.accountBalancerUI(), ledger);
   ledger->addProcessor(accountBalancer);
   QObject::connect(accountBalancer,
                    SIGNAL(balance(QString,bool,Currency)),
                    mw.accountBalancerUI(),
                    SLOT(setBalance(QString,bool,Currency)));
   QObject::connect(accountBalancer, SIGNAL(message(QString)),
                    mw.messageUI(), SLOT(appendMessage(QString)));
   QObject::connect(accountBalancer, SIGNAL(message(LedgerItem,QString)),
                    mw.messageUI(), SLOT(appendMessage(LedgerItem,QString)));

   auto budgetBalancer = new BudgetBalancer(ledger);
   ledger->addProcessor(budgetBalancer);
   QObject::connect(budgetBalancer, SIGNAL(message(QString)),
                    mw.messageUI(), SLOT(appendMessage(QString)));
   QObject::connect(budgetBalancer, SIGNAL(message(LedgerItem,QString)),
                    mw.messageUI(), SLOT(appendMessage(LedgerItem,QString)));

   QObject::connect(ledger, SIGNAL(started()),
                    nativeReader, SLOT(readAll()));
   QObject::connect(nativeReader, SIGNAL(item(LedgerItem*)),
                    ledger, SLOT(processItem(LedgerItem*)));
   QObject::connect(nativeReader, SIGNAL(finished()),
                    ledger, SLOT(stop()));
   QTimer::singleShot(0, ledger, SLOT(start()));

   return app.exec();
}
