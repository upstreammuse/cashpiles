#ifndef CATEGORY_H
#define CATEGORY_H

#include <QDate>
#include <QString>
#include "currency.h"
#include "interval.h"

namespace model { struct GoalCategory; }
namespace model { struct IncomeCategory; }
namespace model { struct ReserveAmountCategory; }
namespace model { struct ReservePercentageCategory; }
namespace model { struct RoutineCategory; }

struct model::GoalCategory
{
   QString name;  //PK
};

struct model::IncomeCategory
{
   QString name;  //PK
};

struct model::ReserveAmountCategory
{
   QString name;  //PK
   Currency amount;
   QDate startDate;
   Interval period;
};

struct model::ReservePercentageCategory
{
   QString name;  //PK
   uint percentage;
};

struct model::RoutineCategory
{
   QString name;  //PK
};

#endif
