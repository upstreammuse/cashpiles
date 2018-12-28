#include "budgetbalancer.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"

BudgetBalancer::BudgetBalancer(QObject* parent) :
   ItemProcessor(parent)
{
}

void BudgetBalancer::processItem(LedgerAccountCommand const& account)
{
   advancePeriodToDate(account.date());

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

void BudgetBalancer::processItem(LedgerBudget const& budget)
{
   // TODO decide when or if to reset this
   if (m_firstDate.isNull())
   {
      m_firstDate = budget.date();
   }

   // if we already have a period defined, then run it out to the day before the
   // period we just got is going to start
   if (!m_periodEnd.isNull())
   {
      // if the given date is outside the next period, switch to the next period
      // and allocate funds for it.  keep going until the date is within the
      // next period
      while (m_periodEnd + m_periodLength < budget.date())
      {
         m_periodStart = m_periodEnd.addDays(1);
         m_periodEnd = (m_periodStart + m_periodLength).addDays(-1);
         allocateCategories();
      }

      // make a period that covers the dates in between the last full period and
      // the new one we are about to start, but only if we didn't coincide with
      // the start date of the old period cycle
      if (m_periodEnd.addDays(1) != budget.date())
      {
         m_periodStart = m_periodEnd.addDays(1);
         m_periodEnd = budget.date().addDays(-1);
         Q_ASSERT(m_periodStart <= m_periodEnd);
         allocateCategories();
      }
   }

   // remove reserves that are not in this budget command, and allocate their
   // funds to the available category
   m_categories = budget.categories();
   for (auto it = m_reserves.begin(); it != m_reserves.end(); /*inside*/)
   {
      if (!m_categories.contains(it.key()))
      {
         m_available += it.value();
         it = m_reserves.erase(it);
      }
      else
      {
         ++it;
      }
   }

   // reset the dates for the new period
   m_periodStart = budget.date();
   m_periodLength = budget.interval();
   m_periodEnd = (m_periodStart + m_periodLength).addDays(-1);
   allocateCategories();
}

void BudgetBalancer::processItem(LedgerComment const&)
{
}

void BudgetBalancer::processItem(LedgerTransaction const& transaction)
{
   advancePeriodToDate(transaction.date());

   // make the account on-budget if we haven't seen it yet
   if (!m_accounts.contains(transaction.account()))
   {
      m_accounts[transaction.account()] = true;
   }

   // off-budget account transactions are just checked to make sure they don't
   // have any categories
   if (!m_accounts[transaction.account()])
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
      return;
   }

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      // make the transfer account on-budget if we haven't seen it yet
      if (entry.transfer() && !m_accounts.contains(entry.payee()))
      {
         m_accounts[entry.payee()] = true;
      }

      // if it doesn't have a category, it has to be a transfer to another
      // on-budget account
      if (!entry.hasCategory())
      {
         if (!entry.transfer() || !m_accounts[entry.payee()])
         {
            std::cerr << "Transaction missing a category for payee '"
                      << qPrintable(entry.payee()) << "', line "
                      << transaction.lineNum() << std::endl;
         }
         continue;
      }

      // if it is a transfer to another on-budget account, it shouldn't have a
      // category
      if (entry.transfer() && m_accounts[entry.payee()])
      {
         std::cerr << "Transfers between on-budget accounts '"
                   << qPrintable(transaction.account()) << "' and '"
                   << qPrintable(entry.payee())
                   << "' cannot have a category, line "
                   << transaction.lineNum() << std::endl;
         continue;
      }

      if (!m_categories.contains(entry.category()))
      {
         std::cerr << "Unknown category '" << qPrintable(entry.category ())
                   << "', file '" << qPrintable(transaction.fileName())
                   << "', line " << transaction.lineNum() << std::endl;
         m_categories[entry.category()].type = BudgetCategory::Type::ROUTINE;
      }
      switch (m_categories[entry.category()].type)
      {
         case BudgetCategory::Type::GOAL:
            std::cerr << "I haven't figure this one out yet, TODO" << std::endl;
            break;
         case BudgetCategory::Type::INCOME:
            for (auto it = m_categories.cbegin(); it != m_categories.cend();
                 ++it)
            {
               if (it->type == BudgetCategory::Type::RESERVE_PERCENT)
               {
                  Currency amount = entry.amount().percentage(it->percentage);
                  m_reserves[entry.category()] += amount;
                  m_available -= amount;
                  if (m_reserves[entry.category()].isNegative())
                  {
                     std::cerr << "Reserve '" << qPrintable(entry.category())
                               << "' is underfunded, compensating."
                               << std::endl;
                     std::cerr << "  Amount: "
                               << qPrintable(m_reserves[entry.category()].toString())
                           << std::endl;
                     std::cerr << "  File: " << qPrintable(transaction.fileName())
                               << std::endl;
                     std::cerr << "  Line: " << transaction.lineNum() << std::endl;
                     m_available += m_reserves[entry.category()];
                     m_reserves[entry.category()].clear();
                  }
                  if (m_available.isNegative())
                  {
                     std::cerr << "Available funds are negative.  Trouble!"
                               << std::endl;
                  }
               }
            }
            break;
         case BudgetCategory::Type::ROUTINE:
            m_routineEscrow += entry.amount();
            m_routineTotal += entry.amount();
            if (m_routineEscrow.isNegative())
            {
               std::cerr << "Routine expenses are underfunded, compensating."
                         << std::endl;
               std::cerr << "  Amount: "
                         << qPrintable(m_routineEscrow.toString()) << std::endl;
               std::cerr << "  File: " << qPrintable(transaction.fileName())
                         << std::endl;
               std::cerr << "  Line: " << transaction.lineNum() << std::endl;
               m_available += m_routineEscrow;
               m_routineEscrow.clear();
            }
            if (m_available.isNegative())
            {
               std::cerr << "Available funds are negative.  Trouble!"
                         << std::endl;
            }
            break;

#if 0
            if (m_categories[entry.category()].type == BudgetCategory::Type::INCOME)
            {
               m_available += entry.amount();
               if (m_available.isNegative())
               {
                  std::cerr << "Available funds are negative, file '"
                            << qPrintable(transaction.fileName())
                            << "', line " << transaction.lineNum()
                            << std::endl;
               }
            }
            else
            {
               if (!m_categories.contains(entry.category()))
               {
                  std::cerr << "Budget category '"
                            << qPrintable(entry.category())
                            << "' does not exist, file '"
                            << qPrintable(transaction.fileName())
                            << "', line " << transaction.lineNum()
                            << std::endl;
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
#endif
      }
   }
}

// todo for calculating expenses based on past averages, require that there be more days of history than the longest repeat interval, to ensure that there is at least one instance over time for all repeats.
//   and if there is not enough history, then just look at the projected expenses instead
// projected expenses are:
//   all scheduled transactions for the current budget period
//   for any transaction that does not have an instance this period, find the next instance and divide it between the remaining periods

void BudgetBalancer::stop()
{
#if 0
   std::cout << "Current Budget" << std::endl;
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      std::cout << "   " << qPrintable(it.key()) << "   "
                << qPrintable(it.value().toString()) << std::endl;
   }

   // find the dates of the current budget period
   QDate startDate;
   QDate endDate = m_budgetDate.addDays(-1);
   do
   {
      startDate = endDate.addDays(1);
      endDate = (startDate + m_budgetInterval).addDays(-1);
   }
   while (endDate < QDate::currentDate());
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
         std::cout << "  " << qPrintable(it.key()) << " averages "
                   << qPrintable((it.value() / priorDays).amountA.toString())
                   << " per day" << std::endl;
      }
   }
