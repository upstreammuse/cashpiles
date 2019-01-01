# CashPiles

CashPiles is an envelope budgeting program that uses plain text files.  It
allows you to manage all your finances in an easy-to-use file that readably
tracks accounts, income, expenses, budget categories, and more.  The CashPiles
program reads the file, performs calculations, and provides answers and insight.

## TODO Items

- do not allow uncleared transactions before a balance assertion.  This may
  require the addition of a second date to track transaction original dates vs.
  cleared dates.  Also, be sure to allow the disputed '!' flag to occur before a
  balance assertion.

- warn if budget categories do not already exist, and create them as routine
  expenses if they do not.  Alternatively, force them to be created by a budget
  command ahead of time
