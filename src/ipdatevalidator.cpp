#include "ipdatevalidator.h"

#include <sstream>
#include "cashpiles.h"
#include "ledger.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgercomment.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"
#include "ledgerwarning.h"

using std::make_shared;

IPDateValidator::IPDateValidator(Ledger& ledger) :
   m_ledger{ledger}
{
}

void IPDateValidator::processItem(LedgerAccount const& account)
{
   processDate(account.date(), account.id);
}

bool IPDateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.id);
   return false;
}

bool IPDateValidator::processItem(LedgerReserve const& reserve)
{
   processDate(reserve.date(), reserve.id);
   return false;
}

void IPDateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.id);
}

void IPDateValidator::processDate(Date const& date, int id)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      auto warning = make_shared<LedgerWarning>();
      warning->setMessage("Date '" + date.toString() + "' out of order, " +
                          "other problems expected", id);
      m_ledger.appendAfterCurrent(warning);
   }
   else
   {
      m_latestDate = date;
   }
}
