#pragma once

#include <string>

class ItemProcessor;

class LedgerItem
{
public:
   LedgerItem(std::string const& fileName, size_t lineNum);
   virtual ~LedgerItem();
   std::string fileName() const;
   size_t lineNum() const;
   std::pair<std::string, bool> note() const;
   virtual void processItem(ItemProcessor& processor) const = 0;
   void setNote(std::string const& note);

private:
   std::string m_fileName;
   size_t m_lineNum = 0;
   std::pair<std::string, bool> m_note = std::make_pair("", false);
};
