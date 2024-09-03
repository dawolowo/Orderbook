#ifndef ALIAS_H
#define ALIAS_H
#include <iostream>

typedef double Price;
typedef unsigned long long Quantity;
typedef unsigned long long OrderId;

enum class OrderType{
    // To be used in the matching algorithm
    gtc, // Good Till Cancel
    fok // Fill Or Kill
};

enum class OrderDirection{
    buy,
    sell
};


#endif