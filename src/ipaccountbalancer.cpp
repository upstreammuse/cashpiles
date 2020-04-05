#include "ipaccountbalancer.h"

#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "texttable.h"

IPAccountBalancer::IPAccountBalancer(Date const& today) :
   m_today(today.toQDate())
{
}

void IPAccountBalancer::finish()
{
   QTextStream out(stdout);
   out << "Account balance date: " << m_today.toString() << endl;

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
      if (it->onBudget &&
          (!it->isClosed || !it->cleared.isZero() || !it->balance.isZero() ||
           !it->future.isZero()))
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
      if (!it->onBudget &&
          (!it->isClosed || !it->cleared.isZero() || !it->balance.isZero() ||
           !it->future.isZero()))
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

void IPAccountBalancer::processItem(LedgerAccount const& account)
{
   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         if (m_accounts[account.name()].isClosed)
         {
            warn(account.fileName().toStdString(), account.lineNum(),
                 QString("Cannot close account '%1' that was not open")
                 .arg(account.name()).toStdString());
         }
         else if (!m_accounts[account.name()].future.isZero())
         {
            warn(account.fileName().toStdString(), account.lineNum(),
                 QString("Cannot close account '%1' with non-zero balance %2")
                 .arg(account.name())
                 .arg(m_accounts[account.name()].balance.toString()).toStdString());
         }
         else
         {
            m_accounts[account.name()].isClosed = true;
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
      case LedgerAccount::Mode::ON_BUDGET:
         if (!m_accounts[account.name()].isClosed)
         {
            warn(account.fileName().toStdString(), account.lineNum(),
                 QString("Cannot open account '%1' that was already open")
                 .arg(account.name()).toStdString());
         }
         else
         {
            m_accounts[account.name()].hasPending = false;
            m_accounts[account.name()].isClosed = false;
            m_accounts[account.name()].onBudget =
                  (account.mode() == LedgerAccount::Mode::ON_BUDGET);
         }
         break;
   }
}

void IPAccountBalancer::processItem(LedgerTransaction const& transaction)
{
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
            m_accounts[transaction.account()].hasPending = true;
            break;
      }
   }

   if (transaction.hasBalance())
   {
      if (transaction.balance() != m_accounts[transaction.account()].future)
      {
         warn(transaction.fileName().toStdString(), transaction.lineNum(),
              QString("Account '%1' stated balance %2 does not match "
                      "calculated balance %3")
              .arg(transaction.account())
              .arg(transaction.balance().toString())
              .arg(m_accounts[transaction.account()].balance.toString()).toStdString());
      }
      if (m_accounts[transaction.account()].hasPending)
      {
         warn(transaction.fileName().toStdString(), transaction.lineNum(),
              "Pending transactions included in balance statement");
         m_accounts[transaction.account()].hasPending = false;
      }
   }
}
