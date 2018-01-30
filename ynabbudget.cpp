#include "ynabbudget.h"

#include <QHash>
#include "transaction.h"

YnabBudget::YnabBudget(QObject* parent) :
   QObject(parent)
{
}

void YnabBudget::appendRecord(QHash<QString, QString> const& record)
{
   // TODO columns might not be needed at all
   for (QHash<QString, QString>::const_iterator it(record.begin());
        it != record.end(); ++it)
   {
      if (it.key() == "Budgeted")
      {
         QString fieldCopy(it.value());
         m_budgetColumn.append(
                  fieldCopy.replace('$', "").replace(',', "").replace('.', "")
                  .toInt());
      }
      else if (it.key() == "Category")
      {
         m_categoryColumn.append(it.value());
      }
      else if (it.key() == "Category Group")
      {
         m_groupColumn.append(it.value());
      }
      else if (it.key() == "Month")
      {
         m_dateColumn.append(QDate::fromString(it.value(), "MMM yyyy"));
      }
   }
   Q_ASSERT(m_budgetColumn.size() == m_categoryColumn.size());
   Q_ASSERT(m_budgetColumn.size() == m_groupColumn.size());
   Q_ASSERT(m_dateColumn.size() == m_dateColumn.size());

   Transaction t;
   t.setAccount("");
   t.setDate(m_dateColumn.last());
   t.appendSplit(TransactionSplit("", m_categoryColumn.last(), "",
                                  m_budgetColumn.last()));
   t.appendSplit(TransactionSplit("", "To be Budgeted", "",
                                  -m_budgetColumn.last()));
   emit transaction(t);
}
