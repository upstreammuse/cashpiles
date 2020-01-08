#include "budgetallocator.h"

#include <QTextStream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgerbudget.h"
#include "ledgerreserve.h"
#include "ledgertransaction.h"
#include "ledgertransactionentry.h"
#include "texttable.h"

BudgetAllocator::BudgetAllocator(QDate const& today) :
   m_today(today)
{
   if (!m_today.isValid())
   {
      die(QString("Today's date '%1' is invalid").arg(m_today.toString()));
   }
}

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
   out << endl;

   table.clear();
   total.clear();
   table.appendColumn(0, "== RESERVE ==  ");
   table.appendColumn(1, "== AMOUNT ==  ");
   table.appendColumn(2, "== PERIOD ==  ");
   table.appendColumn(3, "== PERCENTAGE ==  ");
   table.appendColumn(4, "== AVAILABLE ==");
   for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
   {
      table.appendColumn(0, it.key() + "  ");
      table.appendColumn(1, it->amount.toString() + "  ");
      table.appendColumn(2, it->period.startDate().toString() + "-" +
                         it->period.endDate().toString() + "  ");
      table.appendColumn(3, QString::number(it->percentage) + "  ");
      table.appendColumn(4, it->reserved.toString());
      total += it->reserved;
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(4, total.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(4, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;

   table.clear();
   total.clear();
   table.appendColumn(0, "Routine  ");
   table.appendColumn(1, "History  ");
   table.appendColumn(2, "6-Month  ");
   table.appendColumn(3, "Available");
   out << "Routine history period " << m_priorPeriod.startDate().toString()
       << " - " << m_priorPeriod.endDate().toString() << endl;
   for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
   {
      table.appendColumn(0, it.key() + "  ");
      table.appendColumn(1, it->priorAmount.toString() + "  ");
      table.appendColumn(2, (it->priorAmount.amortize(
                                m_priorPeriod,
                                DateRange(m_priorPeriod.startDate(),
                                          m_priorPeriod.startDate())) *
                             qint64(180)).toString() + "  ");
      table.appendColumn(3, it->reserved.toString());
      total += it->reserved;
   }
   table.appendColumn(0, "== TOTAL ==  ");
   table.appendColumn(3, total.toString());
   table.setColumnAlignment(1, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(2, TextTable::Alignment::RightAlign);
   table.setColumnAlignment(3, TextTable::Alignment::RightAlign);
   table.print(out);
   out << endl;

   out << "Available to budget: " << endl;
   for (auto it = m_availables.begin(); it != m_availables.end(); ++it)
   {
      out << it.key() << ": " << it->toString() << endl;
      if (it->isNegative())
      {
         out << "WARNING: over budget!" << endl;
      }
   }
}

void BudgetAllocator::processItem(LedgerAccount const& account)
{
   advanceBudgetPeriod(account.fileName(), account.lineNum(), account.date());
}

void BudgetAllocator::processItem(LedgerBudget const& budget)
{
   if (budget.date() > m_today)
   {
      warn(budget.fileName(), budget.lineNum(),
           "Ignoring future budget configuration");
      return;
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date(),
                       true);

   m_currentPeriod = DateRange(budget.date(), budget.interval());
   for (auto it = m_goals.cbegin(); it != m_goals.cend(); ++it)
   {
      if (!it->reserved.isZero())
      {
         warn(budget.fileName(), budget.lineNum(),
              QString("Returning %1 from category '%2' to available")
              .arg(it->reserved.toString())
              .arg(it.key()));
      }
      m_availables[m_owners[it.key()]] += it->reserved;
   }
   m_goals.clear();
   m_incomes.clear();
   m_priorPeriod = DateRange();
   for (auto it = m_reserves.cbegin(); it != m_reserves.cend(); ++it)
   {
      if (!it->reserved.isZero())
      {
         warn(budget.fileName(), budget.lineNum(),
              QString("Returning %1 from category '%2' to available")
              .arg(it->reserved.toString())
              .arg(it.key()));
      }
      m_availables[m_owners[it.key()]] += it->reserved;
   }
   m_reserves.clear();
   for (auto it = m_routines.cbegin(); it != m_routines.cend(); ++it)
   {
      if (!it->reserved.isZero())
      {
         warn(budget.fileName(), budget.lineNum(),
              QString("Returning %1 from category '%2' to available")
              .arg(it->reserved.toString())
              .arg(it.key()));
      }
      m_availables[m_owners[it.key()]] += it->reserved;
   }
   m_routines.clear();
}

void BudgetAllocator::processItem(LedgerBudgetGoalEntry const& budget)
{
   if (budget.date() > m_today)
   {
      return;
   }
   if (m_owners.contains(budget.name()))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_availables[budget.owner()];
   m_goals[budget.name()];
   m_owners[budget.name()] = budget.owner();
}

void BudgetAllocator::processItem(LedgerBudgetIncomeEntry const& budget)
{
   if (budget.date() > m_today)
   {
      return;
   }
   if (m_owners.contains(budget.name()))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_availables[budget.owner()];
   m_incomes.insert(budget.name());
   m_owners[budget.name()] = budget.owner();
}

void BudgetAllocator::processItem(LedgerBudgetReserveAmountEntry const& budget)
{
   if (budget.date() > m_today)
   {
      return;
   }
   if (m_owners.contains(budget.name()))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());

   m_availables[budget.owner()];
   m_owners[budget.name()] = budget.owner();
   m_reserves[budget.name()].amount = budget.amount();
   m_reserves[budget.name()].period = DateRange(m_currentPeriod.startDate(),
                                                budget.interval());
   syncReserve(budget.name());
}

void BudgetAllocator::processItem(LedgerBudgetReservePercentEntry const &budget)
{
   if (budget.date() > m_today)
   {
      return;
   }
   if (m_owners.contains(budget.name()))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_availables[budget.owner()];
   m_owners[budget.name()] = budget.owner();
   m_reserves[budget.name()].percentage = budget.percentage() / 100.0;
}

void BudgetAllocator::processItem(LedgerBudgetRoutineEntry const& budget)
{
   if (budget.date() > m_today)
   {
      return;
   }
   if (m_owners.contains(budget.name()))
   {
      die(budget.fileName(), budget.lineNum(),
          "Budget category listed multiple times");
   }
   advanceBudgetPeriod(budget.fileName(), budget.lineNum(), budget.date());
   m_availables[budget.owner()];
   m_owners[budget.name()] = budget.owner();
   m_routines[budget.name()];
}

void BudgetAllocator::processItem(LedgerReserve const& reserve)
{
   advanceBudgetPeriod(reserve.fileName(), reserve.lineNum(), reserve.date());

   // TODO need the validation code inside the LedgerReserve to make sure they
   // actually balance to the stated amount, as with transactions
   if (reserve.numEntries() > 1 && !reserve.amount().isZero())
   {
      die(reserve.fileName(), reserve.lineNum(),
          "Multi-line reserve commands must balance to zero");
   }

   if (reserve.numEntries() < 2)
   {
      m_singleReserve = true;
   }
}

// TODO could make sure that all categories are owned by the owners listed, to prevent accidentally moving money into someone else's category
// TODO alternatively, could have a separate command for within an owner, and owner to owner, but that feels artificial
// TODO alternatively, could have the reserve command require the owner as well as the category, which isn't too onerous, even if redundant
void BudgetAllocator::processItem(LedgerReserveEntry const& reserve)
{
   advanceBudgetPeriod(reserve.fileName(), reserve.lineNum(), reserve.date());

   if (reserve.isOwner())
   {
      m_availables[reserve.category()] += reserve.amount();
   }
   else
   {
      if (!m_goals.contains(reserve.category()))
      {
         die(reserve.fileName(), reserve.lineNum(),
             "reserve command only for goals right now, sorry");
      }

      // if the reservation is in this period, get credit for it
      if (reserve.date() <= m_currentPeriod.endDate())
      {
         m_goals[reserve.category()].reservedThisPeriod += reserve.amount();
      }

      // either way, reserve the amount, which will reduce available budget amount
      // but also 'buy down' the amount needed to reserve now
      m_goals[reserve.category()].reserved += reserve.amount();

      if (m_singleReserve)
      {
         m_availables[m_owners[reserve.category()]] -= reserve.amount();
         m_singleReserve = false;
      }
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
      if (m_incomes.contains(entry.category()) && transaction.date() > m_today)
      {
         continue;
      }

      // ignore all non-goal transactions after the current budget period
      if (!m_goals.contains(entry.category()) &&
          transaction.date() > m_currentPeriod.endDate())
      {
         continue;
      }

      // if the category is an owner, make sure we recognize it
      if (entry.isOwner() && !m_availables.contains(entry.category()))
      {
         die(transaction.fileName(), transaction.lineNum(),
             QString("Unknown category owner '%1'").arg(entry.category()));
      }

      // create a new routine category if we haven't seen it before, since this
      // is the safest thing to do without a complete error out
      if (!entry.isOwner() &&
          !m_goals.contains(entry.category()) &&
          !m_incomes.contains(entry.category()) &&
          !m_reserves.contains(entry.category()) &&
          !m_routines.contains(entry.category()))
      {
         warn(transaction.fileName(), transaction.lineNum(),
              QString("Automatically creating routine expense category '%1'")
              .arg(entry.category()));
         m_routines[entry.category()];
      }

      // process the transaction entry based on its budget category type
      if (entry.isOwner())
      {
         m_availables[entry.category()] += entry.amount();
      }
      else if (m_goals.contains(entry.category()))
      {
         // this is a goal that has happened
         if (transaction.date() <= m_today)
         {
            m_goals[entry.category()].reserved += entry.amount();
            if (m_goals[entry.category()].reserved.isNegative())
            {
               warn(transaction.fileName(), transaction.lineNum(),
                    QString("Goal category %1 overspent")
                    .arg(entry.category()));
               m_availables[m_owners[entry.category()]] +=
                     m_goals[entry.category()].reserved;
               m_goals[entry.category()].reserved.clear();
            }
         }
         // this is a goal after today, and the budget period includes the
         // current date, note that the entry amount is negative
         else
         {
            // need to save more for this goal
            if ((m_goals[entry.category()].reserved +
                 entry.amount()).isNegative())
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
               m_goals[entry.category()].neededThisPeriod +=
                     toSave.amortize(goalPeriod, m_currentPeriod);

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
         m_availables[m_owners[entry.category()]] += entry.amount();
         for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
         {
            // skip reserves that are not percentage based
            if (!it->period.isNull()) continue;

            // skip reserves with a different owner
            if (m_owners[it.key()] != m_owners[entry.category()]) continue;

            Currency amount = entry.amount() * it->percentage;
            it->reserved += amount;
            m_availables[m_owners[it.key()]] -= amount;
         }
      }
      else if (m_reserves.contains(entry.category()))
      {
         m_reserves[entry.category()].reserved += entry.amount();
         if (m_reserves[entry.category()].reserved.isNegative())
         {
            m_availables[m_owners[entry.category()]] +=
                  m_reserves[entry.category()].reserved;
            m_reserves[entry.category()].reserved.clear();
         }
      }
      else if (m_routines.contains(entry.category()))
      {
         // TODO see about changing the sign of currentAmount
         m_routines[entry.category()].currentAmount += entry.amount();
         m_routines[entry.category()].reserved += entry.amount();
         if (m_routines[entry.category()].reserved.isNegative())
         {
            m_availables[m_owners[entry.category()]] +=
                  m_routines[entry.category()].reserved;
            m_routines[entry.category()].reserved.clear();
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
          m_currentPeriod.endDate() < m_today)
   {
      // merge routine info and reset for the new budget period
      if (m_priorPeriod.isNull())
      {
         m_priorPeriod = m_currentPeriod;
      }
      else
      {
         m_priorPeriod = DateRange(m_priorPeriod.startDate(),
                                   m_currentPeriod.endDate());
      }
      for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
      {
         it->priorAmount += it->currentAmount;
         it->currentAmount.clear();
      }

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
      for (auto it = m_reserves.begin(); it != m_reserves.end(); ++it)
      {
         // skip reserves that are not amount/period based
         if (it->period.isNull()) continue;

         syncReserve(it.key());
      }

      // fund the routine categories based on prior daily routine expenses and
      // the duration of the current budget period
      for (auto it = m_routines.begin(); it != m_routines.end(); ++it)
      {
         // note that the daily value was a negative number, since it was based
         // on routine expenses
         Currency daily = it->priorAmount.amortize(
                             m_priorPeriod,
                             DateRange(m_priorPeriod.startDate(),
                                       m_priorPeriod.startDate())) *
                          m_currentPeriod.days();
         daily = Currency() - daily;
         m_availables[m_owners[it.key()]] -= daily;
         it->reserved += daily;
      }
   }
}

void BudgetAllocator::syncReserve(QString const& category)
{
   // fund all reserve periods that start before this budget period ends,
   // considering that the current reserve period might have started
   // before this budget period, so only do the overlap
   while (m_reserves[category].period.startDate() <= m_currentPeriod.endDate())
   {
      // since the savings period and the budget period can be different,
      // we only want to allocate the amount that represents the overlap
      // between the savings period and the budget period
      Currency amount = m_reserves[category].amount.amortize(
                           m_reserves[category].period,
                           m_reserves[category].period.intersect(
                              m_currentPeriod));

      // move funds from available to savings goal
      m_reserves[category].reserved += amount;
      m_availables[m_owners[category]] -= amount;

      // if the reserve period extends into the next budget period, we
      // have to stop without incrementing it, so the next budget period
      // can handle the remainder of this reserve period
      if (m_reserves[category].period.endDate() > m_currentPeriod.endDate())
      {
         break;
      }
      // otherwise increment the reserve period and let the loop run again
      else
      {
         ++m_reserves[category].period;
      }
   }
}
