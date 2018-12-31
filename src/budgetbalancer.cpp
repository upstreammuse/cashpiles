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

   // remove categories that are not in this budget command, or that have changed, and allocate their
   // funds to the available category
   // TODO warn if a reserve category is eliminated that has funds in it, so that
   // the user can opt to transfer the funds to a different category before the category
   // goes away
   //  - how do we do that right now?
   //     - split transaction, zero total, outflow from old category, inflow to new category
   auto categories = budget.categories();
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      if (!categories.contains(it.key()) ||
          categories[it.key()].type != it->type)
      {
         switch (it->type)
         {
//            case LedgerBudget::Category::Type::GOAL:
            case LedgerBudget::Category::Type::INCOME:
               // nothing to do for income type
               break;
            case LedgerBudget::Category::Type::RESERVE_AMOUNT:
               m_available += m_reserveAmountAllocator.deallocate(it.key());
               break;
            case LedgerBudget::Category::Type::RESERVE_PERCENT:
               m_available += m_reservePercentAllocator.deallocate(it.key());
               break;
            case LedgerBudget::Category::Type::ROUTINE:
               m_available += m_routineAllocator.deallocate(it.key());
               break;
         }

      }
   }

   // configure new and changed budget categories
   for (auto it = categories.cbegin(); it != categories.cend(); ++it)
   {
      switch (it->type)
      {
         case LedgerBudget::Category::Type::INCOME:
            // nothing to do for income
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
            m_reserveAmountAllocator.budget(budget.date(), it.key(), it->amount,
                                            it->interval);
            break;
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            m_reservePercentAllocator.budget(it.key(), it->percentage);
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            // nothing to do for routine expenses
            break;
      }
   }
   m_categories = categories;

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
   }

   while (m_period.endDate() < date)
   {
      allocateCategories();
      processRecords();
      ++m_period;
   }
}

void BudgetBalancer::allocateCategories()
{
   for (auto it = m_categories.cbegin(); it != m_categories.cend(); ++it)
   {
      switch (it->type)
      {
//         case LedgerBudget::Category::Type::GOAL:
//            break;
         case LedgerBudget::Category::Type::INCOME:
            // these are allocated when income arrives
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
            m_available = m_reserveAmountAllocator.allocate(m_period, it.key(),
                                                            m_available);
            break;
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            // nothing to do for percentage categories
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            m_available = m_routineAllocator.allocate(m_period, it.key(),
                                                      m_available);
            break;
      }
      if (m_available.isNegative())
      {
         std::cerr << "Available funds are negative, trouble!" << std::endl;
         std::cerr << "  Category: " << qPrintable(it.key()) << std::endl;
         std::cerr << "  Budget Period: "
                   << qPrintable(m_period.startDate().toString()) << " to "
                   << qPrintable(m_period.endDate().toString()) << std::endl;
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
         m_categories[entry.category()].type =
               LedgerBudget::Category::Type::ROUTINE;
      }
      switch (m_categories[entry.category()].type)
      {
//         case LedgerBudget::Category::Type::GOAL:
//            break;
         case LedgerBudget::Category::Type::INCOME:
            m_available += entry.amount();
            m_available = m_reservePercentAllocator.allocate(entry.amount(),
                                                             m_available);
            if (m_reservePercentAllocator.isUnderfunded())
            {
               std::cerr << "Reserved category is underfunded, compensating."
                         << std::endl;
               std::cerr << "  File: " << qPrintable(transaction.fileName())
                         << std::endl;
               std::cerr << "  Line: " << transaction.lineNum() << std::endl;
               m_available = m_reservePercentAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
            m_reserveAmountAllocator.spend(entry.category(), entry.amount());
            if (m_reserveAmountAllocator.isUnderfunded())
            {
               std::cerr << "Rserved category is underfunded, compensating."
                         << std::endl;
               std::cerr << "  File: " << qPrintable(transaction.fileName())
                         << std::endl;
               std::cerr << "  Line: " << transaction.lineNum() << std::endl;
               m_available = m_reserveAmountAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            m_reservePercentAllocator.spend(entry.category(), entry.amount());
            if (m_reservePercentAllocator.isUnderfunded())
            {
               std::cerr << "Rserved category is underfunded, compensating."
                         << std::endl;
               std::cerr << "  File: " << qPrintable(transaction.fileName())
                         << std::endl;
               std::cerr << "  Line: " << transaction.lineNum() << std::endl;
               m_available = m_reservePercentAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            m_routineAllocator.spend(transaction.date(), entry.category(),
                                     entry.amount());
            if (m_routineAllocator.isUnderfunded())
            {
               std::cerr << "Routine expenses are underfunded, compensating."
                         << std::endl;
               std::cerr << "  File: " << qPrintable(transaction.fileName())
                         << std::endl;
               std::cerr << "  Line: " << transaction.lineNum() << std::endl;
               m_available = m_routineAllocator.allocate(m_available);
            }
            break;
      }
      if (m_available.isNegative())
      {
         std::cerr << "Available funds are negative.  Trouble!"
                   << std::endl;
      }
   }
}
