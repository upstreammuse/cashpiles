#pragma once

#include <memory>
#include "../util/m_date.h"
#include "modeldata.h"
#include "transactionflag.h"

namespace model { struct BudgetPeriod; }
namespace model { struct Transaction; }

struct model::Transaction : public ModelData
{
   int const id = newId();  //PK
   std::shared_ptr<BudgetPeriod const> budgetPeriod;  //FK
   util::Date date;
   TransactionFlag flag;
   std::string payee;

   Transaction(
         std::shared_ptr<BudgetPeriod const>, util::Date const&,
         TransactionFlag, std::string const&);
};
