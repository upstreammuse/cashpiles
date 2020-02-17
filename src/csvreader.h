#pragma once

#include <QHash>
#include <QObject>

class QIODevice;

class CsvReader : public QObject
{
   Q_OBJECT

public:
   struct Record
   {
      QHash<QString, QString> data;
      QString fileName;
      uint lineNum = 0;
   };

public:
   explicit CsvReader(QString const& fileName, QObject* parent = nullptr);
   void closeFile();
   bool hasContent();
   void openFile();
   Record readRecord();

private:
   enum class FieldMode
   {
      EMPTY, NORMAL, NORMAL_ESCAPED, QUOTED, QUOTED_ESCAPED, CLOSED
   };

private:
   void commitField();
   void commitRecord();
   void parseBackslash();
   void parseChar(QChar c);
   void parseComma();
   void parseNewline();
   void parseQuote();

private:
   int m_fieldIndex = 0;
   FieldMode m_fieldMode = FieldMode::EMPTY;
   QString m_fieldValue;
   QIODevice* m_file = nullptr;
   QStringList m_header;
   Record m_record;
   bool m_waitingForHeader = true;
};
