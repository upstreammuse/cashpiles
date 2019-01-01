// Updated 12/11 for refactor

#ifndef CSVREADER_H
#define CSVREADER_H

#include <QFile>
#include <QHash>

class CsvReader : public QObject
{
   Q_OBJECT

public:
   explicit CsvReader(QString const& fileName, QObject* parent = nullptr);

public slots:
   void readAll();

signals:
   void finished();
   void record(QHash<QString, QString> const&, QString const&, int);

private:
   enum class FieldMode
   {
      EMPTY, NORMAL, NORMAL_ESCAPED, QUOTED, QUOTED_ESCAPED, CLOSED
   };

private:
   void commitField();
   bool commitRecord();
   void error(QString const& message, bool withLineNum = true);
   bool parseBackslash();
   bool parseChar(char c);
   void parseComma();
   bool parseNewline();
   bool parseQuote();

private:
   int m_fieldIndex = 0;
   FieldMode m_fieldMode = FieldMode::EMPTY;
   QString m_fieldValue;
   QIODevice* m_file = nullptr;
   QString m_fileName;
   QStringList m_header;
   int m_lineNum = 0;
   QHash<QString, QString> m_record;
   bool m_waitingForHeader = true;
};

#endif
