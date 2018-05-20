#ifndef NATIVETRANSACTIONENTRY_H
#define NATIVETRANSACTIONENTRY_H

#include <QString>

class NativeTransactionEntry
{
public:
   QString amount() const;
   void setAmount(QString const& amount);

   QString category() const;
   bool hasCategory() const;
   void setCategory(QString const& category);

   QString note() const;
   bool hasNote() const;
   void setNote(QString const& note);

   QString payee() const;
   void setPayee(QString const& payee);

private:
   QString m_amount;
   QString m_category;
   bool m_hasCategory = false;
   QString m_note;
   bool m_hasNote = false;
   QString m_payee;
};

#endif
