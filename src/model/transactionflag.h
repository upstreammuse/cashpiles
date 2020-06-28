#pragma once

namespace model { enum class TransactionFlag; }

enum class model::TransactionFlag
{
   CLEARED,
   DISPUTED,
   PENDING
};
