#include "iptransactioncategorizer.h"

#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgertransaction.h"

void IPTransactionCategorizer::processItem(LedgerAccount const& account)
{
   // TODO might want to move balancer warnings to 'dies' into here
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
   // TODO this goes away with identifier migration
   Identifier account(transaction.account(), Identifier::Type::ACCOUNT);

   // TODO 'checkCreateAccount' or something to encapsulate this
   if (!m_accounts.contains(account))
   {
      warn(transaction.fileName(), transaction.lineNum(),
           QString("Automatically opening on-budget account '%1'")
           .arg(transaction.account()));
      m_accounts[account] = true;
   }

   // TODO this could be a dedicated method to handle transaction entries in
   // the same manner as budget and reserve entries
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      // TODO this goes away when entries use identifiers
      Identifier category;
      if (entry.hasCategory() && entry.isOwner())
      {
         category = Identifier(entry.category(), Identifier::Type::OWNER);
      }
      else if (entry.hasCategory() && !entry.isOwner())
      {
         category = Identifier(entry.category(), Identifier::Type::CATEGORY);
      }

      if (!m_accounts[account] &&
          category.type() != Identifier::Type::UNINITIALIZED)
      {
         die(transaction.fileName(), transaction.lineNum(),
             QString("Budget category set for off-budget account '%1'")
             .arg(transaction.account()));
      }

      // TODO this goes away when entries use identifiers
      Identifier payee;
      if (entry.transfer())
      {
         payee = Identifier(entry.payee(), Identifier::Type::ACCOUNT);
      }
      else
      {
         payee = Identifier(entry.payee(), Identifier::Type::GENERIC);
      }

      switch (payee.type())
      {
         case Identifier::Type::OWNER:
         case Identifier::Type::CATEGORY:
         case Identifier::Type::UNINITIALIZED:
            die("Internal logic error: payee has wrong type");
            // die never returns, so putting a break here triggers a warning
         case Identifier::Type::ACCOUNT:

            if (!m_accounts.contains(payee))
            {
               warn(transaction.fileName(), transaction.lineNum(),
                    QString("Automatically opening on-budget account '%1'")
                    .arg(entry.payee()));
               m_accounts[payee] = true;
            }

            if (m_accounts[account] && m_accounts[payee] &&
                category.type() != Identifier::Type::UNINITIALIZED)
            {
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Budget category set for transfer between on-budget "
                           "accounts '%1' and '%2'")
                   .arg(transaction.account())
                   .arg(entry.payee()));
            }
            else if (m_accounts[account] && !m_accounts[payee] &&
                     category.type() == Identifier::Type::UNINITIALIZED)
            {
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Missing budget category for transfer between "
                           "on-budget account '%1' and off-budget account '%2'")
                   .arg(transaction.account())
                   .arg(entry.payee()));
            }

            break;

         case Identifier::Type::GENERIC:

            if (m_accounts[account] &&
                category.type() == Identifier::Type::UNINITIALIZED)
            {
               die(transaction.fileName(), transaction.lineNum(),
                   QString("Missing budget category for on-budget account '%1'")
                   .arg(transaction.account()));
            }

            break;
      }
   }
}
