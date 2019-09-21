#ifndef ROUTINEALLOCATOR_H
#define ROUTINEALLOCATOR_H

#include <QSet>
#include "currency.h"
#include "daterange.h"
#include "itemprocessor.h"

class RoutineAllocator : public ItemProcessor
{
public:
   void finish() ;
    void processItem(LedgerAccount const& account) ;
    void processItem(LedgerBudget const& budget) ;
    void processItem(LedgerBudgetGoalEntry const& budget) ;
    void processItem(LedgerBudgetIncomeEntry const& budget) ;
    void processItem(LedgerBudgetReserveAmountEntry const& budget) ;
    void processItem(LedgerBudgetReservePercentEntry const& budget) ;
    void processItem(LedgerBudgetRoutineEntry const& budget) ;
    void processItem(LedgerComment const& comment) ;
    void processItem(LedgerTransaction const& transaction) ;
    void start() ;

//   Currency allocate(Currency available);
//   Currency allocate(DateRange const& period, Currency available);
//   void deallocate(QString const& category);
//   bool isUnderfunded() const;
//   void spend(QDate const& date, QString const& category,
//              Currency const& amount);

private:
//   Currency m_allocation;
//   QMap<QDate, QPair<QString, Currency>> m_history;
//   QHash<QString, Currency> m_historyTotals;
QDate m_startDate;
//    DateRange m_historyPeriod;
QSet<QString> m_categories;
Currency m_totalSpent;
QDate m_endDate;
};

#endif
