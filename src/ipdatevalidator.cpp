#include "ipdatevalidator.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

using std::string;

IPDateValidator::IPDateValidator(string const& dateFormat) :
   m_dateFormat(dateFormat),
   // TODO this is a HACK
   m_latestDate(DateBuilder().month(1).day(1).year(1).toDate())
{
}

void IPDateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void IPDateValidator::processItem(LedgerAccountBalance const& balance)
{
   processDate(balance.date(), balance.fileName(), balance.lineNum());
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

bool IPDateValidator::processItem(LedgerTransactionV2 const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
   return false;
}

void IPDateValidator::processDate(Date const& date, std::string const& fileName,
                                  size_t lineNum)
{
   if (date < m_latestDate)
   {
      std::stringstream ss;
      ss << "Date " << date.toString(m_dateFormat) << " out of order";
      die(fileName, lineNum, ss.str());
   }
   else
   {
      m_latestDate = date;
   }
}
