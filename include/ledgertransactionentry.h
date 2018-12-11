#ifndef LEDGERTRANSACTIONENTRY_H
#define LEDGERTRANSACTIONENTRY_H

#include <QString>

class LedgerTransactionEntry
{
public:
   int amount() const;
   void setAmount(int const& amount);

   QString category() const;
   bool hasCategory() const;
   void setCategory(QString const& category);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   QString payee() const;
   void setPayee(QString const& payee);

   bool transfer() const;
   void setTransfer(bool transfer);

private:
   int m_amount = 0;
   QString m_category;
   bool m_hasCategory = false;
   QString m_note;
   bool m_hasNote = false;
   QString m_payee;
   bool m_transfer = false;
};

#endif
