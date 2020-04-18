#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

class CsvReader
{
public:
   struct Record
   {
      std::map<std::string, std::string> data;
      std::string fileName;
      size_t lineNum = 0;
   };

public:
   explicit CsvReader(std::string const& fileName);
   void closeFile();
   bool hasContent();
   bool openFile();
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
   void parseChar(char c);
   void parseComma();
   void parseNewline();
   void parseQuote();

private:
   size_t m_fieldIndex = 0;
   FieldMode m_fieldMode = FieldMode::EMPTY;
   std::string m_fieldValue;
   std::ifstream m_file;
   std::vector<std::string> m_header;
   Record m_record;
   bool m_waitingForHeader = true;
};
