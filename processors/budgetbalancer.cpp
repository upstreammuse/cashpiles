#include "budgetbalancer.h"

#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgertransaction.h"

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
               m_routineAllocator.deallocate(it.key());
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
   // TODO allocate goals
   m_available = m_reserveAmountAllocator.allocate(m_period, m_available);
   m_available = m_routineAllocator.allocate(m_period, m_available);

   if (m_available.isNegative())
   {
      emit message(QString("Available funds negative for budget period %1 to "
                           "%2")
                   .arg(m_period.startDate().toString())
                   .arg(m_period.endDate().toString()));
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
      Q_ASSERT(m_recordedAccounts.contains(i) ||
               m_recordedTransactions.contains(i));
      if (m_recordedAccounts.contains(i))
      {
         processAccount(*m_recordedAccounts.find(i));
      }
      else if (m_recordedTransactions.contains(i))
      {
         processTransaction(*m_recordedTransactions.find(i));
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
            emit message(transaction,
                         QString("Transactions for off-budget account '%1' "
                                 "cannot have categories")
                         .arg(transaction.account()));
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
            emit message(transaction,
                         QString("Transaction missing a category for payee "
                                 "'%1'")
                         .arg(entry.payee()));
         }
         continue;
      }

      // if it is a transfer to another on-budget account, it shouldn't have a
      // category
      if (entry.transfer() && m_accounts[entry.payee()])
      {
         emit message(transaction,
                      QString("Transfers between on-budget accounts '%1' and "
                              "'%2' cannot have a category")
                      .arg(transaction.account())
                      .arg(entry.payee()));
         continue;
      }

      if (!m_categories.contains(entry.category()))
      {
         emit message(transaction,
                      QString("Unknown category '%1'")
                      .arg(entry.category()));
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
               emit message(transaction,
                            QString("Reserved category '%1' is underfunded, "
                                    "compensating")
                            .arg(entry.category()));
               m_available = m_reservePercentAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
            m_reserveAmountAllocator.spend(entry.category(), entry.amount());
            if (m_reserveAmountAllocator.isUnderfunded())
            {
               emit message(transaction,
                            QString("Reserved category '%1' is underfunded, "
                                    "compensating")
                            .arg(entry.category()));
               m_available = m_reserveAmountAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            m_reservePercentAllocator.spend(entry.category(), entry.amount());
            if (m_reservePercentAllocator.isUnderfunded())
            {
               emit message(transaction,
                            QString("Reserved category '%1' is underfunded, "
                                    "compensating")
                            .arg(entry.category()));
               m_available = m_reservePercentAllocator.allocate(m_available);
            }
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            m_routineAllocator.spend(transaction.date(), entry.category(),
                                     entry.amount());
            if (m_routineAllocator.isUnderfunded())
            {
               emit message(transaction,
                            QString("Routine category '%1' is underfunded, "
                                    "compensating")
                            .arg(entry.category()));
               m_available = m_routineAllocator.allocate(m_available);
            }
            break;
      }
      if (m_available.isNegative())
      {
         emit message(transaction, "Available funds are negative.  Trouble!");
      }
   }
}
