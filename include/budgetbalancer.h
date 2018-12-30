#ifndef BUDGETBALANCER_H
#define BUDGETBALANCER_H

#include <QDate>
#include "ledgerbudget.h"
#include "currency.h"
#include "daterange.h"
#include "interval.h"
#include "itemprocessor.h"

class BudgetBalancer : public ItemProcessor
{
public:
   BudgetBalancer(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   void advancePeriodToDate(QDate const& date);
   void allocateCategories();
   void processAccount(LedgerAccountCommand const& account);
   void processRecords();
   void processTransaction(LedgerTransaction const& transaction);

private:
   struct PeriodReserve
   {
      Currency amount;
      DateRange range;
   };

private:
   QHash<QString, bool> m_accounts;
   Currency m_available;
   QHash<QString, LedgerBudget::Category> m_categories;
   DateRange m_period;
   QHash<QString, Currency> m_periodAllocations;
   QHash<QString, Currency> m_percentReserves;
   QHash<QString, PeriodReserve> m_periodReserves;
   DateRange m_priorDays;
   Currency m_routineEscrow;
   Currency m_routineTotal;

private:
   QHash<int, LedgerAccountCommand> m_recordedAccounts;
   QHash<int, LedgerTransaction> m_recordedTransactions;
   int m_numRecords = 0;
};

#endif
