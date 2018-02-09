#include "account.h"

Account::Account() :
   m_allClear(true),
   m_balance(0),
   // TODO use a real file name or registry
   m_settings(new QSettings("~/cashpiles.ini", QSettings::IniFormat))
{
}

void Account::appendTransaction(Transaction const& transaction)
{
   m_allClear = m_allClear && transaction.cleared();
   m_balance += transaction.amount();
   verifyName(transaction.account());
   verifyDate(transaction.date());
   verifyBalance(transaction.balance(), transaction.hasBalance());
   foreach (TransactionSplit const& split, transaction.splits())
   {
      if (split.isTransfer())
      {
         m_transfers[split.payee()] += split.amount();
         m_settings->beginGroup("Accounts");
         m_settings->beginGroup(split.payee());
         bool onBudget = m_settings->value("onBudget", true).toBool();
         m_settings->setValue("onBudget", onBudget);
         m_settings->endGroup();
         m_settings->endGroup();
         if (!m_onBudget && split.category() != "")
         {
            qWarning("category set on transfer from off-budget account");
         }
         else if (m_onBudget && onBudget && split.category() != "")
         {
            qWarning("category set on transfer between budget accounts");
         }
         else if (m_onBudget && !onBudget && split.category() == "")
         {
            qWarning("category not set on transfer to off-budget account");
         }
      }
   }
}

void Account::matchTransfers(QMap<QString, Account> const& accounts) const
{
   for (QHash<QString, int>::const_iterator it(m_transfers.begin());
        it != m_transfers.end(); ++it)
   {
      if (accounts[it.key()].m_transfers[m_name] + it.value() != 0)
      {
         qWarning("transfer mismatch between %s and %s", it.key().toStdString().c_str(), m_name.toStdString().c_str());
      }
   }
}

void Account::verifyBalance(int balance, bool hasBalance)
{
   if (hasBalance)
   {
      if (!m_allClear)
      {
         qWarning("uncleared transactions before a balance assertion");
      }
      if (balance != m_balance)
      {
         qWarning("balance assertion failed");
      }
   }
}

void Account::verifyDate(QDate const& date)
{
   if (m_latestDate.isNull())
   {
      m_latestDate = date;
   }
   if (date < m_latestDate)
   {
      qWarning("transaction dates are out of order");
   }
   m_latestDate = date;
}

void Account::verifyName(QString const& name)
{
   if (m_name.isNull())
   {
      m_name = name;
   }
   Q_ASSERT(m_name == name);

   m_settings->beginGroup("Accounts");
   m_settings->beginGroup(m_name);
   m_onBudget = m_settings->value("onBudget", true).toBool();
   m_settings->setValue("onBudget", m_onBudget);
   m_settings->endGroup();
   m_settings->endGroup();
}
