#pragma once

#include "budgetentry.h"
#include "../util/m_currency.h"
#include "../util/m_date.h"

namespace model { struct BudgetGoalEntry; }
namespace model { struct BudgetGoalsEntry; }

struct model::BudgetGoalEntry : public BudgetEntry
{
   std::string const name;  //PK
   std::shared_ptr<BudgetGoalsEntry const> const category;  //PK/FK
   util::Date date;
   util::Currency amount;

   BudgetGoalEntry(
         std::string const&, std::shared_ptr<BudgetGoalsEntry const>,
         util::Date const&);
};
