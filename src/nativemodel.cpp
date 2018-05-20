#include "nativemodel.h"

#include <QLocale>

void NativeModel::parseAccount(NativeAccountCommand const& accountCommand)
{
   LedgerAccountCommand command(accountCommand.seqNum(),
                                accountCommand.lineNum());
   command.setAccount(accountCommand.account());
   command.setDate(parseDate(accountCommand.date(), accountCommand.lineNum()));
   if (accountCommand.command() == "on-budget")
   {
      command.setMode(LedgerAccountCommand::Mode::ON_BUDGET);
      emit accountChanged(command);
   }
   else if (accountCommand.command() == "off-budget")
   {
      command.setMode(LedgerAccountCommand::Mode::OFF_BUDGET);
      emit accountChanged(command);
   }
   else if (accountCommand.command() == "close")
   {
      command.setMode(LedgerAccountCommand::Mode::CLOSED);
      emit accountChanged(command);
   }
   else
   {
      qWarning("Unknown account command '%s', line %d",
               qPrintable(accountCommand.command()), accountCommand.lineNum());
   }
}

void NativeModel::parseBudget(NativeBudgetCommand const& budgetCommand)
{
   LedgerBudgetAllocation allocation(budgetCommand.seqNum(),
                                     budgetCommand.lineNum());
   allocation.setDate(parseDate(budgetCommand.date(), budgetCommand.lineNum()));
   for (int i(0); i < budgetCommand.allocations(); ++i)
   {
      allocation.appendAllocation(budgetCommand.category(i),
                                  parseCurrency(budgetCommand.amount(i),
                                                budgetCommand.lineNum()));
   }
   emit budgetAllocated(allocation);
}

void NativeModel::parseComment(NativeComment const& comment)
{
   LedgerComment comment_(comment.seqNum(), comment.lineNum());
   comment_.setNote(comment.note());
   emit commented(comment_);
}

void NativeModel::parseTransaction(NativeTransaction const& transaction)
{
   LedgerTransaction xact(transaction.seqNum(), transaction.lineNum());
   xact.setAccount(transaction.account());
   if (transaction.hasBalance())
   {
      xact.setBalance(
               parseCurrency(transaction.balance(), transaction.lineNum()));
   }
   xact.setCleared(transaction.cleared() == "*");
   xact.setDate(parseDate(transaction.date(), transaction.lineNum()));
   if (transaction.hasNote())
   {
      xact.setNote(transaction.note());
   }
   int total = parseCurrency(transaction.amount(), transaction.lineNum());

   foreach (NativeTransactionEntry const& entry, transaction.entries())
   {
      LedgerTransactionEntry entry_;
      int amount = parseCurrency(entry.amount(), transaction.lineNum());
      entry_.setAmount(amount);
      total -= amount;
      if (entry.hasCategory())
      {
         entry_.setCategory(entry.category());
      }
      if (entry.hasNote())
      {
         entry_.setNote(entry.note());
      }
      if (entry_.payee().startsWith("@"))
      {
         entry_.setPayee(entry.payee().mid(1));
         entry_.setTransfer(true);
      }
      else
      {
         entry_.setPayee(entry.payee());
      }
      xact.appendEntry(entry_);
   }

   if (total != 0)
   {
      qWarning("Transaction entries did not total correctly, line %d",
               transaction.lineNum());
      exit(EXIT_FAILURE);
   }
   emit transacted(xact);
}

int NativeModel::parseCurrency(QString curr, int line)
{
   curr.remove(QLocale::system().currencySymbol());
   curr.remove(QLocale::system().groupSeparator());
   int decimalIndex = curr.indexOf(QLocale::system().decimalPoint());
   int decimalDigits = curr.length() - decimalIndex - 1;
   if (m_decimalDigits == -1)
   {
      m_decimalDigits = decimalDigits;
   }
   else if (m_decimalDigits != decimalDigits)
   {
      qWarning("Inconsistent number of decimal digits, line %d", line);
      exit(EXIT_FAILURE);
   }
   curr.remove(QLocale::system().decimalPoint());
   bool success;
   int retval = curr.toInt(&success);
   if (!success)
   {
      qWarning("Could not understand currency, line %d", line);
      exit(EXIT_FAILURE);
   }
   return retval;
}

QDate NativeModel::parseDate(QString const& date, int line)
{
   // TODO let this be dynamic to accept dates in any unambiguous format
   QDate d(QDate::fromString(date, Qt::SystemLocaleShortDate));
   if (!d.isValid())
   {
      qWarning("Unable to read date '%s', expected something like '%s', line "
               "%d", qPrintable(date),
               qPrintable(QLocale::system().dateFormat(QLocale::ShortFormat)),
               line);
      exit(EXIT_FAILURE);
   }
   return d;
}
