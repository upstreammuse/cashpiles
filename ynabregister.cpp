#include "ynabregister.h"

#include "transaction.h"

YnabRegister::YnabRegister(QObject* parent) :
   QObject(parent)
{
}

void YnabRegister::appendRecord(QHash<QString, QString> const& record)
{
   for (QHash<QString, QString>::const_iterator it(record.begin());
        it != record.end(); ++it)
   {
      if (it.key() == "Account")
      {
         m_accountColumn.append(it.value());
      }
      else if (it.key() == "Category")
      {
         m_categoryColumn.append(it.value());
      }
      else if (it.key() == "Cleared")
      {
         if (it.value() == "Uncleared")
         {
            m_clearedColumn.append(UNCLEARED);
         }
         else if (it.value() == "Cleared")
         {
            m_clearedColumn.append(CLEARED);
         }
         else if (it.value() == "Reconciled")
         {
            m_clearedColumn.append(RECONCILED);
         }
      }
      else if (it.key() == "Date")
      {
         m_dateColumn.append(QDate::fromString(it.value(), "MM/dd/yyyy"));
      }
      else if (it.key() == "Inflow")
      {
         QString fieldCopy(it.value());
         m_inflowColumn.append(
                  fieldCopy.replace('$', "").replace(',', "").replace('.', "")
                  .toInt());
      }
      else if (it.key() == "Memo")
      {
         m_memoColumn.append(it.value());
      }
      else if (it.key() == "Outflow")
      {
         QString fieldCopy(it.value());
         m_outflowColumn.append(
                  fieldCopy.replace('$', "").replace(',', "").replace('.', "")
                  .toInt());
      }
      else if (it.key() == "Payee")
      {
         m_payeeColumn.append(it.value());
      }
   }
   Q_ASSERT(m_accountColumn.size() == m_categoryColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_clearedColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_dateColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_inflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_memoColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_outflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_payeeColumn.size());
}

void YnabRegister::showTrans()
{
   QMultiMap<QDate, Transaction> transactions;
   Transaction* t = new Transaction;
   for (int i = 0; i < m_accountColumn.size(); ++i)
   {
      t->setDate(m_dateColumn[i]);
      t->setAccount(m_accountColumn[i]);
      t->setCleared(m_clearedColumn[i] == CLEARED ||
                    m_clearedColumn[i] == RECONCILED);

      QRegExp rx("Split \\(([0-9]*)/([0-9]*)\\) (.*)");
      bool inSplit = false;
      QString memo;
      if (rx.exactMatch(m_memoColumn[i]))
      {
         memo = rx.cap(3);
         size_t splitIndex = rx.cap(1).toInt();
         size_t splitCount = rx.cap(2).toInt();
         inSplit = splitIndex != splitCount;
      }
      else
      {
         memo = m_memoColumn[i];
      }

      QRegExp rx2("Transfer : (.*)");
      QString payee;
      bool isTransfer(false);
      if (rx2.exactMatch(m_payeeColumn[i]))
      {
         payee = rx2.cap(1);
         isTransfer = true;
      }
      else
      {
         payee = m_payeeColumn[i];
      }

      // TODO ynab has a problem with their export format that leaves split transfers inheriting their parent payee "sometimes"
      // ye gods, what fresh hell... should I file a bug report?
      // --- oh good, it looks like the latest CSV export fixed it
      // ..... time for bed!
      {
         static int asdf = 0;
         if (t->account() == "Checking" && isTransfer && payee == "Cash")
         {
            asdf += m_inflowColumn[i] - m_outflowColumn[i];
            qWarning("on %s: checking to cash %d, difference %d", t->date().toString().toStdString().c_str(), m_inflowColumn[i] - m_outflowColumn[i], asdf);
         }
         if (t->account() == "Cash" && isTransfer && payee == "Checking")
         {
            asdf += m_inflowColumn[i] - m_outflowColumn[i];
            qWarning("on %s: cash to checking %d, difference %d", t->date().toString().toStdString().c_str(), m_inflowColumn[i] - m_outflowColumn[i], asdf);
         }
      }

      t->appendSplit(TransactionSplit(
                        payee, m_categoryColumn[i], memo,
                        m_inflowColumn[i] - m_outflowColumn[i], isTransfer));

      if (!inSplit)
      {
         transactions.insertMulti(t->date(), *t);
         delete t;
         t = new Transaction;
      }
   }

   foreach (Transaction const& t, transactions)
   {
      emit transaction(t);
   }
   emit finished();
}
