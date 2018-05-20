#include "ledger.h"

#if 0
notes for implementation

do not allow uncleared before a balance assertion

compact xacts only have space for one note
- if LT has 1 payee, and either master or payee note (not both)
--- then can render as compact xact
- else > 1 note or > 1 payee requires full format

categories, accounts must already exist

seqnum is strict ordering with no missing numbers

dates must always be >= to previous date
#endif

void Ledger::allocateBudget(LedgerBudgetAllocation const& allocation)
{
   validateDate(allocation.date(), allocation.lineNum());
   QMap<QString, int> allocations(allocation.allocations());
   for (QMap<QString, int>::iterator it(allocations.begin());
        it != allocations.end(); ++it)
   {
      if (!m_categories.contains(it.key()))
      {
         m_categories[it.key()] = 0;
      }
      m_categories[it.key()] += it.value();
      qDebug("Budget category '%s' has balance of %d", qPrintable(it.key()),
             it.value());
      if (m_categories[it.key()] < 0)
      {
         qWarning("Budget allocation on line %d, category '%s' has a negative "
                  "balance", allocation.lineNum(), qPrintable(it.key()));
         exit(EXIT_FAILURE);
      }
   }
}

void Ledger::changeAccount(LedgerAccountCommand const& command)
{
   validateDate(command.date(), command.lineNum());
   switch (command.mode())
   {
      case LedgerAccountCommand::Mode::ON_BUDGET:
         if (m_accounts.contains(command.account()))
         {
            qWarning("Account command on line %d, attempted to open account "
                     "'%s' that was already open", command.lineNum(),
                     qPrintable(command.account()));
            exit(EXIT_FAILURE);
         }
         m_accounts[command.account()].setBudget(true);
         m_accounts[command.account()].setName(command.account());
         qDebug("Opened account '%s'", qPrintable(command.account()));
         break;
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         if (m_accounts.contains(command.account()))
         {
            qWarning("Account command on line %d, attempted to open account "
                     "'%s' that was already open", command.lineNum(),
                     qPrintable(command.account()));
            exit(EXIT_FAILURE);
         }
         m_accounts[command.account()].setName(command.account());
         qDebug("Opened account '%s'", qPrintable(command.account()));
         break;
      case LedgerAccountCommand::Mode::CLOSED:
         if (!m_accounts.contains(command.account()))
         {
            qWarning("Account command on line %d, attempted to close account "
                     "'%s' that was not open", command.lineNum(),
                     qPrintable(command.account()));
            exit(EXIT_FAILURE);
         }
         if (m_accounts[command.account()].balance() != 0)
         {
            qWarning("Account command on line %d, attempted to close account "
                     "'%s' with non-zero balance", command.lineNum(),
                     qPrintable(command.account()));
            exit(EXIT_FAILURE);
         }
         m_accounts.remove(command.account());
         qDebug("Closed account '%s'", qPrintable(command.account()));
   }
}

void Ledger::comment(LedgerComment const& comment)
{
   qDebug("Comment note '%s'", qPrintable(comment.note()));
}

void Ledger::transact(LedgerTransaction const& transaction)
{
   validateDate(transaction.date(), transaction.lineNum());
   if (!m_accounts.contains(transaction.account()))
   {
      qWarning("Account '%s' is not open, line %d",
               qPrintable(transaction.account()), transaction.lineNum());
      exit(EXIT_FAILURE);
   }
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         if (!m_accounts.contains(entry.payee()))
         {
            qWarning("Transfer account '%s' is not open, line %d",
                     qPrintable(entry.payee()), transaction.lineNum());
            exit(EXIT_FAILURE);
         }
         m_accounts[transaction.account()].addTransferTo(entry.payee(),
                                                         entry.amount());
         m_accounts[entry.payee()].addTransferFrom(transaction.account(),
                                                   entry.amount());
      }
      m_accounts[transaction.account()].addBalance(entry.amount());
   }
}

void Ledger::validateDate(QDate const& date, int line)
{
   if (m_latestDate.isValid() && date < m_latestDate)
   {
      qWarning("Date %s is out of order, line %d", qPrintable(date.toString()),
               line);
      exit(EXIT_FAILURE);
   }
   else
   {
      m_latestDate = date;
   }
}
