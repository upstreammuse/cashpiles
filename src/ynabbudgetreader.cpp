#include "ynabbudgetreader.h"

#include <QHash>
#include "ledgerbudgetallocation.h"

YnabBudgetReader::YnabBudgetReader(QObject* parent) :
   QObject(parent)
{
}

void YnabBudgetReader::processRecord(QHash<QString, QString> const& record,
                                     QString const& fileName, int lineNum)
{
   if (record["Budgeted"].isEmpty())
   {
      qWarning("Budget entry is missing budgeted amount, file %s, line %d",
               qPrintable(fileName), lineNum);
      return;
   }
   if (record["Category"].isEmpty())
   {
      qWarning("Budget entry is missing category, file %s, line %d",
               qPrintable(fileName),  lineNum);
      return;
   }
   if (record["Month"].isEmpty())
   {
      qWarning("Budget entry is missing month and year, file %s, line %d",
               qPrintable(fileName), lineNum);
      return;
   }

   QDate date(QDate::fromString(record["Month"], "MMM yyyy"));
   if (!m_allocation)
   {
      m_allocation = new LedgerBudgetAllocation(fileName, lineNum);
      m_allocation->setDate(date);
   }
   else if (m_allocation->date() != date)
   {
      if (!m_allocation->allocations().empty())
      {
         emit item(m_allocation->date(), m_allocation);
         m_allocation = new LedgerBudgetAllocation(fileName, lineNum);
      }
      m_allocation->setDate(QDate::fromString(record["Month"], "MMM yyyy"));
   }

   Currency amount(record["Budgeted"], lineNum);
   if (!amount.isZero())
   {
      if (!record["Category Group"].isEmpty())
      {
         m_allocation->appendAllocation(
                  record["Category Group"] + ":" + record["Category"],
               amount);
      }
      else
      {
         m_allocation->appendAllocation(record["Category"],
               amount);
      }
   }
}

void YnabBudgetReader::stop()
{
   if (m_allocation)
   {
      if (!m_allocation->allocations().isEmpty())
      {
         emit item(m_allocation->date(), m_allocation);
      }
      else
      {
         delete m_allocation;
      }
      m_allocation = nullptr;
   }
   emit finished();
}
