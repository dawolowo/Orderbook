#include "orderbook.h"
using namespace std;

int main(){
    OrderBook btc_OB;

    Price prices[] = {60000, 60100, 60200, 60400, 60400, 60300, 60800, 60152, 60355, 60452};

    OrderDirection directions[] = {
        OrderDirection::buy,OrderDirection::sell, OrderDirection::buy,OrderDirection::sell,OrderDirection::sell, OrderDirection::buy, OrderDirection::sell,
        OrderDirection::sell,OrderDirection::sell,OrderDirection::buy,};

    OrderId ids[] = {get_order_id(),get_order_id(),get_order_id(),get_order_id(),get_order_id(),
        get_order_id(),get_order_id(),get_order_id(),get_order_id(),get_order_id()};

    Quantity quantites[] = {10,89,11,45,66,1,2,4,26,61};

    for (int i = 0; i < 10; i++){
        btc_OB.add_order(Order(prices[i], quantites[i], OrderType::gtc, directions[i], ids[i]));
    }

    cout << btc_OB << endl;

    LevelsInfo asks = btc_OB.get_asks();

    for (auto &ask : asks){
        cout << ask << endl;
    }

    return 0;
}