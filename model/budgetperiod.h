#ifndef MODEL_BUDGETPERIOD_H
#define MODEL_BUDGETPERIOD_H

#include <QDate>
#include "interval.h"

namespace model { struct BudgetPeriod; }

struct model::BudgetPeriod
{
   int id;  //PK
   QDate startDate;
   Interval period;
};

#endif
