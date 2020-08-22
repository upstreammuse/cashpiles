#include "filereaderregex.h"

#include <sstream>

using std::string;
using std::stringstream;

FileReaderRegEx::FileReaderRegEx() :
   CLEAR_RX("(\\*|\\!|\\?)"),
   CURR_RX(currencyRx()),
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
      START_RX + DATE_RX + SPACE_RX + "balance" + SPACE_RX + IDENT_RX + SEP_RX +
      CURR_RX + END_RX),
   budgetRx(
      START_RX + DATE_RX + SPACE_RX + "budget" + SPACE_RX + INTERVAL_RX +
      END_RX),
   budgetLineCancelRx(
      START_RX + SEP_RX + "cancel" + SPACE_RX + IDENT_RX + SEP_RX + IDENT_RX +
      END_RX),
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
      START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX + SEP_RX + CURR_RX +
      SPACE_RX + INTERVAL_RX + optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineReservePercentRx(
      START_RX + SEP_RX + "reserve" + SPACE_RX + IDENT_RX + SEP_RX +
      PERCENT_RX + optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineRoutineRx(
      START_RX + SEP_RX + "routine" + SPACE_RX + IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   budgetLineWithholdingRx(
      START_RX + SEP_RX + "withholding" + SPACE_RX + IDENT_RX +
      optional(SEP_RX + IDENT_RX) + END_RX),
   txnCompactOffRx(
      START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX + SEP_RX +
      IDENT_RX + SEP_RX + CURR_RX + END_RX),
   txn2Rx(
      START_RX + DATE_RX + SPACE_RX + CLEAR_RX + SPACE_RX + IDENT_RX +
      optional(SEP_RX + CURR_RX) + END_RX),
   txn2LineRx(
      START_RX + SEP_RX + IDENT_RX + optional(SEP_RX + CURR_RX) + END_RX),
   txn2TrackingLineRx(
      START_RX + SEP_RX + IDENT_RX + SEP_RX + IDENT_RX +
      optional(SEP_RX + CURR_RX) + END_RX)
{
}

string FileReaderRegEx::currencyRx()
{
   stringstream retval;
   struct lconv* lc = localeconv();
   retval << "((?:";

   string symbol(lc->currency_symbol);
   // make sure ascii currency symbols don't interfere with regex
   for (char c : symbol)
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

   string sep(lc->mon_thousands_sep);
   // make sure ascii separator symbols don't interfere with regex
   for (char c : sep)
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

   string decimal(lc->mon_decimal_point);
   for (char c : decimal)
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

   string negative(lc->negative_sign);
   for (char c : negative)
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

string FileReaderRegEx::optional(string const& item)
{
   stringstream ss;
   ss << "(?:" << item << ")?";
   return ss.str();
}
