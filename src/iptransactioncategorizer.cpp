#include "iptransactioncategorizer.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgertransaction.h"

void IPTransactionCategorizer::processItem(LedgerAccount const& account)
{
   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         m_accounts.erase(account.name());
         break;
      case LedgerAccount::Mode::ON_BUDGET:
         m_accounts[account.name()] = true;
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
         m_accounts[account.name()] = false;
         break;
   }
}

void IPTransactionCategorizer::processItem(LedgerTransaction const& transaction)
{
   checkCreateAccount(transaction.account(), transaction.fileName(),
                      transaction.lineNum());

   for (LedgerTransactionEntry const& entry : transaction.entries())
   {
      if (!m_accounts[transaction.account()] &&
          entry.category().type() != Identifier::Type::UNINITIALIZED)
      {
         std::stringstream ss;
         ss << "Budget category set for off-budget account '"
            << transaction.account() << "'";
         die(transaction.fileName(), transaction.lineNum(), ss.str());
      }

      switch (entry.payee().type())
      {
         case Identifier::Type::ACCOUNT:
            checkCreateAccount(entry.payee(), transaction.fileName(),
                               transaction.lineNum());
            if (m_accounts[transaction.account()] &&
                m_accounts[entry.payee()] &&
                entry.category().type() != Identifier::Type::UNINITIALIZED)
            {
               std::stringstream ss;
               ss << "Budget category set for transfer between on-budget "
                     "accounts '" << transaction.account() << "' and '"
                  << entry.payee() << "'";
               die(transaction.fileName(), transaction.lineNum(), ss.str());
            }
            else if (m_accounts[transaction.account()] &&
                     !m_accounts[entry.payee()] &&
                     entry.category().type() == Identifier::Type::UNINITIALIZED)
            {
               std::stringstream ss;
               ss << "Missing budget category for transfer between "
                     "on-budget account '" << transaction.account()
                  << "' and off-budget account '" << entry.payee() << "'";
               die(transaction.fileName(), transaction.lineNum(), ss.str());
            }
            break;
         case Identifier::Type::GENERIC:
            if (m_accounts[transaction.account()] &&
                entry.category().type() == Identifier::Type::UNINITIALIZED)
            {
               std::stringstream ss;
               ss << "Missing budget category for on-budget account '"
                  << transaction.account() << "'";
               die(transaction.fileName(), transaction.lineNum(), ss.str());
            }
            break;
         case Identifier::Type::OWNER:
         case Identifier::Type::CATEGORY:
         case Identifier::Type::UNINITIALIZED:
            die("Internal logic error: payee has wrong type");
      }
   }
}

void IPTransactionCategorizer::checkCreateAccount(
      Identifier const& account, std::string const& filename, size_t linenum)
{
   if (!m_accounts.count(account))
   {
      std::stringstream ss;
      ss << "Automatically opening on-budget account '" << account << "'";
      warn(filename, linenum, ss.str());
      m_accounts[account] = true;
   }
}
