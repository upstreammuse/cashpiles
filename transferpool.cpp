#include "transferpool.h"

TransferPool::TransferPool() :
   m_value(0)
{
}

void TransferPool::addTransfer(int amount)
{
   m_value += amount;
}
