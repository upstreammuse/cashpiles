#ifndef LEDGERACCOUNTCOMMAND_H
#define LEDGERACCOUNTCOMMAND_H

#include <QDate>
#include <QString>
#include "ledgeritem.h"

class LedgerAccountCommand : public LedgerItem
{
public:
   enum class Mode
   {
      ON_BUDGET,
      OFF_BUDGET,
      CLOSED
   };

public:
   LedgerAccountCommand(QString const& filename, int lineNum);

   QString account() const;
   void setAccount(QString const& account);

   QDate date() const;
   void setDate(QDate const& date);

   Mode mode() const;
   void setMode(Mode mode);

   void processItem(ItemProcessor* processor);

private:
   QString m_account;
   QDate m_date;
   Mode m_mode = Mode::CLOSED;
};

#endif
