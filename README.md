# CashPiles





      // TODO save into essentials average if essential
      // TODO push into the statements container if cleared

      // TODO statements container is sorted list of buckets with date properties
      // - finding a bucket is the first with a date equal or after the transaction date


/*
to build essential spending reserve, do something like this

- calculate average of essential costs ($/day)
- starting with first day (earliest xaction), add average to reserve
- when encountering an essential xaction without a pile, subtract from reserve
  - if reserve ever becomes negative, add to reserve until 0
- iterate for all days to present

After doing this, you have the breakdown

- total cash available
  - (include cleared income and all expenses across all accounts)
- subtract value of uncategorized essentials pile
- subtract pile reservations
- subtract emergency fund reservation
- what's left is spendable



How to manage interplay between essentials and piles?
- essential may have a pile, how does this calculate?

*/


There should be validation that @asdf transfers match an account name.
