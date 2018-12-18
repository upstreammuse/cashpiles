#include "ynabregisterreader.h"

YnabRegisterReader::YnabRegisterReader(QObject* parent) :
   QObject(parent)
{
}

void YnabRegisterReader::processRecord(QHash<QString, QString> const& record,
                                       QString const& filename, int lineNum)
{
   m_filenameColumn.append(filename);
   m_lineNumColumn.append(lineNum);
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
         m_inflowColumn.append(Currency(it.value(), lineNum));
      }
      else if (it.key() == "Memo")
      {
         m_memoColumn.append(it.value());
      }
      else if (it.key() == "Outflow")
      {
         m_outflowColumn.append(Currency(it.value(), lineNum));
      }
      else if (it.key() == "Payee")
      {
         m_payeeColumn.append(it.value());
      }
   }
   Q_ASSERT(m_accountColumn.size() == m_categoryColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_clearedColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_dateColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_filenameColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_inflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_lineNumColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_memoColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_outflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_payeeColumn.size());
}

void YnabRegisterReader::stop()
{
   LedgerTransaction* t = nullptr;
   for (int i = 0; i < m_accountColumn.size(); ++i)
   {
      if (!t)
      {
         t = new LedgerTransaction(m_filenameColumn[i], m_lineNumColumn[i]);
      }
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

      LedgerTransactionEntry entry;
      entry.setAmount(m_inflowColumn[i] - m_outflowColumn[i]);
      if (m_categoryColumn[i] != "")
      {
         entry.setCategory(m_categoryColumn[i]);
      }
      if (memo != "")
      {
         entry.setNote(memo);
      }
      entry.setPayee(payee);
      entry.setTransfer(isTransfer);
      t->appendEntry(entry);

      if (!inSplit)
      {
         emit item(t->date(), t);
         t = nullptr;
      }
   }
   emit finished();
}