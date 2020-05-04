#include "iptransactioncategorizer.h"

#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"

using std::stringstream;

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
   checkCreateOff(transaction.account(), transaction.fileName(),
                  transaction.lineNum());

   if (m_accounts[transaction.account()])
   {
      std::stringstream ss;
      ss << "Cannot use off-budget transaction with on-budget account '"
         << transaction.account() << "'";
      die(transaction.fileName(), transaction.lineNum(), ss.str());
   }
}

void IPTransactionCategorizer::processItem(
      LedgerTransactionV2AccountEntry const& entry)
{
   checkCreateOn(entry.account(), entry.fileName(), entry.lineNum());
   if (!m_accounts[entry.account()])
   {
      stringstream ss;
      ss << "Cannot use off-budget account '" << entry.account()
         << "'as transaction entry";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPTransactionCategorizer::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   if (entry.trackingAccount().second)
   {
      checkCreateOff(entry.trackingAccount().first, entry.fileName(),
                     entry.lineNum());
   }
   if (m_accounts[entry.trackingAccount().first])
   {
      stringstream ss;
      ss << "Cannot use on-budget account '" << entry.trackingAccount().first
         << "'as tracking account for transaction entry";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPTransactionCategorizer::processItem(
      LedgerTransactionV2OwnerEntry const& entry)
{
   if (entry.trackingAccount().second)
   {
      checkCreateOff(entry.trackingAccount().first, entry.fileName(),
                     entry.lineNum());
   }
   if (m_accounts[entry.trackingAccount().first])
   {
      stringstream ss;
      ss << "Cannot use on-budget account '" << entry.trackingAccount().first
         << "'as tracking account for transaction entry";
      die(entry.fileName(), entry.lineNum(), ss.str());
   }
}

void IPTransactionCategorizer::checkCreateOff(
      std::string const& account, std::string const& filename, size_t linenum)
{
   if (!m_accounts.count(account))
   {
      std::stringstream ss;
      ss << "Automatically opening off-budget account '" << account << "'";
      warn(filename, linenum, ss.str());
      m_accounts[account] = false;
   }
}

void IPTransactionCategorizer::checkCreateOn(
      std::string const& account, std::string const& filename, size_t linenum)
{
   if (!m_accounts.count(account))
   {
      std::stringstream ss;
      ss << "Automatically opening on-budget account '" << account << "'";
      warn(filename, linenum, ss.str());
      m_accounts[account] = true;
   }
}
