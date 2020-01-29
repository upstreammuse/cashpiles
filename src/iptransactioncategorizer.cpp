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
   // TODO 'checkCreateAccount' or something to encapsulate this
   if (!m_accounts.contains(transaction.account()))
   {
      warn(transaction.fileName(), transaction.lineNum(),
           QString("Automatically opening on-budget account '%1'")
           .arg(transaction.account()));
      m_accounts[transaction.account()] = true;
   }

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
         case Identifier::Type::OWNER:
         case Identifier::Type::CATEGORY:
         case Identifier::Type::UNINITIALIZED:
            die("Internal logic error: payee has wrong type");
            // die never returns, so putting a break here triggers a warning
         case Identifier::Type::ACCOUNT:

            if (!m_accounts.contains(entry.payee()))
            {
               warn(transaction.fileName(), transaction.lineNum(),
                    QString("Automatically opening on-budget account '%1'")
                    .arg(entry.payee()));
               m_accounts[entry.payee()] = true;
            }

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
      }
   }
}
