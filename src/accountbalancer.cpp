#include "accountbalancer.h"

#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "texttable.h"

AccountBalancer::AccountBalancer(QDate const& today) :
   m_today(today)
{
}

void AccountBalancer::finish()
{
   checkTransfers(m_lastDate.addDays(1));
   QTextStream out(stdout);

   out << "Account balance date: " << m_today.toString() << endl;
   out << "Future balance date: " << m_lastDate.toString() << endl << endl;

   TextTable table;
   table.appendColumn(0, "== ON-BUDGET ACCOUNT ==  ");
   table.appendColumn(1, "== CLEARED ==  ");
   table.appendColumn(2, "== BALANCE ==  ");
   table.appendColumn(3, "== FUTURE ==");
   Currency totalOn;
   Currency totalOnCleared;
   Currency totalOnFuture;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (it->onBudget)
      {
         table.appendColumn(0, it.key() + "  ");
         table.appendColumn(1, it->cleared.toString() + "  ");
         table.appendColumn(2, it->balance.toString() + "  ");
         table.appendColumn(3, it->future.toString());
         totalOnCleared += it->cleared;
         totalOn += it->balance;
         totalOnFuture += it->future;
      }
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(1, totalOnCleared.toString() + "  ");
   table.appendColumn(2, totalOn.toString() + "  ");
   table.appendColumn(3, totalOnFuture.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;

   table.clear();
   table.appendColumn(0, "== OFF-BUDGET ACCOUNT ==  ");
   table.appendColumn(1, "== CLEARED ==  ");
   table.appendColumn(2, "== BALANCE ==  ");
   table.appendColumn(3, "== FUTURE ==");
   Currency totalOff;
   Currency totalOffCleared;
   Currency totalOffFuture;
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      if (!it->onBudget)
      {
         table.appendColumn(0, it.key() + "  ");
         table.appendColumn(1, it->cleared.toString() + "  ");
         table.appendColumn(2, it->balance.toString() + "  ");
         table.appendColumn(3, it->future.toString());
         totalOffCleared += it->cleared;
         totalOff += it->balance;
         totalOffFuture += it->future;
      }
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(1, totalOffCleared.toString() + "  ");
   table.appendColumn(2, totalOff.toString() + "  ");
   table.appendColumn(3, totalOffFuture.toString());
   table.appendColumn(0, "== NET WORTH ==  ");
   table.appendColumn(1, (totalOnCleared + totalOffCleared).toString() + "  ");
   table.appendColumn(2, (totalOn + totalOff).toString() + "  ");
   table.appendColumn(3, (totalOnFuture + totalOffFuture).toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;

   out << "Available for budgeting: " << totalOn.toString() << endl << endl;
}

void AccountBalancer::processItem(LedgerAccount const& account)
{
   checkTransfers(account.date());

   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         if (m_accounts.contains(account.name()))
         {
            if (!m_accounts[account.name()].balance.isZero())
            {
               warn(account.fileName(), account.lineNum(),
                    QString("Cannot close account '%1' with non-zero balance "
                            "%2")
                    .arg(account.name())
                    .arg(m_accounts[account.name()].balance.toString()));
            }
            else
            {
               m_accounts.remove(account.name());
            }
         }
         else
         {
            warn(account.fileName(), account.lineNum(),
                 QString("Cannot close account '%1' that was not open")
                 .arg(account.name()));
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onBudget = false;
         }
         else
         {
            warn(account.fileName(), account.lineNum(),
                 QString("Cannot open account '%1' that was already open")
                 .arg(account.name()));
         }
         break;
      case LedgerAccount::Mode::ON_BUDGET:
         if (!m_accounts.contains(account.name()))
         {
            m_accounts[account.name()].onBudget = true;
         }
         else
         {
            warn(account.fileName(), account.lineNum(),
                 QString("Cannot open account '%1' that was already open")
                 .arg(account.name()));
         }
         break;
   }
}

void AccountBalancer::processItem(LedgerBudget const& budget)
{
   checkTransfers(budget.date());
}

void AccountBalancer::processItem(LedgerTransaction const& transaction)
{
   checkTransfers(transaction.date());

   if (!m_accounts.contains(transaction.account()))
   {
      warn(transaction.fileName(), transaction.lineNum(),
           QString("Automatically opening on-budget account '%1'")
           .arg(transaction.account()));
      m_accounts[transaction.account()].onBudget = true;
   }

   m_accounts[transaction.account()].future += transaction.amount();
   if (transaction.date() <= m_today)
   {
      m_accounts[transaction.account()].balance += transaction.amount();
      switch (transaction.status())
      {
         case LedgerTransaction::Status::CLEARED:
         case LedgerTransaction::Status::DISPUTED:
            m_accounts[transaction.account()].cleared += transaction.amount();
            break;
         case LedgerTransaction::Status::PENDING:
            break;
      }
   }

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         if (!m_accounts.contains(entry.payee()))
         {
            warn(transaction.fileName(), transaction.lineNum(),
                 QString("Automatically opening on-budget account '%1'")
                 .arg(entry.payee()));
            m_accounts[entry.payee()].onBudget = true;
         }

         if (m_accounts[transaction.account()].onBudget &&
             m_accounts[entry.payee()].onBudget &&
             entry.hasCategory())
         {
            // TODO there needs to be a validation layer between reading the file (syntax validation) and this sort of semantic validation
            //   the semantic validation is there because a human made the input file, and should still be used as part of reading a gui-saved file
            //   but the semantic validation isn't related to actual processing errors, so it could be in a layer before the real processing starts
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for transfer between on-budget "
                        "accounts '%1' and '%2'")
                .arg(transaction.account())
                .arg(entry.payee()));
         }
         else if (m_accounts[transaction.account()].onBudget &&
                  !m_accounts[entry.payee()].onBudget &&
                  !entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Missing budget category for transfer between "
                        "on-budget account '%1' and off-budget account '%2'")
                .arg(transaction.account())
                .arg(entry.payee()));
         }
         else if (!m_accounts[transaction.account()].onBudget &&
                  entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for off-budget account '%1'")
                .arg(transaction.account()));
         }

         m_transfers[transaction.account()][entry.payee()] += entry.amount();
      }
      else
      {
         if (m_accounts[transaction.account()].onBudget && !entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Missing budget category for on-budget account '%1'")
                .arg(transaction.account()));
         }
         else if (!m_accounts[transaction.account()].onBudget &&
                  entry.hasCategory())
         {
            die(transaction.fileName(), transaction.lineNum(),
                QString("Budget category set for off-budget account '%1'")
                .arg(transaction.account()));
         }
      }
   }

   if (transaction.hasBalance())
   {
      if (transaction.balance() != m_accounts[transaction.account()].balance)
      {
         die(transaction.fileName(), transaction.lineNum(),
             QString("Account '%1' stated balance %2 does not match calculated "
                     "balance %3")
             .arg(transaction.account())
             .arg(transaction.balance().toString())
             .arg(m_accounts[transaction.account()].balance.toString()));
      }
      if (m_hasPending)
      {
         die(transaction.fileName(), transaction.lineNum(),
             "Pending transactions included in balance statement");
      }
   }
}

void AccountBalancer::checkTransfers(QDate const& date)
{
   if (date != m_lastDate)
   {
      for (auto it = m_accounts.cbegin(); it != m_accounts.cend(); ++it)
      {
         for (auto it2 = m_accounts.cbegin(); it2 != m_accounts.cend(); ++it2)
         {
            Currency balance = m_transfers[it.key()][it2.key()] +
                               m_transfers[it2.key()][it.key()];
            if (!balance.isZero())
            {
               if (balance.isNegative())
               {
                  balance = -balance;
               }
               die(QString("Transfers between '%1' and '%2' do not match as of "
                           "%3.  Mismatch of %4")
                   .arg(it.key())
                   .arg(it2.key())
                   .arg(date.toString())
                   .arg(balance.toString()));
            }
         }
      }
      m_lastDate = date;
   }
}
