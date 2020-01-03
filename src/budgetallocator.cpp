#include "budgetallocator.h"

#include <QDebug>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"
#include "ledgertransactionentry.h"
#include "texttable.h"

void BudgetAllocator::finish()
{
   QTextStream out(stdout);
   TextTable table;
   Currency total;
   table.appendColumn(0, "== GOAL ==  ");
   table.appendColumn(1, "== RESERVED ==  ");
   table.appendColumn(2, "== NEEDED ==  ");
   table.appendColumn(3, "== AVAILABLE ==");
   for (auto it = m_goals.begin(); it != m_goals.end(); ++it)
   {
      table.appendColumn(0, it.key() + "  ");
      table.appendColumn(1, it->reservedThisPeriod.toString() + "  ");
      if ((it->reservedThisPeriod - it->neededThisPeriod).isNegative())
      {
         table.appendColumn(2, (it->neededThisPeriod -
                                it->reservedThisPeriod).toString() + "  ");
      }
      else
      {
         table.appendColumn(2, "---  ");
      }
      table.appendColumn(3, it->reserved.toString());
      total += it->reserved;
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(3, total.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(out);

   Currency reserved;
   for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
   {
      reserved += *it;
   }
   qDebug() << "reserved amounts" << reserved.toString();
   qDebug() << "routine expenses" << m_priorRoutine.toString();

   if (!m_priorPeriod.isNull())
   {
      qDebug() << "Routine escrow based on" << m_priorPeriod.startDate().toString() << "to" << m_priorPeriod.endDate().toString();
      qDebug() << "routine escrow balance" << m_escrow.toString();
      Currency daily = m_priorRoutine.amortize(m_priorPeriod, DateRange(m_priorPeriod.startDate(),m_priorPeriod.startDate()));
      qDebug() << "180 days is" << (daily * qint64(180)).toString();

      Currency difference = daily * qint64(180) - m_escrow;
      if (!difference.isZero() && !difference.isNegative())
      {
         qDebug() << "add" << difference.toString() << "to achieve 180 days";
      }
   }

   qDebug() << "remaining available" << m_available.toString();
}

void BudgetAllocator::processItem(LedgerAccount const& account)
{
   advanceBudgetPeriod(account.fileName(), account.lineNum(), account.date());
   if (account.mode() == LedgerAccount::Mode::ON_BUDGET)
   {
      m_available += account.balance();
   }
}

void BudgetAllocator::processItem(LedgerBudget const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      qDebug() << "ignoring future budget configuration";
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date(),
                       true);

   m_currentPeriod = DateRange(budget.date(), budget.interval());
   m_currentRoutine.clear();
   for (auto it = m_goals.cbegin(); it != m_goals.cend(); ++it)
   {
      if (!it->reserved.isZero())
      {
         qDebug() << "Returning" << it->reserved.toString() << "from category" << it.key() << "to available";
      }
      m_available += it->reserved;
   }
   m_goals.clear();
   m_incomes.clear();
   m_priorPeriod = DateRange();
   m_priorRoutine.clear();
   m_reserveAmounts.clear();
   m_reservePercentages.clear();
   m_reservePeriods.clear();
   for (auto it = m_reserves.cbegin(); it != m_reserves.cend(); ++it)
   {
      if (!it->isZero())
      {
         qDebug() << "Returning" << it->toString() << "from category" << it.key() << "to available";
      }
      m_available += *it;
   }
   m_reserves.clear();
   m_routines.clear();
}

void BudgetAllocator::processItem(LedgerBudgetGoalEntry const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_goals[budget.name()];
}

void BudgetAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_incomes.insert(budget.name());
}

