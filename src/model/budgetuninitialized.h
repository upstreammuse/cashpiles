#pragma once

#include <stdexcept>

namespace model { class BudgetUninitialized; }

class model::BudgetUninitialized : public std::logic_error
{
public:
   BudgetUninitialized();
};
