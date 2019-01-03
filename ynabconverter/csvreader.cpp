// Updated 12/11 for refactor

#include "csvreader.h"

CsvReader::CsvReader(QString const& fileName, QObject* parent) :
   QObject(parent),
   m_file(new QFile(fileName, this)),
   m_fileName(fileName)
{
}

void CsvReader::readAll()
{
   if (!m_file->open(QIODevice::ReadOnly))
   {
      error("Could not open the CSV file", false);
      emit finished();
      return;
   }

   QByteArray line;
   while (!(line = m_file->readLine()).isEmpty())
   {
      ++m_lineNum;
      foreach (char const c, line)
      {
         bool ok = true;
         if (c == ',') parseComma();
         else if (c == '"') ok = parseQuote();
         else if (c == '\\') ok = parseBackslash();
         else if (c == '\n') ok = parseNewline();
         else if (c >= ' ') ok = parseChar(c);
         if (!ok)
         {
            emit finished();
            return;
         }
      }
   }

   m_file->close();
   emit finished();
}

void CsvReader::commitField()
{
   if (m_waitingForHeader)
   {
      m_header.append(m_fieldValue);
   }
   else
   {
      m_record[m_header[m_fieldIndex]] = m_fieldValue;
   }
   m_fieldValue.clear();
   ++m_fieldIndex;
}

bool CsvReader::commitRecord()
{
   if (m_waitingForHeader)
   {
      m_waitingForHeader = false;
   }
   else if (m_header.size() == m_fieldIndex)
   {
      emit record(m_record, m_fileName, m_lineNum);
      m_record.clear();
   }
   else
   {
      error("The end of a line was found when more text was expected");
      return false;
   }
   m_fieldIndex = 0;
   return true;
}

void CsvReader::error(QString const& message_, bool withLineNum)
{
   QString msg;
   if (withLineNum)
   {
      emit message(QString("Error in CSV file '%1', line %2: %3")
                   .arg(m_fileName)
                   .arg(m_lineNum)
                   .arg(message_));
   }
   else
   {
      emit message(QString("Error in CSV file '%1': %2")
                   .arg(m_fileName)
                   .arg(message_));
   }
}

bool CsvReader::parseBackslash()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
         m_fieldMode = FieldMode::NORMAL_ESCAPED;
         break;
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
         error("An extra backslash was found when a comma was expected");
         return false;
   }
   return true;
}

bool CsvReader::parseChar(char c)
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
         m_fieldValue += c;
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::NORMAL:
      case FieldMode::QUOTED:
         m_fieldValue += c;
         break;
      case FieldMode::NORMAL_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = FieldMode::NORMAL;
         break;
      case FieldMode::QUOTED_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = FieldMode::QUOTED;
         break;
      case FieldMode::CLOSED:
         error("Extra text was found when a comma was expected");
         return false;
   }
   return true;
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

bool CsvReader::parseNewline()
{
   switch (m_fieldMode)
   {
      case FieldMode::EMPTY:
      case FieldMode::NORMAL:
      case FieldMode::CLOSED:
         commitField();
         if (!commitRecord())
         {
            return false;
         }
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
   return true;
}

bool CsvReader::parseQuote()
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
         error("An extra quote was found when a comma was expected");
         return false;
   }
   return true;
}
