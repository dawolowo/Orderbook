#ifndef LEVELS_AGG
#define LEVELS_AGG

#include "alias.hpp"
#include "enums.hpp"
#include <vector>
/*Structure that contains the price at total volume of orders at that level
@param price Price of the level
@param quantity total volume of order present at that level
*/
struct LevelsAgg{
    Price price;
    Quantity quantity;
    friend std::ostream &operator<<(std::ostream &out, LevelsAgg & level){
        out << level.price << "\t" << level.quantity;
        return out;
    }
};
typedef std::vector<LevelsAgg> LevelsInfo; // Contains the Quantity at every price level

#endif
