#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include <QDate>
#include <QHash>
#include <QSharedPointer>
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "currency.h"
#include "daterange.h"
#include "interval.h"
#include "itemprocessor.h"
#include "reserveamountallocator.h"
#include "reservepercentallocator.h"
#include "routineallocator.h"
#include "goalallocator.h"

// TODO move this
class BudgetAllocator
{
public:
   virtual ~BudgetAllocator();
   virtual Currency deallocate(QString const& category);
};

class BudgetBalancer : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   void advancePeriodToDate(QDate const& date);
   void allocateCategories();
   void processAccount(LedgerAccount const& account);
   void processRecords();
   void processTransaction(LedgerTransaction const& transaction);

private:
   QHash<QString, bool> m_accounts;
   QHash<LedgerBudgetEntry::Type, QSharedPointer<BudgetAllocator>> m_allocators;
   Currency m_available;
   QHash<QString, LedgerBudgetEntry::Type> m_categories;
   GoalAllocator m_goalAllocator;
   int m_numRecords = 0;
   DateRange m_period;
   QHash<int, LedgerAccount> m_recordedAccounts;
   QHash<int, LedgerTransaction> m_recordedTransactions;
   ReserveAmountAllocator m_reserveAmountAllocator;
   ReservePercentAllocator m_reservePercentAllocator;
   RoutineAllocator m_routineAllocator;
};

#endif
