#include "transactionentry.h"

void asdf()
{
   model::CategorizedTransactionEntry entry =
   {
      .id = 4123,
      .transaction = 11,
      .payee = "Hello",
      .category = "stuff",
      .amount =
      {
         .value = 4312,
         .decimalPlaces = 2,
      }
   };

   model::Currency asdf = {.value=13,.decimalPlaces=3};

   model::Currency total = entry.amount + asdf;

}
