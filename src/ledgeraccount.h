#ifndef LEDGERACCOUNT_H
#define LEDGERACCOUNT_H

#include <QDate>
#include <QString>
#include "currency.h"
#include "ledgeritem.h"

class LedgerAccount : public LedgerItem
{
public:
   enum class Mode
   {
      ON_BUDGET,
      OFF_BUDGET,
      CLOSED
   };
   static Mode modeFromString(QString const& mode, bool* ok);
   static QString modeToString(Mode mode);

public:
   LedgerAccount(QString const& filename, uint lineNum);
   QDate date() const;
   Mode mode() const;
   QString name() const;
   void processItem(ItemProcessor& processor) const;
   void setDate(QDate const& date);
   void setMode(Mode mode);
   void setName(QString const& account);

private:
   QDate m_date;
   Mode m_mode = Mode::CLOSED;
   QString m_name;
};

#endif
