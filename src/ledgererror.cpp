#include "ledgererror.h"

#include "itemprocessor.h"

bool LedgerError::hasMessages() const
{
   return m_messages.size();
}

void LedgerError::insertContent(std::string const& line, Location const& loc)
{
   m_content.insert(std::make_pair(loc, line));
}

void LedgerError::insertMessage(std::string const& message, Location const& loc)
{
   m_messages.insert(std::make_pair(loc, message));
}

std::vector<std::string> LedgerError::lines() const
{
   std::vector<std::string> retval;
   auto it1 = m_content.cbegin();
   auto it2 = m_messages.cbegin();
   while (it1 != m_content.cend() || it2 != m_messages.cend())
   {
      // TODO there should be a better way to write this
      if (it1 == m_content.cend() && it2 != m_messages.cend())
      {
         retval.push_back(
                  std::string(it2->first.pos, ' ') + "^ " + it2->second);
         ++it2;
      }
      else if (it1 != m_content.cend() && it2 == m_messages.cend())
      {
         retval.push_back(it1->second);
         ++it1;
      }
      else if (it2->first < it1->first)
      {
         retval.push_back(
                  std::string(it2->first.pos, ' ') + "^ " + it2->second);
         ++it2;
      }
      else
      {
         retval.push_back(it1->second);
         ++it1;
      }
   }
   return retval;
}

void LedgerError::processItem(ItemProcessor& processor) const
{
   processor.processItem(*this);
}
