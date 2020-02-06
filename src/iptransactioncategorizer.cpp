#include "iptransactioncategorizer.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgertransaction.h"

void IPTransactionCategorizer::processItem(LedgerAccount const& account)
{
   switch (account.mode())
   {
      case LedgerAccount::Mode::CLOSED:
         m_accounts.remove(account.name());
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

   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (!m_accounts[transaction.account()] &&
          entry.category().type() != Identifier::Type::UNINITIALIZED)
      {
         die(transaction.fileName(), transaction.lineNum(),
             QString("Budget category set for off-budget account '%1'")
             .arg(transaction.account()));
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
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Budget category set for transfer between on-budget "
                           "accounts '%1' and '%2'")
                   .arg(transaction.account())
                   .arg(entry.payee()));
            }
            else if (m_accounts[transaction.account()] &&
                     !m_accounts[entry.payee()] &&
                     entry.category().type() == Identifier::Type::UNINITIALIZED)
            {
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Missing budget category for transfer between "
                           "on-budget account '%1' and off-budget account '%2'")
                   .arg(transaction.account())
                   .arg(entry.payee()));
            }
            break;
         case Identifier::Type::GENERIC:
            if (m_accounts[transaction.account()] &&
                entry.category().type() == Identifier::Type::UNINITIALIZED)
            {
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Missing budget category for on-budget account '%1'")
                   .arg(transaction.account()));
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
      Identifier const& account, QString const& filename, uint linenum)
{
   if (!m_accounts.contains(account))
   {
      warn(filename, linenum,
           QString("Automatically opening on-budget account '%1'")
           .arg(account));
      m_accounts[account] = true;
   }
}
