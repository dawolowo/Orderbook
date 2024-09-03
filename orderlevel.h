#ifndef ORDER_LEVEL_H
#define ORDER_LEVEL_H

#include "alias_enums.h"
#include <vector>

struct OrderLevel {
    // Contains the quantity of orders at a price level
    Price price;
    Quantity quantity;
    OrderLevel(){

    }
    OrderLevel(Price price, Quantity quantity){
        this->price = price;
        this->quantity = quantity;
    }
    bool operator>(const OrderLevel &level) const {
        return (this->price > level.price);
    }
    bool operator<(const OrderLevel &level) const {
        return (this->price < level.price);
    }
    friend std::ostream &operator<<(std::ostream &out, const OrderLevel &other);
};

std::ostream &operator<<(std::ostream &out, const OrderLevel &other){
    out << other.price << "\t" << other.quantity;
    return out;
}

typedef std::vector<OrderLevel> LevelsInfo; // Contains the Quantity at every price level

#endif
