#ifndef TIME_SALES_HPP
#define TIME_SALES_HPP

#include "alias.hpp"
#include "enums.hpp"

/*Structure that contains the time and sales data
@param timestamp execution time
@param price execution price 
@param quantity volume traded
@param buyer_is_maker maker/taker. True if it's sell market order, false if it's buy market order
*/
struct TimeSale {
    time_t timestamp;
    Price price;
    Quantity quantity;
    bool buyer_is_maker;
    friend std::ostream &operator<<(std::ostream &out, const TimeSale &ts){
        out << ts.timestamp << "," << ts.price << "," << ts.quantity << "," << ts.buyer_is_maker;
        return out;
    }
};

#endif
