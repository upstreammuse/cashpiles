#include "budgetbalancer.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerallocation.h"
#include "ledgerbudget.h"
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
      m_categories["  Available  "] -= it.value();
      if (m_categories["  Available  "].isNegative())
      {
         std::cerr << "Available funds are negative, file '"
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

void BudgetBalancer::processItem(LedgerBudget const& budget)
{
   m_budgetDate = budget.date();
   m_budgetInterval = budget.interval();

   // remove categories that are not in this budget command, and allocate their
   // funds to the available category
   Currency available;
   QSet<QString> categories = budget.categories();
   for (auto it = m_categories.begin(); it != m_categories.end(); /*inside*/)
   {
      if (!categories.contains(it.key()))
      {
         available += it.value();
         it = m_categories.erase(it);
      }
      else
      {
         ++it;
      }
   }
   m_categories["  Available  "] += available;

   // populate the category list with everything in the current budget
   foreach (QString const& category, categories)
   {
      m_categories[category];
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
               m_totals[entry.category()] += entry.amount();
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

   // find the dates of the current budget period
   QDate startDate;
   QDate endDate = m_budgetDate.addDays(-1);
   do {
      startDate = endDate.addDays(1);
      endDate = (startDate + m_budgetInterval).addDays(-1);
   } while (endDate < QDate::currentDate());
   std::cout << "current budget period begins "
             << qPrintable(startDate.toString()) << " and ends "
             << qPrintable(endDate.toString()) << std::endl;

   // find the number of days during the budget periods before this one
   qint64 priorDays = m_budgetDate.daysTo(startDate);
   std::cout << "averaging over " << priorDays << " days" << std::endl;
   for (auto it = m_totals.cbegin(); it != m_totals.cend(); ++it)
   {
      if (m_categories.contains(it.key()))
      {
         // TODO this seems to be giving bogus answers
         std::cout << "  " << qPrintable(it.key()) << " averages "
                   << qPrintable((it.value() / priorDays).amountA.toString())
                   << " per day" << std::endl;
      }
   }
}
