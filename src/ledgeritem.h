#pragma once

#include <memory>
#include <string>

class ItemProcessor;

class LedgerItem
{
public:
   int const id;

public:
   LedgerItem();
   // TODO this ctor will go away in preference to the in-situ error system
   LedgerItem(std::string const& fileName, size_t lineNum);
   virtual ~LedgerItem();
   std::string fileName() const;
   size_t lineNum() const;
   virtual void processItem(ItemProcessor& processor) const = 0;

private:
   static int sm_nextId;

private:
   std::string m_fileName;
   size_t m_lineNum = 0;
};
