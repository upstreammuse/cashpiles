// Updated 12/11 for refactor

#include "csvreader.h"

#include <iostream>

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

void CsvReader::error(QString const& message, bool withLineNum)
{
   std::cerr << "Error in CSV file '" << qPrintable(m_fileName) << "'";
   if (withLineNum)
   {
      std::cerr << ", line " << m_lineNum;
   }
   std::cerr << ":  " << qPrintable(message) << std::endl;;
}

bool CsvReader::parseBackslash()
{
   switch (m_fieldMode)
   {
      case EMPTY:
         m_fieldMode = NORMAL_ESCAPED;
         break;
      case NORMAL:
         m_fieldMode = NORMAL_ESCAPED;
         break;
      case NORMAL_ESCAPED:
         m_fieldValue += '\\';
         m_fieldMode = NORMAL;
         break;
      case QUOTED:
         m_fieldMode = QUOTED_ESCAPED;
         break;
      case QUOTED_ESCAPED:
         m_fieldValue += '\\';
         m_fieldMode = QUOTED;
         break;
      case CLOSED:
         error("An extra backslash was found when a comma was expected");
         return false;
   }
   return true;
}

bool CsvReader::parseChar(char c)
{
   switch (m_fieldMode)
   {
      case EMPTY:
         m_fieldValue += c;
         m_fieldMode = NORMAL;
         break;
      case NORMAL:
      case QUOTED:
         m_fieldValue += c;
         break;
      case NORMAL_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = NORMAL;
         break;
      case QUOTED_ESCAPED:
         m_fieldValue += c;
         m_fieldMode = QUOTED;
         break;
      case CLOSED:
         error("Extra text was found when a comma was expected");
         return false;
   }
   return true;
}

void CsvReader::parseComma()
{
   switch (m_fieldMode)
   {
      case EMPTY:
      case NORMAL:
      case CLOSED:
         commitField();
         m_fieldMode = EMPTY;
         break;
      case NORMAL_ESCAPED:
         m_fieldValue += ',';
         m_fieldMode = NORMAL;
         break;
      case QUOTED:
         m_fieldValue += ',';
         break;
      case QUOTED_ESCAPED:
         m_fieldValue += ',';
         m_fieldMode = QUOTED;
         break;
   }
}

bool CsvReader::parseNewline()
{
   switch (m_fieldMode)
   {
      case EMPTY:
      case NORMAL:
      case CLOSED:
         commitField();
         if (!commitRecord())
         {
            return false;
         }
         m_fieldMode = EMPTY;
         break;
      case NORMAL_ESCAPED:
         m_fieldValue += '\n';
         m_fieldMode = NORMAL;
         break;
      case QUOTED:
         m_fieldValue += '\n';
         break;
      case QUOTED_ESCAPED:
         m_fieldValue += '\n';
         m_fieldMode = QUOTED;
         break;
   }
   return true;
}

bool CsvReader::parseQuote()
{
   switch (m_fieldMode)
   {
      case EMPTY:
         m_fieldMode = QUOTED;
         break;
      case NORMAL:
         m_fieldValue += '"';
         break;
      case NORMAL_ESCAPED:
         m_fieldValue += '"';
         m_fieldMode = NORMAL;
         break;
      case QUOTED:
         m_fieldMode = CLOSED;
         break;
      case QUOTED_ESCAPED:
         m_fieldValue += '"';
         m_fieldMode = QUOTED;
         break;
      case CLOSED:
         error("An extra quote was found when a comma was expected");
         return false;
   }
   return true;
}
