#include "ipdatevalidator.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

void IPDateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

bool IPDateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.fileName(), budget.lineNum());
   return false;
}

void IPDateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void IPDateValidator::processDate(Date const& date, std::string const& fileName,
                                  size_t lineNum)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      std::stringstream ss;
      ss << "Date " << date.toString() << " out of order";
      die(fileName, lineNum, ss.str());
   }
   else
   {
      m_latestDate = date;
   }
}
