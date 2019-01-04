#include "datevalidator.h"

#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgercomment.h"
#include "kernel/ledgertransaction.h"

DateValidator::DateValidator(QObject* parent) :
   ItemProcessor(parent)
{
}

void DateValidator::processItem(LedgerAccountCommand const& account)
{
   processDate(account.date(), account.fileName(), account.lineNum());
}

void DateValidator::processItem(LedgerBudget const& budget)
{
   processDate(budget.date(), budget.fileName(), budget.lineNum());
}

void DateValidator::processItem(LedgerComment const&)
{
}

void DateValidator::processItem(LedgerTransaction const& transaction)
{
   processDate(transaction.date(), transaction.fileName(),
               transaction.lineNum());
}

void DateValidator::stop()
{
}

void DateValidator::processDate(QDate const& date, QString const& fileName,
                                int lineNum)
{
   if (!m_latestDate.isNull() && date < m_latestDate)
   {
      emit message(LedgerComment(fileName, lineNum),
                   QString("Date %1 out of order")
                   .arg(date.toString()));
   }
   else
   {
      m_latestDate = date;
   }
}
