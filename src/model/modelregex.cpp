#include "modelregex.h"

#include <sstream>

using namespace model;

using std::string;
using std::stringstream;

ModelRegex::ModelRegex(FileReaderFormat const& format) :
   CLEAR_RX("(\\*|\\!|\\?)"),
   CURR_RX(currencyRx(format)),
   DATE_RX("(\\d+[\\/\\.\\-]\\d+[\\/\\.\\-]\\d+)"),
   END_RX("$"),
   IDENT_RX("(\\S(?:\\S| (?! ))*)"),
   INTERVAL_RX("(\\+\\d+[dwmy])"),
   PERCENT_RX("(\\d+)%"),
   SEP_RX("(?:\\s{2,}|\\t)\\s*"),
   SPACE_RX("\\s+"),
   START_RX("^"),
   accountRx(
      START_RX + DATE_RX + SPACE_RX + "(on-budget|off-budget|close)" +
      SPACE_RX + IDENT_RX + END_RX),
   accountBalanceRx(
      START_RX + DATE_RX + SPACE_RX + "balance" + SPACE_RX + IDENT_RX +
      SEP_RX + CURR_RX + END_RX),
   budgetRx(
      START_RX + DATE_RX + SPACE_RX + "budget" + SPACE_RX + INTERVAL_RX +
      END_RX),
   budgetLineCancelRx(
      START_RX + SEP_RX + "cancel" + SPACE_RX + IDENT_RX + SEP_RX +
      IDENT_RX + END_RX),
   budgetLineCloseRx(
      START_RX + SEP_RX + "close" + SPACE_RX + IDENT_RX + END_RX),
   budgetLineGoalRx(
      START_RX + SEP_RX + "goal" + SPACE_RX + IDENT_RX + SEP_RX + IDENT_RX +
      SEP_RX + CURR_RX + SPACE_RX + DATE_RX + END_RX),
   budgetLineGoalsRx(
      START_RX + SEP_RX + "goals" + SPACE_RX + IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineIncomeRx(
      START_RX + SEP_RX + "income" + SPACE_RX + IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineReserveAmountRx(
      START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX +
      SEP_RX + CURR_RX + SPACE_RX + INTERVAL_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineReservePercentRx(
      START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX +
      SEP_RX + PERCENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineRoutineRx(
      START_RX + SEP_RX + "routine" + SPACE_RX + IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineWithholdingRx(
      START_RX + SEP_RX + "withholding" + SPACE_RX +
      IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   txnRx(
      START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX +
      optional(SEP_RX + CURR_RX) + END_RX),
   txnLineRx(
      START_RX + SEP_RX + IDENT_RX + optional(SEP_RX + CURR_RX) + END_RX),
   txnTrackingLineRx(
      START_RX + SEP_RX + IDENT_RX + SEP_RX + IDENT_RX +
      optional(SEP_RX + CURR_RX) + END_RX),
   txnRefRx(
      START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX +
      SEP_RX + IDENT_RX + SEP_RX + CURR_RX + END_RX)
{
}

string ModelRegex::currencyRx(FileReaderFormat const& format)
{
   stringstream retval;
   retval << "((?:";

   // escape ascii symbols
   for (char c : format.currencyFormat.symbol)
   {
      if (c & 0x80)
      {
         retval << c;
      }
      else
      {
         retval << '\\' << c;
      }
   }
   retval << '|';

   // escape ascii symbols
   for (char c : format.currencyFormat.separator)
   {
      if (c & 0x80)
      {
         retval << c;
      }
      else
      {
         retval << '\\' << c;
      }
   }
   retval << '|';

   // escape ascii symbols
   for (char c : format.currencyFormat.decimal)
   {
      if (c & 0x80)
      {
         retval << c;
      }
      else
      {
         retval << '\\' << c;
      }
   }
   retval << '|';

   // escape ascii symbols
   for (char c : format.currencyFormat.negative)
   {
      if (c & 0x80)
      {
         retval << c;
      }
      else
      {
         retval << '\\' << c;
      }
   }
   retval << '|';

   retval << "\\d)+)";
   return retval.str();
}

string ModelRegex::optional(string const& item)
{
   stringstream ss;
   ss << "(?:" << item << ")?";
   return ss.str();
}
