#include "datevalidator.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

void DateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void DateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.fileName(), budget.lineNum());
}

void DateValidator::processItem(LedgerReserve const& reserve)
{
   processDate(reserve.date(), reserve.fileName(), reserve.lineNum());
}

void DateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void DateValidator::processDate(QDate const& date, QString const& fileName,
                                uint lineNum)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      die(fileName, lineNum,
          QString("Date %1 out of order")
          .arg(date.toString()));
   }
   else
   {
      m_latestDate = date;
   }
}
