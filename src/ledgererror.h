#pragma once

#include <map>
#include <vector>
#include "ledgeritem.h"

struct Location
{
   std::string filename;
   size_t linenum;
   size_t pos;

   bool operator<(Location const& other) const
   {
      return filename < other.filename ||
            (filename == other.filename && linenum < other.linenum) ||
            (filename == other.filename && linenum == other.linenum &&
             pos < other.pos);
   }
};

class LedgerError : public LedgerItem
{
public:
   bool hasMessages() const;
   void insertContent(std::string const& line, Location const& loc);
   void insertMessage(std::string const& message, Location const& loc);
   std::vector<std::string> lines() const;

   void processItem(ItemProcessor& processor) const;

private:
   std::multimap<Location, std::string> m_content;
   std::multimap<Location, std::string> m_messages;
};
