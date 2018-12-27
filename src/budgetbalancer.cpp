#include "budgetbalancer.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerallocation.h"
#include "ledgertransaction.h"

BudgetBalancer::BudgetBalancer(QObject* parent) :
   ItemProcessor(parent)
{
}

void BudgetBalancer::processItem(LedgerAccountCommand const& account)
{
   switch (account.mode())
   {
      case LedgerAccountCommand::Mode::CLOSED:
         m_accounts.remove(account.account());
         break;
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         m_accounts[account.account()] = false;
         break;
      case LedgerAccountCommand::Mode::ON_BUDGET:
         m_accounts[account.account()] = true;
         break;
   }
}

void BudgetBalancer::processItem(LedgerAllocation const& allocation)
{
   QMap<QString, Currency> allocations = allocation.allocations();
   for (auto it = allocations.cbegin(); it != allocations.cend(); ++it)
   {
      if (!m_categories.contains(it.key()))
      {
         std::cerr << "Budget category '" << qPrintable(it.key())
                   << "' does not exist, file '"
                   << qPrintable(allocation.fileName()) << "', line "
                   << allocation.lineNum() << std::endl;
      }
      m_categories[it.key()] += it.value();
      if (m_categories[it.key()].isNegative())
      {
         std::cerr << "Budget category '" << qPrintable(it.key())
                   << "' is negative, file '"
                   << qPrintable(allocation.fileName()) << "', line "
                   << allocation.lineNum() << std::endl;
      }
   }
}

void BudgetBalancer::processItem(LedgerComment const&)
{
}

void BudgetBalancer::processItem(LedgerTransaction const& transaction)
{
   if (!m_accounts.contains(transaction.account()))
   {
      m_accounts[transaction.account()] = true;
   }

   // TODO clean up the logic below.  I have this suspicion there are holes in
   // the if-else branches that are not obvious

   if (m_accounts[transaction.account()])
   {
      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         if (entry.transfer() && !m_accounts.contains(entry.payee()))
         {
            m_accounts[entry.payee()] = true;
         }

         if (entry.hasCategory())
         {
            if (entry.transfer() && m_accounts[entry.payee()])
            {
               std::cerr << "Transfers between on-budget accounts '"
                         << qPrintable(transaction.account()) << "' and '"
                         << qPrintable(entry.payee())
                         << "' cannot have a category, line "
                         << transaction.lineNum() << std::endl;
            }
            else
            {
               if (!m_categories.contains(entry.category()))
               {
                  std::cerr << "Budget category '"
                            << qPrintable(entry.category())
                            << "' does not exist, file '"
                            << qPrintable(transaction.fileName()) << "', line "
                            << transaction.lineNum() << std::endl;
               }
               m_categories[entry.category()] += entry.amount();
               if (m_categories[entry.category()].isNegative())
               {
                  std::cerr << "Budget category '"
                            << qPrintable(entry.category())
                            << "' is underfunded.  Balance is "
                            << qPrintable(m_categories[entry.category()].toString())
                        << ".  Line " << transaction.lineNum() << std::endl;
               }
            }
         }
         else
         {
            if (entry.transfer() && m_accounts[entry.payee()])
            {
               // nothing to do here for on-budget transfers
            }
            else
            {
               std::cerr << "Transaction missing a category for payee '"
                         << qPrintable(entry.payee()) << "', line "
                         << transaction.lineNum() << std::endl;
            }
         }
      }
   }
   else
   {
      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         if (entry.hasCategory())
         {
            std::cerr << "Transactions for off-budget account '"
                      << qPrintable(transaction.account())
                      << "' cannot have categories, line "
                      << transaction.lineNum() << std::endl;
         }
      }
   }
}

void BudgetBalancer::stop()
{
   std::cout << "Current Budget" << std::endl;
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      std::cout << "   " << qPrintable(it.key()) << "   "
                << qPrintable(it.value().toString()) << std::endl;
   }
}
