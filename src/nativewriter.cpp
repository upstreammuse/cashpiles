#include "nativewriter.h"

#include <iostream>
#include "ledgeraccountcommand.h"
#include "ledgerbudget.h"
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

void NativeWriter::processItem(LedgerBudget const& budget)
{
   breakBetween();
   std::cout << qPrintable(budget.date().toString(Qt::SystemLocaleShortDate))
             << " budget " << qPrintable(budget.interval().toString())
             << std::endl;
   for (auto it = budget.categories().cbegin();
        it != budget.categories().cend(); ++it)
   {
      std::cout << "  ";
      switch (it.value().type)
      {
         case BudgetCategory::Type::GOAL:
            std::cout << "goal   ";
            break;
         case BudgetCategory::Type::INCOME:
            std::cout << "income ";
            break;
         case BudgetCategory::Type::RESERVE_AMOUNT:
         case BudgetCategory::Type::RESERVE_PERCENT:
            std::cout << "reserve";
            break;
         case BudgetCategory::Type::ROUTINE:
            std::cout << "routine";
            break;
      }
      std::cout << " " << qPrintable(it.key());
      switch (it.value().type)
      {
         case BudgetCategory::Type::GOAL:
         case BudgetCategory::Type::INCOME:
         case BudgetCategory::Type::ROUTINE:
            break;
         case BudgetCategory::Type::RESERVE_AMOUNT:
            std::cout << " " << qPrintable(it.value().amount.toString()) << " "
                      << qPrintable(it.value().interval.toString());
            break;
         case BudgetCategory::Type::RESERVE_PERCENT:
            std::cout << " " << it.value().percentage << "%";
            break;
      }
      std::cout << std::endl;
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
   std::cout << qPrintable(transaction.date().toString(Qt::SystemLocaleShortDate));
   if (transaction.cleared())
   {
      std::cout << " * ";
   }
   else
   {
      std::cout << " ! ";
   }
   std::cout << qPrintable(transaction.account()) << "  ";
   int numNotes = transaction.hasNote() ? 1 : 0;
   foreach (LedgerTransactionEntry const& entry, transaction.entries())
   {
      if (entry.hasNote())
      {
         numNotes++;
      }
   }
   if (transaction.entries().size() > 1 || numNotes > 1)
   {
      std::cout << qPrintable(transaction.amount().toString());
      if (transaction.hasBalance())
      {
         std::cout << " = " << qPrintable(transaction.balance().toString());
      }
      if (transaction.hasNote())
      {
         std::cout << " ;" << qPrintable(transaction.note());
      }
      std::cout << std::endl;
      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         std::cout << "   ";
         if (entry.transfer())
         {
            std::cout << "@";
         }
         std::cout << qPrintable(entry.payee()) << "  ";
         if (entry.hasCategory())
         {
            std::cout << qPrintable(entry.category()) << "  ";
         }
         std::cout << qPrintable(entry.amount().toString());
         if (entry.hasNote())
         {
            std::cout << " ;" << qPrintable(entry.note());
         }
         std::cout << std::endl;
      }
   }
   else
   {
      LedgerTransactionEntry entry(transaction.entries()[0]);
      if (entry.transfer())
      {
         std::cout << "@";
      }
      std::cout << qPrintable(entry.payee());
      if (entry.hasCategory())
      {
         std::cout << "  " << qPrintable(entry.category());
      }
      std::cout << "  " << qPrintable(transaction.amount().toString());
      if (transaction.hasBalance())
      {
         std::cout << " = " << qPrintable(transaction.balance().toString());
      }
      if (transaction.hasNote())
      {
         std::cout << " ;" << qPrintable(transaction.note());
      }
      else if (entry.hasNote())
      {
         std::cout << " ;" << qPrintable(entry.note());
      }
      std::cout << std::endl;
   }
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
