#include "ipdatevalidator.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"

void IPDateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void IPDateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.fileName(), budget.lineNum());
}

void IPDateValidator::processItem(LedgerReserve const& reserve)
{
   processDate(reserve.date(), reserve.fileName(), reserve.lineNum());
}

void IPDateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void IPDateValidator::processDate(QDate const& date, QString const& fileName,
                                uint lineNum)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      die(fileName.toStdString(), lineNum,
          QString("Date %1 out of order")
          .arg(date.toString()).toStdString());
   }
   else
   {
      m_latestDate = date;
   }
}