#endif
}

void BudgetBalancer::advancePeriodToDate(QDate const& date)
{
   // TODO decide when (or if) to reset this
   if (m_firstDate.isNull())
   {
      m_firstDate = date;
   }

   // by default we just tick each day
   if (m_periodEnd.isNull())
   {
      m_periodEnd = date.addDays(-1);
      m_periodStart = m_periodEnd;
      m_periodLength = Interval(1, Interval::Period::DAYS);
   }

   // if the given date is outside the current period, switch to the next period
   // and allocate funds for it.  keep going until the date is within the
   // current period
   while (m_periodEnd < date)
   {
      m_periodStart = m_periodEnd.addDays(1);
      m_periodEnd = (m_periodStart + m_periodLength).addDays(-1);
      allocateCategories();
   }
}

void BudgetBalancer::allocateCategories()
{
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      Currency amount;
      switch (it->type)
      {
         case BudgetCategory::Type::GOAL:
            std::cerr << "TODO, I don't know the future to handle this yet" << std::endl;
            break;
         case BudgetCategory::Type::INCOME:
            break;
         case BudgetCategory::Type::RESERVE_AMOUNT:
            amount = (it->amount / it->interval.toApproximateDays()).amountA * (m_periodStart.daysTo(m_periodEnd) + 1);
            m_reserves[it.key()] += amount;
            m_available -= amount;
            break;
         case BudgetCategory::Type::RESERVE_PERCENT:
            break;
         case BudgetCategory::Type::ROUTINE:
            (m_routineTotal / priorDays()).amountA * (m_periodStart.daysTo(m_periodEnd) + 1);
            break;
      }
   }
}

int BudgetBalancer::priorDays()
{
   Q_ASSERT(!m_firstDate.isNull());
   Q_ASSERT(!m_periodStart.isNull());
   int result = m_firstDate.daysTo(m_periodStart);
   if (result < 1)
   {
      std::cerr << "No prior days to calculate with.  Expect a crash!" << std::endl;
      // TODO make this work properly
   }
   return result;
}