void BudgetAllocator::processItem(LedgerBudgetReserveAmountEntry const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());

   m_reserveAmounts[budget.name()] = budget.amount();
   m_reservePeriods[budget.name()] = DateRange(m_currentPeriod.startDate(), budget.interval());
   m_reserves[budget.name()];

   // TODO this needs to be commonized with the code in the advancwe... method that does the same thing, and this one is probably buggy!
   // fund all reserve periods that end within this budget period
   while (m_reservePeriods[budget.name()].endDate() <= m_currentPeriod.endDate())
   {
      Currency amount = m_reserveAmounts[budget.name()];
      if ((m_available - amount).isNegative())
      {
         qDebug() << "unable to fully fund reserve amount";
         amount = m_available;
      }
      m_reserves[budget.name()] += amount;
      m_available -= amount;
      ++m_reservePeriods[budget.name()];
   }
   // fund this budget period's share of the next savings period that either starts in this period and ends in a future one, or starts after this period ends
   Currency amount = m_reserveAmounts[budget.name()].amortize(m_reservePeriods[budget.name()],
         m_reservePeriods[budget.name()].intersect(m_currentPeriod));
   if ((m_available - amount).isNegative())
   {
      qDebug() << "unable to fully fund reserve amount";
      amount = m_available;
   }
   m_reserves[budget.name()] += amount;
   m_available -= amount;
}

