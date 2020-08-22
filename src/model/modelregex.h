#pragma once

#include <regex>
#include <string>
#include "../filereaderformat.h"

namespace model { struct ModelRegex; }

struct model::ModelRegex
{
   std::string const CLEAR_RX;
   std::string const CURR_RX;
   std::string const DATE_RX;
   std::string const END_RX;
   std::string const IDENT_RX;
   std::string const INTERVAL_RX;
   std::string const PERCENT_RX;
   std::string const SEP_RX;
   std::string const SPACE_RX;
   std::string const START_RX;

   std::regex const accountRx;
   std::regex const accountBalanceRx;
   std::regex const budgetRx;
   std::regex const budgetLineCancelRx;
   std::regex const budgetLineCloseRx;
   std::regex const budgetLineGoalRx;
   std::regex const budgetLineGoalsRx;
   std::regex const budgetLineIncomeRx;
   std::regex const budgetLineReserveAmountRx;
   std::regex const budgetLineReservePercentRx;
   std::regex const budgetLineRoutineRx;
   std::regex const budgetLineWithholdingRx;
   std::regex const txnRx;
   std::regex const txnLineRx;
   std::regex const txnTrackingLineRx;
   std::regex const txnRefRx;

   ModelRegex(FileReaderFormat const&);
   std::string currencyRx(FileReaderFormat const&);
   std::string optional(std::string const& item);
};
