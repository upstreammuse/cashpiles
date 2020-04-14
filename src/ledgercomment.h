#ifndef LEDGERCOMMENT_H
#define LEDGERCOMMENT_H

#include "ledgeritem.h"

class LedgerComment : public LedgerItem
{
public:
   LedgerComment(std::string const& filename, size_t lineNum);
   std::string note() const;
   void processItem(ItemProcessor& processor) const;
   void setNote(std::string const& note);

private:
   std::string m_note;
};

#endif
