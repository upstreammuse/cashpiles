#include "csvreader.h"

#include <sstream>
#include "cashpiles.h"

CsvReader::CsvReader(std::string const& fileName)
{
   m_record.fileName = fileName;
}

void CsvReader::closeFile()
{
   m_file.close();
}

bool CsvReader::hasContent()
{
   return !m_file.eof();
}

bool CsvReader::openFile()
{
   m_file.open(m_record.fileName);
   return bool(m_file);
}

CsvReader::Record CsvReader::readRecord()
{
   std::string line;
   while (hasContent() && (m_waitingForHeader ||
                           m_record.data.size() != m_header.size()))
   {
      getline(m_file, line);
      if (line.back() != '\n')
      {
         line += '\n';
      }
      ++m_record.lineNum;
      for (char const c : line)
      {
         if (c == ',') parseComma();
         else if (c == '"') parseQuote();
         else if (c == '\\') parseBackslash();
         else if (c == '\n') parseNewline();
         else if (c >= ' ') parseChar(c);
      }
   }

   CsvReader::Record record(m_record);
   m_record.data.clear();
   return record;
}

void CsvReader::commitField()
{
   if (m_waitingForHeader)
   {
      m_header.push_back(m_fieldValue);
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
         m_fieldValue += '"';
         m_fieldMode = FieldMode::NORMAL_ESCAPED;
         break;
   }
}

void CsvReader::parseChar(char c)
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
         m_fieldValue += '"';
         m_fieldValue += c;
         m_fieldMode = FieldMode::NORMAL;
         break;
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
         m_fieldValue += '"';
         m_fieldMode = FieldMode::QUOTED;
         break;
   }
}
