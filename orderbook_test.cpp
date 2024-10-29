#include "orderbook.hpp"
using namespace std;

int main(){
    OrderBook btc_OB(60200);

    Price prices[] = {60000, 60300, 60050, 60400, 60400, 60054, 60800, 61502, 60355, 60195};

    OrderDirection directions[] = {
        OrderDirection::buy,OrderDirection::sell, OrderDirection::buy,OrderDirection::sell,OrderDirection::sell, OrderDirection::buy, OrderDirection::sell,
        OrderDirection::sell,OrderDirection::sell,OrderDirection::buy,};

    Quantity quantites[] = {10,89,11,45,66,1,2,4,26,61};
    OrderId ids[10];
    for (int i = 0; i < 10; i++){
        Order order = Order(prices[i], quantites[i], OrderType::limit, directions[i]);
        ids[i] = order.order_id();
        btc_OB.add_order(order);
    }
    // btc_OB.cancel_order(ids[0]);
    btc_OB.add_order(Order(61055, 226, OrderType::mo, OrderDirection::sell));
    btc_OB.match();

    cout << btc_OB << "\n";
    cout << btc_OB.last_price() << "\n";
    for (const auto & x : btc_OB.time_sales()){
        cout << x << "\n";
    }   
    
    return 0;
}
