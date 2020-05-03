#include "ipaccountbalancer.h"

#include <iostream>
#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"
#include "texttable.h"

IPAccountBalancer::IPAccountBalancer(Date const& today) :
   m_today(today)
{
}

void IPAccountBalancer::finish()
{
   std::cout << "Account balance date: " << m_today.toString() << std::endl;

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
      if (it->second.onBudget &&
          (!it->second.isClosed || !it->second.cleared.isZero() ||
           !it->second.balance.isZero() || !it->second.future.isZero()))
      {
         std::stringstream ss;
         ss << it->first << "  ";
         table.appendColumn(0, ss.str());
         table.appendColumn(1, it->second.cleared.toString() + "  ");
         table.appendColumn(2, it->second.balance.toString() + "  ");
         table.appendColumn(3, it->second.future.toString());
         totalOnCleared += it->second.cleared;
         totalOn += it->second.balance;
         totalOnFuture += it->second.future;
      }
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(1, totalOnCleared.toString() + "  ");
   table.appendColumn(2, totalOn.toString() + "  ");
   table.appendColumn(3, totalOnFuture.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(std::cout);
   std::cout << std::endl;

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
      if (!it->second.onBudget &&
          (!it->second.isClosed || !it->second.cleared.isZero() ||
           !it->second.balance.isZero() || !it->second.future.isZero()))
      {
         std::stringstream ss;
         ss << it->first << "  ";
         table.appendColumn(0, ss.str());
         table.appendColumn(1, it->second.cleared.toString() + "  ");
         table.appendColumn(2, it->second.balance.toString() + "  ");
         table.appendColumn(3, it->second.future.toString());
         totalOffCleared += it->second.cleared;
         totalOff += it->second.balance;
         totalOffFuture += it->second.future;
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
   table.print(std::cout);
   std::cout << std::endl;

   std::cout << "Available for budgeting: " << totalOn.toString() << std::endl
             << std::endl;
}

void IPAccountBalancer::processItem(LedgerAccount const& account_)
{
   auto name = account_.name();
   auto& account = m_accounts[name];
   auto mode = account_.mode();

   switch (mode)
   {
      case LedgerAccount::Mode::CLOSED:
         if (account.isClosed)
         {
            std::stringstream ss;
            ss << "Cannot close account '" << name << "' that was not open";
            warn(account_.fileName(), account_.lineNum(), ss.str());
         }
         else if (!account.future.isZero())
         {
            std::stringstream ss;
            ss << "Cannot close account '" << name << "' with non-zero balance "
               << account.future.toString();
            warn(account_.fileName(), account_.lineNum(), ss.str());
         }
         else
         {
            account.isClosed = true;
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
      case LedgerAccount::Mode::ON_BUDGET:
         if (!account.isClosed)
         {
            std::stringstream ss;
            ss << "Cannot open account '" << name << "' that was already open";
            warn(account_.fileName(), account_.lineNum(), ss.str());
         }
         else
         {
            account.hasPending = false;
            account.isClosed = false;
            account.onBudget = (mode == LedgerAccount::Mode::ON_BUDGET);
         }
         break;
   }
}

// TODO
#if 0
void IPAccountBalancer::processItem(LedgerBalance const& balance)
{
   if (balance.amount() != m_accounts[balance.account()].future)
   {
      std::stringstream ss;
      ss << "Account '" << balance.account() << "' stated balance "
         << balance.amount().toString() << " does not match calculated "
         << "balance "
         << m_accounts[balance.account()].future.toString();
      warn(balance.fileName(), balance.lineNum(), ss.str());
   }
   if (m_accounts[balance.account()].hasPending)
   {
      warn(balance.fileName(), balance.lineNum(),
           "Pending transactions included in balance statement");
      m_accounts[balance.account()].hasPending = false;
   }
}
#endif

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
         std::stringstream ss;
         ss << "Account '" << transaction.account() << "' stated balance "
            << transaction.balance().toString() << " does not match calculated "
            << "balance "
            << m_accounts[transaction.account()].balance.toString();
         warn(transaction.fileName(), transaction.lineNum(), ss.str());
      }
      if (m_accounts[transaction.account()].hasPending)
      {
         warn(transaction.fileName(), transaction.lineNum(),
              "Pending transactions included in balance statement");
         m_accounts[transaction.account()].hasPending = false;
      }
   }
}

bool IPAccountBalancer::processItem(LedgerTransactionV2 const& transaction)
{
   m_workingDate = transaction.date();
   m_workingStatus = transaction.status();
   return true;
}

void IPAccountBalancer::processItem(
      LedgerTransactionV2AccountEntry const& entry)
{
   auto& account = m_accounts[entry.account()];
   auto amount = entry.amount().first;

   account.future += amount;
   if (m_workingDate <= m_today)
   {
      account.balance += amount;
      switch (m_workingStatus)
      {
         case LedgerTransactionV2::Status::CLEARED:
         case LedgerTransactionV2::Status::DISPUTED:
            account.cleared += amount;
            break;
         case LedgerTransactionV2::Status::PENDING:
            account.hasPending = true;
            break;
      }
   }
}

void IPAccountBalancer::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto& account = m_accounts[entry.trackingAccount().first];
   auto amount = entry.amount().first;

   account.future -= amount;
   if (m_workingDate <= m_today)
   {
      account.balance -= amount;
      switch (m_workingStatus)
      {
         case LedgerTransactionV2::Status::CLEARED:
         case LedgerTransactionV2::Status::DISPUTED:
            account.cleared -= amount;
            break;
         case LedgerTransactionV2::Status::PENDING:
            account.hasPending = true;
            break;
      }
   }
}

void IPAccountBalancer::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto& account = m_accounts[entry.trackingAccount().first];
   auto amount = entry.amount().first;

   account.future -= amount;
   if (m_workingDate <= m_today)
   {
      account.balance -= amount;
      switch (m_workingStatus)
      {
         case LedgerTransactionV2::Status::CLEARED:
         case LedgerTransactionV2::Status::DISPUTED:
            account.cleared -= amount;
            break;
         case LedgerTransactionV2::Status::PENDING:
            account.hasPending = true;
            break;
      }
   }
}
