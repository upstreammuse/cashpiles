#include "datevalidator.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerbudgetallocation.h"
#include "ledgertransaction.h"

DateValidator::DateValidator(QObject* parent) :
   ItemProcessor(parent)
{
}

void DateValidator::processItem(LedgerAccountCommand const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void DateValidator::processItem(LedgerBudgetAllocation const& allocation)
{
   processDate(allocation.date(), allocation.fileName(), allocation.lineNum());
}

void DateValidator::processItem(LedgerComment const&)
{
}

void DateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void DateValidator::processDate(QDate const& date, QString const& fileName,
                                int lineNum)
{
   if (m_latestDate.isValid() && date < m_latestDate)
   {
      std::cerr << "Date " << qPrintable(date.toString())
                << " out of order in file '" << qPrintable(fileName)
                << "', line " << lineNum << std::endl;
      exit(EXIT_FAILURE);
   }
   else
   {
      m_latestDate = date;
   }
}
