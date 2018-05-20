#ifndef LEDGER_H
#define LEDGER_H

#include <QMap>
#include <QObject>
#include "ledgeraccount.h"
#include "ledgeraccountcommand.h"
#include "ledgerbudgetallocation.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

class Ledger : public QObject
{
   Q_OBJECT

public slots:
   void allocateBudget(LedgerBudgetAllocation const& allocation);
   void changeAccount(LedgerAccountCommand const& command);
   void comment(LedgerComment const& comment);
   void transact(LedgerTransaction const& transaction);

private:
   void validateDate(QDate const& date, int line);

private:
   QMap<QString, LedgerAccount> m_accounts;
   QMap<QString, int> m_categories;
   QDate m_latestDate;
};

#endif
