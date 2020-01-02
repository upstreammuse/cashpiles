#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>

class Interval;
class Ledger;
class QIODevice;

class FileReader : public QObject
{
   Q_OBJECT

public:
   FileReader(QString const& fileName, Ledger& ledger,
              QObject* parent = nullptr);
   bool readAll();
   void setDateFormat(QString const& dateFormat);

private:
   void processAccount(QRegularExpressionMatch const& match);
   void processBudget(QRegularExpressionMatch& match);
   void processComment(QRegularExpressionMatch const& match);
   void processCompactTransaction(QRegularExpressionMatch const& match);
   void processCompactTransactionOff(QRegularExpressionMatch const& match);
   void processLine(QString const& line);
   void processReserve(QRegularExpressionMatch const& match);
   void processTransaction(QRegularExpressionMatch& match);

   bool hasLines();
   QString readLine();
   void unReadLine(QString const& line);

private:
   void failed(QString const& message);
   void failedCurrency(QString const& currency);
   QDate parseDate(QString const& date);

private:
   QString m_dateFormat = "yyyy/MM/dd";
   QIODevice* m_file = nullptr;
   QString m_fileName;
   Ledger& m_ledger;
   uint m_lineNum = 0;
   QStringList m_lines;
   bool m_success = false;
};

#endif
