#include "orderbook.hpp"
#include <cstdlib>
#include <chrono>
using namespace std;

/*Generates random limit orders
@param ob OrderBook where the limit will be placed
@param n number of buy and sell limit to generate. Total limit generated will be 2*n
@param spread spread of the OB. A value 0.05 means the highest and lowest price in the OB cannot be more than +5% and -5% away from the price.
*/
void generate_limits(OrderBook &ob, size_t n, double spread=0.05){
    OrderDirection directions[] = {OrderDirection::buy, OrderDirection::sell};
    srand(time(0));
    while (n--){
        if (rand()%2 == 0){ 
            Price s_price = ob.last_price() + ((double) rand()/RAND_MAX)*ob.last_price()*spread ;
            Quantity s_qty = ((double)rand()/RAND_MAX)*100 + 1;
            ob.add_order(Order(s_price, s_qty, OrderType::limit, OrderDirection::sell));
        }
        else {
            Price b_price = ob.last_price() - ((double) rand()/RAND_MAX)*ob.last_price()*spread ;
            Quantity b_qty = ((double)rand()/RAND_MAX)*100 + 1;
            ob.add_order(Order(b_price, b_qty, OrderType::limit, OrderDirection::buy));
        }
    }
}

void generate_mo(OrderBook &ob, size_t n){
    OrderDirection directions[] = {OrderDirection::buy, OrderDirection::sell};
    srand(time(0));
    while (n--){
        Price price = 5; // Price doesn't matter for market orders, since it would be executed at best price
        Quantity qty = ((double)rand()/RAND_MAX)*100 + 1;
        OrderDirection dir = directions[rand()%2];
        ob.add_order(Order(price, qty, OrderType::mo, dir));
    }
}

int main(){
    OrderBook btc_OB(60200, 2);
    
    generate_limits(btc_OB, 600000, 900);
    generate_mo(btc_OB, 100000);
    auto start = std::chrono::high_resolution_clock::now();
    btc_OB.match();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-start);
    
    return 0;
}
