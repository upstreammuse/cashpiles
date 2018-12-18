#include "nativewriter.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerbudgetallocation.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"

NativeWriter::NativeWriter(QObject* parent) :
   ItemProcessor(parent)
{
}

void NativeWriter::processItem(LedgerAccountCommand const& account)
{
   breakBetween();
   std::cout << qPrintable(account.date().toString(Qt::SystemLocaleShortDate))
             << " ";
   switch (account.mode())
   {
      case LedgerAccountCommand::Mode::CLOSED:
         std::cout << "closed";
         break;
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         std::cout << "off-budget";
         break;
      case LedgerAccountCommand::Mode::ON_BUDGET:
         std::cout << "on-budget";
         break;
   }
   std::cout << " " << qPrintable(account.account()) << std::endl;
}

void NativeWriter::processItem(LedgerBudgetAllocation const& allocation)
{
   breakBetween();
   std::cout << qPrintable(allocation.date().toString(Qt::SystemLocaleShortDate))
             << " budget" << std::endl;
   for (auto it(allocation.allocations().cbegin());
        it != allocation.allocations().cend(); ++it)
   {
      std::cout << "   " << qPrintable(it.key()) << "  "
                << qPrintable(it.value().toString()) << std::endl;
   }
}

void NativeWriter::processItem(LedgerComment const& comment)
{
   breakBetween();
   std::cout << ";" << qPrintable(comment.note()) << std::endl;
}

void NativeWriter::processItem(LedgerTransaction const& transaction)
{
   breakBetween();
   std::cout << qPrintable(transaction.date().toString(Qt::SystemLocaleShortDate))
             << std::endl;
}

void NativeWriter::stop()
{
   // TODO nothing for now, but...
   // fancy version should cache the input, measure the lengths of the resulting strings, and then write to make everything line up nicely
}

void NativeWriter::breakBetween()
{
   if (m_firstItem)
   {
      m_firstItem = false;
   }
   else
   {
      std::cout << std::endl;
   }
}
