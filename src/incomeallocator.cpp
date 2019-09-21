#include "incomeallocator.h"

#include <QDebug>
#include "ledgerbudgetentry.h"
#include "ledgertransaction.h"

void IncomeAllocator::finish()
{
   qDebug() << "Earned" << m_income.toString();
   qDebug() << "reserved" << m_reserved.toString();
   qDebug() << "spent" << m_spent.toString();
   qDebug() << "remaining available" << (m_income - m_reserved).toString();
   qDebug() << "remaining reserved" << (m_reserved + m_spent).toString();
}

void IncomeAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   m_incomeCategories.insert(budget.name());
   qDebug() << budget.name() << "is income category";
}

void IncomeAllocator::processItem(LedgerBudgetReservePercentEntry const &budget)
{
   m_reserveCategories[budget.name()] = budget.percentage() / 100.0;
   qDebug() << budget.name() << "is reserve% category";
}

void IncomeAllocator::processItem(LedgerTransaction const& transaction)
{
   foreach (LedgerTransactionEntry entry, transaction.entries())
   {
      if (entry.hasCategory() && m_incomeCategories.contains(entry.category()))
      {
         m_income += entry.amount();
         foreach (double percentage, m_reserveCategories)
         {
            m_reserved += entry.amount() * percentage;
         }
      }
      else if (entry.hasCategory() && m_reserveCategories.contains(entry.category()))
      {
         m_spent += entry.amount();
      }
   }
}
