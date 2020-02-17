#include "csvreader.h"

#include <QFile>
#include "cashpiles.h"

CsvReader::CsvReader(QString const& fileName, QObject* parent) :
   QObject(parent),
   m_file(new QFile(fileName, this))
{
   m_record.fileName = fileName;
}

void CsvReader::closeFile()
{
   m_file->close();
}

bool CsvReader::hasContent()
{
   return !m_file->atEnd();
}

void CsvReader::openFile()
{
   if (!m_file->open(QIODevice::ReadOnly))
   {
      die(QString("Unable to open input file '%1'").arg(m_record.fileName));
   }
}

CsvReader::Record CsvReader::readRecord()
{
   QString line;
   while (hasContent() && (m_waitingForHeader ||
                           m_record.data.size() != m_header.size()))
   {
      line = m_file->readLine();
      if (!line.endsWith('\n'))
      {
         line.append('\n');
      }
      ++m_record.lineNum;
      foreach (QChar const c, line)
      {
         if (c == ',') parseComma();
         else if (c == '"') parseQuote();
         else if (c == '\\') parseBackslash();
         else if (c == '\n') parseNewline();
         else if (c >= ' ') parseChar(c);
      }
   }

   if (m_waitingForHeader)
   {
      die("Unable to read complete CSV header");
   }
   if (m_record.data.size() != 0 && m_record.data.size() != m_header.size())
   {
      die(m_record.fileName, m_record.lineNum,
          "Unable to read complete CSV record");
   }
   CsvReader::Record record(m_record);
   m_record.data.clear();
   return record;
}

void CsvReader::commitField()
{
   if (m_waitingForHeader)
   {
      m_header.append(m_fieldValue);
   }
   else
   {
      m_record.data[m_header[m_fieldIndex]] = m_fieldValue;
   }
   m_fieldValue.clear();
   ++m_fieldIndex;
}

void CsvReader::commitRecord()
{
   if (m_waitingForHeader)
   {
      m_waitingForHeader = false;
   }
   else if (m_record.data.size() != 0 &&
            m_record.data.size() != m_header.size())
   {
      die(m_record.fileName, m_record.lineNum,
          "The end of a line was found when more text was expected");
   }
   m_fieldIndex = 0;
}

void CsvReader::parseBackslash()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
      case FieldMode::NORMAL:
         m_fieldMode = FieldMode::NORMAL_ESCAPED;
         break;
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += '\\';
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::QUOTED:
         m_fieldMode = FieldMode::QUOTED_ESCAPED;
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += '\\';
         m_fieldMode = FieldMode::QUOTED;
         break;
      case FieldMode::CLOSED:
         die(m_record.fileName, m_record.lineNum,
             "An extra backslash was found when a comma was expected");
   }
}

void CsvReader::parseChar(QChar c)
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::NORMAL:
      case FieldMode::QUOTED:
         m_fieldValue += c;
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = FieldMode::QUOTED;
         break;
      case FieldMode::CLOSED:
         die(m_record.fileName, m_record.lineNum,
             "Extra text was found when a comma was expected");
   }
}

void CsvReader::parseComma()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
      case FieldMode::NORMAL:
      case FieldMode::CLOSED:
         commitField();
         m_fieldMode = FieldMode::EMPTY;
         break;
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += ',';
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::QUOTED:
         m_fieldValue += ',';
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += ',';
         m_fieldMode = FieldMode::QUOTED;
         break;
   }
}

void CsvReader::parseNewline()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
      case FieldMode::NORMAL:
      case FieldMode::CLOSED:
         commitField();
         commitRecord();
         m_fieldMode = FieldMode::EMPTY;
         break;
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += '\n';
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::QUOTED:
         m_fieldValue += '\n';
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += '\n';
         m_fieldMode = FieldMode::QUOTED;
         break;
   }
}

void CsvReader::parseQuote()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
         m_fieldMode = FieldMode::QUOTED;
         break;
      case FieldMode::NORMAL:
         m_fieldValue += '"';
         break;
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += '"';
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::QUOTED:
         m_fieldMode = FieldMode::CLOSED;
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += '"';
         m_fieldMode = FieldMode::QUOTED;
         break;
      case FieldMode::CLOSED:
         die(m_record.fileName, m_record.lineNum,
             "An extra quote was found when a comma was expected");
   }
}
