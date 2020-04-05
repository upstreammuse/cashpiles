#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include "ledgeraccount.h"

class Currency;
class Date;
class Interval;
class Ledger;
class QIODevice;

class FileReader : public QObject
{
   Q_OBJECT

public:
   static Currency parseCurrency(QString const& currency,
                                 QString const& fileName, uint lineNum);
   static Date parseDate(QString const& date, QString const& dateFormat,
                          QString const& fileName, uint lineNum);

public:
   FileReader(QString const& fileName, Ledger& ledger,
              QObject* parent = nullptr);
   void readAll();
   void setDateFormat(QString const& dateFormat);

private:
   void processAccount(QRegularExpressionMatch const& match);
   void processBlank();
   void processBudget(QRegularExpressionMatch& match);
   void processComment(QRegularExpressionMatch const& match);
   void processCompactReserve(QRegularExpressionMatch const& match);
   void processCompactTransaction(QRegularExpressionMatch const& match);
   void processCompactTransactionOff(QRegularExpressionMatch const& match);
   void processLine(QString const& line);
   void processReserve(QRegularExpressionMatch& match);
   void processTransaction(QRegularExpressionMatch& match);

   bool hasLines();
   QString readLine();
   void unReadLine(QString const& line);

private:
   Currency parseCurrency(QString const& currency);
   Date parseDate(QString const& date);
   Interval parseInterval(QString const& interval);
   LedgerAccount::Mode parseMode(QString const& mode);

private:
   QString m_dateFormat = "yyyy/MM/dd";
   QIODevice* m_file = nullptr;
   QString m_fileName;
   Ledger& m_ledger;
   uint m_lineNum = 0;
   QStringList m_lines;
};

#endif
