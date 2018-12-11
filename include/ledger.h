#ifndef LEDGER_H
#define LEDGER_H

#include <QMap>
#include <QObject>
#include "ledgertransaction.h"

class Ledger : public QObject
{
   Q_OBJECT

public:
   explicit Ledger(QObject* parent = 0);

public slots:
   void processItem(LedgerItem* item);
   void start();
   void stop();
//   void allocateBudget(LedgerBudgetAllocation const& allocation);
//   void changeAccount(LedgerAccountCommand const& command);
//   void comment(LedgerComment const& comment);
//   void transact(Transaction const& transaction);

signals:
   void finished();
   void started();

private:
//   void processLedgerCommand(LedgerCommand const* command);
//   void validateDate(QDate const& date, int line);

private:
//   QMap<QString, LedgerAccount> m_accounts;
//   QMap<QString, int> m_categories;
//   QList<LedgerCommand*> m_commands;
//   QDate m_latestDate;

private:
   QList<ItemProcessor*> m_processors;
};

#endif
