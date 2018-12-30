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
   // make sure we are in the current recording period
   advancePeriodToDate(account.date());

   // record the command
   m_recordedAccounts.insert(m_numRecords, account);
   ++m_numRecords;
}

void BudgetBalancer::processItem(LedgerBudget const& budget)
{
   // if there is a budget period, process recorded items until we are within
   // range of this budget command
   advancePeriodToDate(budget.date());

   // if the current period started before today, or if it already has recorded
   // commands, then end the period today and process what we have
   if (m_period.startDate() != budget.date() || m_numRecords != 0)
   {
      m_period = DateRange(m_period.startDate(), budget.date());
      allocateCategories();
      processRecords();
   }

   // remove reserves that are not in this budget command, and allocate their
   // funds to the available category
   m_categories = budget.categories();
   for (auto it = m_percentReserves.begin(); it != m_percentReserves.end();
        /*inside*/)
   {
      if (!m_categories.contains(it.key()))
      {
         m_available += it.value();
         it = m_percentReserves.erase(it);
      }
      else
      {
         ++it;
      }
   }
   for (auto it = m_periodReserves.begin(); it != m_periodReserves.end();
        /*inside*/)
   {
      if (!m_categories.contains(it.key()))
      {
         m_available += it->amount;
         it = m_periodReserves.erase(it);
      }
      else
      {
         ++it;
      }
   }

   // reset the dates for the new period
   m_period = DateRange(budget.date(), budget.interval());
}

void BudgetBalancer::processItem(LedgerComment const&)
{
}

void BudgetBalancer::processItem(LedgerTransaction const& transaction)
{
   // make sure we are in the current recording period
   advancePeriodToDate(transaction.date());

   // record the transaction
   m_recordedTransactions.insert(m_numRecords, transaction);
   ++m_numRecords;
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
   // if no budget is defined, instantiate a default 1-month budget cycle to
   // attempt to give useful info out of the box
   if (m_period.isNull())
   {
      m_period = DateRange(date, Interval(1, Interval::Period::MONTHS));
      m_priorDays = DateRange(date, date);
   }

   while (m_period.endDate() < date)
   {
      allocateCategories();
      processRecords();
      m_priorDays = DateRange(m_priorDays.startDate(), m_period.endDate());
      ++m_period;
   }
}

void BudgetBalancer::allocateCategories()
{
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      switch (it->type)
      {
         case LedgerBudget::Category::Type::GOAL:
            std::cerr << "TODO, I don't know the future to handle this yet" << std::endl;
            break;
         case LedgerBudget::Category::Type::INCOME:
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
         {
            Currency amount;
            DateRange& range = m_periodReserves[it.key()].range;
            DateRange overlap = range.intersect(m_period);

            // in the case where the last period perfectly coincided with the
            // end of the last range of this reserve
            if (overlap.isNull())
            {
               ++range;
               overlap = range.intersect(m_period);
               Q_ASSERT(!overlap.isNull());
            }

            // in the case where the overlap is the entire period
            if (overlap == m_period)
            {
               amount += m_categories[it.key()].amount.amortize(range, overlap);
            }
            // in the case where the overlap is at the beginning of the period
            // but does not cover the whole period
            else if (overlap.startDate() == m_period.startDate())
            {
               amount += m_categories[it.key()].amount.amortize(range, overlap);
               do
               {
                  ++range;
                  overlap = range.intersect(m_period);
                  amount += m_categories[it.key()].amount.amortize(range,
                                                                  overlap);
               }
               while (overlap.endDate() != m_period.endDate());
            }
            // otherwise we screwed up and advanced the reserve range too far
            // somewhere else
            else
            {
               std::cerr << "Internal error: reserve period advanced too far"
                         << std::endl;
            }

            m_periodReserves[it.key()].amount += amount;
            m_available -= amount;
            break;
         }
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            // TODO I don't like the way this is calculated
            DateRange oneDay(m_priorDays.startDate(), m_priorDays.startDate());
            m_routineEscrow += m_routineTotal.amortize(m_priorDays, oneDay) * m_period.days();
            break;
      }
   }
}

void BudgetBalancer::processAccount(LedgerAccountCommand const& account)
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

void BudgetBalancer::processRecords()
{
   for (int i = 0; i < m_numRecords; ++i)
   {
      if (m_recordedAccounts.contains(i))
      {
         processAccount(*m_recordedAccounts.find(i));
      }
      else if (m_recordedTransactions.contains(i))
      {
         processTransaction(*m_recordedTransactions.find(i));
      }
      else
      {
         std::cerr << "Internal logic error, missing record" << std::endl;
      }
   }
   m_recordedAccounts.clear();
   m_recordedTransactions.clear();
   m_numRecords = 0;
}

void BudgetBalancer::processTransaction(LedgerTransaction const& transaction)
{
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
         m_categories[entry.category()].type = LedgerBudget::Category::Type::ROUTINE;
      }
      switch (m_categories[entry.category()].type)
      {
         case LedgerBudget::Category::Type::GOAL:
            std::cerr << "I haven't figure this one out yet, TODO" << std::endl;
            break;
         case LedgerBudget::Category::Type::INCOME:
            for (auto it = m_categories.cbegin(); it != m_categories.cend();
                 ++it)
            {
               if (it->type == LedgerBudget::Category::Type::RESERVE_PERCENT)
               {
                  Currency amount = entry.amount().percentage(it->percentage);
                  m_percentReserves[entry.category()] += amount;
                  m_available -= amount;
                  if (m_percentReserves[entry.category()].isNegative())
                  {
                     std::cerr << "Reserve '" << qPrintable(entry.category())
                               << "' is underfunded, compensating."
                               << std::endl;
                     std::cerr << "  Amount: "
                               << qPrintable(m_percentReserves[entry.category()].toString())
                           << std::endl;
                     std::cerr << "  File: " << qPrintable(transaction.fileName())
                               << std::endl;
                     std::cerr << "  Line: " << transaction.lineNum() << std::endl;
                     m_available += m_percentReserves[entry.category()];
                     m_percentReserves[entry.category()].clear();
                  }
                  if (m_available.isNegative())
                  {
                     std::cerr << "Available funds are negative.  Trouble!"
                               << std::endl;
                  }
               }
            }
            break;
         case LedgerBudget::Category::Type::ROUTINE:
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
