#ifndef LEDGERACCOUNTCOMMAND_H
#define LEDGERACCOUNTCOMMAND_H

#include <QDate>
#include <QString>
#include "ledgercommand.h"

class LedgerAccountCommand : public LedgerCommand
{
public:
   enum class Mode
   {
      ON_BUDGET,
      OFF_BUDGET,
      CLOSED
   };

public:
   LedgerAccountCommand(int seqNum, int lineNum);
   QString account() const;
   QDate date() const;
   Mode mode() const;
   void setAccount(QString const& account);
   void setDate(QDate const& date);
   void setMode(Mode mode);

private:
   QString m_account;
   QDate m_date;
   Mode m_mode = Mode::CLOSED;
};

#endif
