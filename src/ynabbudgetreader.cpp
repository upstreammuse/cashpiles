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

   LedgerBudgetAllocation* allocation =
         new LedgerBudgetAllocation(fileName, lineNum);
   allocation->setDate(QDate::fromString(record["Month"], "MMM yyyy"));
   QString amountStr(record["Budgeted"]);
   int amount = amountStr.replace('$', "").replace(',', "").replace('.', "")
                .toInt();
   if (!record["Category Group"].isEmpty())
   {
      allocation->appendAllocation(
               record["Category Group"] + ":" + record["Category"], amount);
   }
   else
   {
      allocation->appendAllocation(record["Category"], amount);
   }
   emit item(allocation);
}