void BudgetAllocator::processItem(LedgerBudgetReservePercentEntry const &budget)
{
   if (budget.date() > QDate::currentDate())
   {
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_reservePercentages[budget.name()] = budget.percentage() / 100.0;
   m_reserves[budget.name()];
}

void BudgetAllocator::processItem(LedgerBudgetRoutineEntry const& budget)
{
   if (budget.date() > QDate::currentDate())
   {
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_routines.insert(budget.name());
}

void BudgetAllocator::processItem(LedgerReserve const& reserve)
{
   advanceBudgetPeriod(reserve.fileName(), reserve.lineNum(), reserve.date());
   if (!m_goals.contains(reserve.category()))
   {
      die(reserve.fileName(), reserve.lineNum(),
          "reserve command only for goals right now, sorry");
   }

   if ((m_available - reserve.amount()).isNegative())
   {
      qDebug() << "unable to fully fund reserve amount";
      m_goals[reserve.category()].reserved += m_available;
      m_goals[reserve.category()].reservedThisPeriod += m_available;
      m_available.clear();
   }
   else
   {
      m_goals[reserve.category()].reserved += reserve.amount();
      m_goals[reserve.category()].reservedThisPeriod += reserve.amount();
      m_available -= reserve.amount();
   }
}

void BudgetAllocator::processItem(LedgerTransaction const& transaction)
{
   advanceBudgetPeriod(transaction.fileName(), transaction.lineNum(),
                       transaction.date());
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      // ignore off-budget transaction entries, the account balancer will error
      // if a category was set or not set incorrectly
      if (!entry.hasCategory())
      {
         continue;
      }

      // ignore future incomes for budgeting purposes
      if (m_incomes.contains(entry.category()) &&
          transaction.date() > QDate::currentDate())
      {
         continue;
      }

      // ignore all non-goal transactions after the current budget period
      if (!m_goals.contains(entry.category()) &&
          transaction.date() > m_currentPeriod.endDate())
      {
         continue;
      }

      // create a new routine category if we haven't seen it before, since this
      // is the safest thing to do without a complete error out
      if (!m_goals.contains(entry.category()) &&
          !m_incomes.contains(entry.category()) &&
          !m_reserves.contains(entry.category()) &&
          !m_routines.contains(entry.category()))
      {
         warn(transaction.fileName(), transaction.lineNum(),
              QString("Automatically creating routine expense category '%1'")
              .arg(entry.category()));
         m_routines.insert(entry.category());
      }

      // process the transaction entry based on its budget category type
      if (m_goals.contains(entry.category()))
      {
         // this is a goal that has happened
         if (transaction.date() <= QDate::currentDate())
         {
            m_goals[entry.category()].reserved += entry.amount();
            if (m_goals[entry.category()].reserved.isNegative())
            {
               // TODO change this message, this is a case where a goal wan't saved for properly
               qDebug() << "goal category " << entry.category() << "underfunded, compensating";
               // TODO make generic function to allocate from available to a category, it gets used everywhere
               if ((m_available + m_goals[entry.category()].reserved).isNegative())
               {
                  qDebug() << "failing to allocate for category" << entry.category();
                  m_goals[entry.category()].reserved += m_available;
                  m_available.clear();
               }
               else
               {
                  m_available += m_goals[entry.category()].reserved;
                  m_goals[entry.category()].reserved.clear();
               }
            }
         }
         // this is a goal after today, and the budget period includes the
         // current date, note that the entry amount is negative
         else
         {
            // need to save more for this goal
            if ((m_goals[entry.category()].reserved + entry.amount()).isNegative())
            {
               // get the positive amount to save
               Currency toSave = Currency() - entry.amount() -
                                 m_goals[entry.category()].reserved;

               // find out the total date range left to save in, including the
               // current period dates
               DateRange goalPeriod = m_currentPeriod;
               while (goalPeriod.endDate() < transaction.date())
               {
                  ++goalPeriod;
               }
               goalPeriod = DateRange(m_currentPeriod.startDate(),
                                      goalPeriod.endDate());

               // split the total savings need for the current budget period
               Currency toAllocate = toSave.amortize(goalPeriod,
                                                     m_currentPeriod);
               m_goals[entry.category()].neededThisPeriod += toAllocate;

               // since we have to save for this entry, we have nothing left for
               // any additional goal entries
               m_goals[entry.category()].reserved.clear();
            }
            // we have enough, so "spend" from the saved goal money
            else
            {
               m_goals[entry.category()].reserved += entry.amount();
            }
         }
      }
      else if (m_incomes.contains(entry.category()))
      {
         m_available += entry.amount();
         if (m_available.isNegative())
         {
            qDebug() << "income brought available funds negative";
         }
         for (auto it = m_reservePercentages.begin();
              it != m_reservePercentages.end(); ++it)
         {
            Currency amount = entry.amount() * it.value();
            if ((m_available - amount).isNegative())
            {
               qDebug() << "failing to allocate for category" << it.key();
               amount = m_available;
            }
            m_reserves[it.key()] += amount;
            m_available -= amount;
         }
      }
      else if (m_reserves.contains(entry.category()))
      {
         m_reserves[entry.category()] += entry.amount();
         if (m_reserves[entry.category()].isNegative())
         {
            qDebug() << "category" << entry.category() << "overspent, compensating";
            if ((m_available + m_reserves[entry.category()]).isNegative())
            {
               qDebug() << "failing to allocate for category" << entry.category();
               m_reserves[entry.category()] += m_available;
               m_available.clear();
            }
            else
            {
               m_available += m_reserves[entry.category()];
               m_reserves[entry.category()].clear();
            }
         }
      }
      else if (m_routines.contains(entry.category()))
      {
         m_currentRoutine += entry.amount();
         m_escrow += entry.amount();
         if (m_escrow.isNegative())
         {
            qDebug() << "routine escrow overspent, compensating";
            if ((m_available + m_escrow).isNegative())
            {
               qDebug() << "failing to allocate for routine escrow";
               m_escrow += m_available;
               m_available.clear();
            }
            else
            {
               m_available += m_escrow;
               m_escrow.clear();
            }
         }
      }
      else
      {
         Q_ASSERT_X(false, "processItem", "Category type not handled");
      }
   }
}

void BudgetAllocator::advanceBudgetPeriod(QString const& filename, uint lineNum,
                                          QDate const& date, bool rebudgeting)
{
   // use a monthly period by default if not initialized otherwise
   if (m_currentPeriod.isNull())
   {
      // but only warn if we aren't about to replace the default with a new one
      if (!rebudgeting)
      {
         warn(filename, lineNum, "Creating a default monthly budget period");
      }
      m_currentPeriod = DateRange(QDate(date.year(), date.month(), 1),
                                  Interval(1, Interval::Period::MONTHS));
   }

   if (date < m_currentPeriod.startDate())
   {
      die(filename, lineNum,
          "Cannot rewind budget period for earlier dated item");
   }

   // stop advancing once the budget period covers either the requested date or
   // the current date, whichever comes first
   while (m_currentPeriod.endDate() < date &&
          m_currentPeriod.endDate() < QDate::currentDate())
   {
      // merge escrow info and reset for the new budget period
      if (m_priorPeriod.isNull())
      {
         m_priorPeriod = m_currentPeriod;
      }
      else
      {
         m_priorPeriod = DateRange(m_priorPeriod.startDate(),
                                   m_currentPeriod.endDate());
      }
      m_priorRoutine += m_currentRoutine;
      m_currentRoutine.clear();

      // reset tracking how much we reserved in the current period for goals
      for (auto it = m_goals.begin(); it != m_goals.end(); ++it)
      {
         it->neededThisPeriod.clear();
         it->reservedThisPeriod.clear();
      }

      // advance the budget period to the new dates
      ++m_currentPeriod;

      // if we are rebudgeting and the new period starts on the same day as the
      // old one would have, skip allocating funds for that old period
      if (rebudgeting && date == m_currentPeriod.startDate())
      {
         continue;
      }

      // fund each category that allocates amounts per budget period
      for (auto it = m_reserveAmounts.begin(); it != m_reserveAmounts.end();
           ++it)
      {
         // fund all reserve periods that start before this budget period ends,
         // considering that the current reserve period might have started
         // before this budget period, so only do the overlap
         while (m_reservePeriods[it.key()].startDate() <=
                m_currentPeriod.endDate())
         {
            // since the savings period and the budget period can be different,
            // we only want to allocate the amount that represents the overlap
            // between the savings period and the budget period
            Currency amount = m_reserveAmounts[it.key()].amortize(
                                 m_reservePeriods[it.key()],
                                 m_reservePeriods[it.key()].intersect(
                                       m_currentPeriod));

            // if we can't fund it all, take what we can get
            if ((m_available - amount).isNegative())
            {
               warn(filename, lineNum,
                    QString("Unable to fully fund reserve category '%1' in "
                            "budget period %2-%3.  Desired amount %4, "
                            "available amount %5")
                    .arg(it.key())
                    .arg(m_currentPeriod.startDate().toString())
                    .arg(m_currentPeriod.endDate().toString())
                    .arg(amount.toString())
                    .arg(m_available.toString()));
               amount = m_available;
            }

            // move funds from available to savings goal
            m_reserves[it.key()] += amount;
            m_available -= amount;

            // if the reserve period extends into the next budget period, we
            // have to stop without incrementing it, so the next budget period
            // can handle the remainder of this reserve period
            if (m_reservePeriods[it.key()].endDate() >
                m_currentPeriod.endDate())
            {
               break;
            }
            // otherwise increment the reserve period and let the loop run again
            else
            {
               ++m_reservePeriods[it.key()];
            }
         }
      }

      // fund the routine escrow account based on prior daily routine expenses
      // and the duration of the current budget period
      Currency daily = m_priorRoutine.amortize(
                          m_priorPeriod,
                          DateRange(m_priorPeriod.startDate(),
                                    m_priorPeriod.startDate())) *
                       m_currentPeriod.days();
      // note that the daily value was a negative number, since it was based on
      // routine expenses
      daily = Currency() - daily;
      // if it is too much, take what we can
      if ((m_available - daily).isNegative())
      {
         warn(filename, lineNum,
              QString("Unable to fully reserve for routine expenses in budget "
                      "period %1-%2.  Desired amount %3, available amount %4")
              .arg(m_currentPeriod.startDate().toString())
              .arg(m_currentPeriod.endDate().toString())
              .arg(daily.toString())
              .arg(m_available.toString()));
         m_escrow += m_available;
         m_available.clear();
      }
      else
      {
         m_available -= daily;
         m_escrow += daily;
      }
   }
}
