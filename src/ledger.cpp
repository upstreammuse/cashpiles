#include "ledger.h"

#include "itemprocessor.h"

#if 0
do not allow uncleared before a balance assertion

compact xacts only have space for one note
- if LT has 1 payee, and either master or payee note (not both)
--- then can render as compact xact
- else > 1 note or > 1 payee requires full format

categories must already exist
#endif

Ledger::Ledger(QObject *parent) :
   QObject(parent)
{
}

void Ledger::addProcessor(ItemProcessor* processor)
{
   processor->setParent(this);
   m_processors.append(processor);
}

void Ledger::processItem(LedgerItem* item)
{
   foreach (ItemProcessor* processor, m_processors)
   {
      item->processItem(processor);
   }
   delete item;
}

void Ledger::start()
{
   emit started();
}

void Ledger::stop()
{
   foreach (ItemProcessor* processor, m_processors)
   {
      processor->stop();
   }
   emit finished();
}

#if 0
void Ledger::allocateBudget(LedgerBudgetAllocation const& allocation)
{
   processLedgerCommand(&allocation);
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

void Ledger::transact(Transaction const& transaction)
{
   processLedgerCommand(&transaction);
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
#endif
