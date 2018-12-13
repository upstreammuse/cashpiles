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
            if (m_accounts[account.account()] != 0)
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
            m_accounts[account.account()] = 0;
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
      m_accounts[transaction.account()] = 0;
   }
   m_accounts[transaction.account()] += transaction.amount();
   if (transaction.hasBalance() &&
       transaction.balance() != m_accounts[transaction.account()])
   {
      std::cerr << "Transaction balance incorrect in file '"
                << qPrintable(transaction.fileName()) << "', line "
                << transaction.lineNum() << std::endl;
      std::cerr << "   Transaction balance: " << transaction.balance()
                << std::endl;
      std::cerr << "   Calculated balance:  "
                << m_accounts[transaction.account()] << std::endl;
   }
}
