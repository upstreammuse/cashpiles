#include "texttable.h"

#include <iomanip>

void TextTable::appendColumn(size_t index, std::string const& item)
{
   if (index < m_columns.size())
   {
      ColumnData& data = m_columns[index];
      data.content.push_back(item);
      data.width = std::max(item.size(), m_columns[index].width);
   }
   else
   {
      setColumn(index, std::vector<std::string>());
      appendColumn(index, item);
   }
}

void TextTable::clear()
{
   m_columns.clear();
}

void TextTable::print(std::ostream& out) const
{
   size_t height = 0;
   for (ColumnData const& data : m_columns)
   {
      height = std::max(data.content.size(), height);
   }

   for (size_t i = 0; i < height; ++i)
   {
      for (ColumnData const& data : m_columns)
      {
         if (i < data.content.size())
         {
            std::string const& item = data.content[i];
            switch (data.align)
            {
               case Alignment::LeftAlign:
                  out << std::left << std::setw(data.width) << item;
                  break;
               case Alignment::RightAlign:
                  out << std::right << std::setw(data.width) << item;
                  break;
            }
         }
         else
         {
            out << std::left << std::setw(data.width) << "";
         }
      }
      out << std::endl;
   }
}

void TextTable::setColumn(size_t index, std::vector<std::string> const& content,
                          Alignment align)
{
   ColumnData data;
   data.content = content;
   data.align = align;
   data.width = 0;
   for (std::string const& s : content)
   {
      data.width = std::max(s.size(), data.width);
   }
   m_columns.resize(std::max(m_columns.size(), index + 1));
   m_columns[index] = data;
}

void TextTable::setColumnAlignment(size_t index, Alignment align)
{
   if (index < m_columns.size())
   {
      m_columns[index].align = align;
   }
   else
   {
      setColumn(index, std::vector<std::string>(), align);
   }
}
