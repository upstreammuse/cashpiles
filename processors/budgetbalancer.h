#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include <QDate>
#include <QHash>
#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgertransaction.h"
#include "kernel/currency.h"
#include "kernel/daterange.h"
#include "kernel/interval.h"
#include "kernel/itemprocessor.h"
#include "processors/reserveamountallocator.h"
#include "processors/reservepercentallocator.h"
#include "processors/routineallocator.h"
#include "goalallocator.h"

class BudgetBalancer : public ItemProcessor
{
   Q_OBJECT

public:
   BudgetBalancer(QObject* parent);
   void processItem(LedgerAccountBalance const& balance);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

signals:
   void message(QString const&) const;
   void message(LedgerItem const&, QString const&) const;

private:
   void advancePeriodToDate(QDate const& date);
   void allocateCategories();
   void processAccount(LedgerAccountCommand const& account);
   void processRecords();
   void processTransaction(LedgerTransaction const& transaction);

private:
   QHash<QString, bool> m_accounts;
   Currency m_available;
   QHash<QString, LedgerBudget::Category> m_categories;
   GoalAllocator m_goalAllocator;
   int m_numRecords = 0;
   DateRange m_period;
   QHash<int, LedgerAccountCommand> m_recordedAccounts;
   QHash<int, LedgerTransaction> m_recordedTransactions;
   ReserveAmountAllocator m_reserveAmountAllocator;
   ReservePercentAllocator m_reservePercentAllocator;
   RoutineAllocator m_routineAllocator;
};

#endif
