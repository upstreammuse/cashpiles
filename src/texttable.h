#pragma once

#include <string>
#include <vector>

class TextTable
{
public:
   enum class Alignment
   {
      LeftAlign,
      RightAlign
   };

public:
   void appendColumn(size_t index, std::string const& item);
   void clear();
   void print(std::ostream& out) const;
   void setColumn(size_t index, std::vector<std::string> const& content,
                  Alignment align = Alignment::LeftAlign);
   void setColumnAlignment(size_t index, Alignment align);

private:
   struct ColumnData
   {
      std::vector<std::string> content;
      Alignment align;
      size_t width;
   };

private:
   std::vector<ColumnData> m_columns;
};
