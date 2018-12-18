#include "accountbalancer.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgertransaction.h"

AccountBalancer::AccountBalancer(QObject* parent) :
   ItemProcessor(parent)
{
}

void AccountBalancer::processItem(LedgerAccountCommand const& account)
{
   switch (account.mode())
   {
      case LedgerAccountCommand::Mode::CLOSED:
         if (m_accounts.contains(account.account()))
         {
            if (!m_accounts[account.account()].isZero())
            {
               std::cerr << "Closing account with non-zero balance in file '"
                         << qPrintable(account.fileName()) << "', line "
                         << account.lineNum() << std::endl;
            }
            m_accounts.remove(account.account());
         }
         else
         {
            std::cerr << "Closing account that was not open in file '"
                      << qPrintable(account.fileName()) << "', line "
                      << account.lineNum() << std::endl;
         }
         break;
      case LedgerAccountCommand::Mode::ON_BUDGET:
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         if (!m_accounts.contains(account.account()))
         {
            m_accounts[account.account()];
         }
         else
         {
            std::cerr << "Opening account that was already open in file '"
                      << qPrintable(account.fileName()) << "', line "
                      << account.lineNum() << std::endl;
         }
         break;
   }
}

void AccountBalancer::processItem(LedgerBudgetAllocation const&)
{
}

void AccountBalancer::processItem(LedgerComment const&)
{
}

void AccountBalancer::processItem(LedgerTransaction const& transaction)
{
   if (!m_accounts.contains(transaction.account()))
   {
      std::cerr << "Transaction against unknown or closed account '"
                << qPrintable(transaction.account()) << "' in file '"
                << qPrintable(transaction.fileName()) << "', line "
                << transaction.lineNum() << std::endl;
   }
   m_accounts[transaction.account()] += transaction.amount();
   if (transaction.hasBalance() &&
       transaction.balance() != m_accounts[transaction.account()])
   {
      std::cerr << "Transaction balance incorrect in file '"
                << qPrintable(transaction.fileName()) << "', line "
                << transaction.lineNum() << std::endl;
      std::cerr << "   Transaction balance: "
                << qPrintable(transaction.balance().toString()) << std::endl;
      std::cerr << "   Calculated balance:  "
                << qPrintable(m_accounts[transaction.account()].toString())
            << std::endl;
   }

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.transfer())
      {
         m_transfers[transaction.account()][entry.payee()] += entry.amount();
      }
   }
}

void AccountBalancer::stop()
{
   for (auto it(m_accounts.cbegin()); it != m_accounts.cend(); ++it)
   {
      std::cout << "Account '" << qPrintable(it.key()) << "' has balance "
                << qPrintable(it.value().toString()) << std::endl;
   }

   for (auto it = m_transfers.cbegin(); it != m_transfers.cend(); ++it)
   {
      for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2)
      {
         Q_ASSERT(m_transfers.contains(it2.key()));
         Q_ASSERT(m_transfers[it2.key()].contains(it.key()));
         Currency side1 = *it2;
         // TODO dangerous if the other side does not exist, will modify structure and invalidate iterators
         Currency side2 = m_transfers[it2.key()][it.key()];
         if (!(side1 + side2).isZero())
         {
            std::cerr << "Transfers between '" << qPrintable(it.key())
                      << "' and '" << qPrintable(it2.key())
                      << "' do not balance.  Mismatch is "
                      << qPrintable((side1 + side2).toString()) << std::endl;
         }
      }
   }
}
