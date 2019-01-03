#include "nativewriter.h"

#include <QTextStream>
#include "kernel/ledgeraccountcommand.h"
#include "kernel/ledgerbudget.h"
#include "kernel/ledgercomment.h"
#include "kernel/ledgertransaction.h"

NativeWriter::NativeWriter(QObject* parent) :
   ItemProcessor(parent)
{
}

void NativeWriter::processItem(LedgerAccountCommand const& account)
{
   breakBetween();
   QTextStream out(stdout);
   out << account.date().toString(Qt::SystemLocaleShortDate) << " ";
   switch (account.mode())
   {
      case LedgerAccountCommand::Mode::CLOSED:
         out << "closed";
         break;
      case LedgerAccountCommand::Mode::OFF_BUDGET:
         out << "off-budget";
         break;
      case LedgerAccountCommand::Mode::ON_BUDGET:
         out << "on-budget";
         break;
   }
   out << " " << account.account() << endl;
}

void NativeWriter::processItem(LedgerBudget const& budget)
{
   breakBetween();
   QTextStream out(stdout);
   out << budget.date().toString(Qt::SystemLocaleShortDate) << " budget "
       << budget.interval().toString() << endl;
   for (auto it = budget.categories().cbegin();
        it != budget.categories().cend(); ++it)
   {
      out << "  ";
      switch (it->type)
      {
         case LedgerBudget::Category::Type::GOAL:
            out << "goal   ";
            break;
         case LedgerBudget::Category::Type::INCOME:
            out << "income ";
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            out << "reserve";
            break;
         case LedgerBudget::Category::Type::ROUTINE:
            out << "routine";
            break;
      }
      out << " " << it.key();
      switch (it->type)
      {
         case LedgerBudget::Category::Type::GOAL:
         case LedgerBudget::Category::Type::INCOME:
         case LedgerBudget::Category::Type::ROUTINE:
            break;
         case LedgerBudget::Category::Type::RESERVE_AMOUNT:
            out << " " << it->amount.toString() << " "
                << it->interval.toString();
            break;
         case LedgerBudget::Category::Type::RESERVE_PERCENT:
            out << " " << it->percentage << "%";
            break;
      }
      out << endl;
   }
}

void NativeWriter::processItem(LedgerComment const& comment)
{
   breakBetween();
   QTextStream out(stdout);
   out << ";" << comment.note() << endl;
}

void NativeWriter::processItem(LedgerTransaction const& transaction)
{
   breakBetween();
   QTextStream out(stdout);
   out << transaction.date().toString(Qt::SystemLocaleShortDate);
   if (transaction.cleared())
   {
      out << " * ";
   }
   else
   {
      out << " ! ";
   }
   out << transaction.account() << "  ";
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
      out << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         out << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         out << " ;" << transaction.note();
      }
      out << endl;
      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         out << "   ";
         if (entry.transfer())
         {
            out << "@";
         }
         out << entry.payee() << "  ";
         if (entry.hasCategory())
         {
            out << entry.category() << "  ";
         }
         out << entry.amount().toString();
         if (entry.hasNote())
         {
            out << " ;" << entry.note();
         }
         out << endl;
      }
   }
   else
   {
      LedgerTransactionEntry entry(transaction.entries()[0]);
      if (entry.transfer())
      {
         out << "@";
      }
      out << entry.payee();
      if (entry.hasCategory())
      {
         out << "  " << entry.category();
      }
      out << "  " << transaction.amount().toString();
      if (transaction.hasBalance())
      {
         out << " = " << transaction.balance().toString();
      }
      if (transaction.hasNote())
      {
         out << " ;" << transaction.note();
      }
      else if (entry.hasNote())
      {
         out << " ;" << entry.note();
      }
      out << endl;
   }
}

void NativeWriter::stop()
{
   // TODO nothing for now, but...
   // fancy version should cache the input, measure the lengths of the resulting strings, and then write to make everything line up nicely
}

void NativeWriter::breakBetween()
{
   QTextStream out(stdout);
   if (m_firstItem)
   {
      m_firstItem = false;
   }
   else
   {
      out << endl;
   }
}
