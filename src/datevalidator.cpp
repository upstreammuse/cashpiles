#include "datevalidator.h"

#include <QDebug>
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

void DateValidator::finish()
{
}

void DateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void DateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.fileName(), budget.lineNum());
}

void DateValidator::processItem(LedgerBudgetGoalEntry const&)
{
}

void DateValidator::processItem(LedgerBudgetIncomeEntry const&)
{
}

void DateValidator::processItem(LedgerBudgetReserveAmountEntry const&)
{
}

void DateValidator::processItem(LedgerBudgetReservePercentEntry const&)
{
}

void DateValidator::processItem(LedgerBudgetRoutineEntry const&)
{
}

void DateValidator::processItem(LedgerComment const&)
{
}

void DateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void DateValidator::start()
{
}

void DateValidator::processDate(QDate const& date, QString const& fileName,
                                uint lineNum)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      qDebug() << QString("File '%1', Line %2: Date %3 out of order")
                  .arg(fileName).arg(lineNum).arg(date.toString());
   }
   else
   {
      m_latestDate = date;
   }
}
