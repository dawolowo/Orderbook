#ifndef ORDER_LEVEL_HPP
#define ORDER_LEVEL_HPP

#include "alias.hpp"
#include "order.hpp"
#include <vector>
#include <queue>

/* Queue like data structure that stores all orders at a price level
*/
class OrderLevel {
    std::queue<Order> _data;
    
public:
    Price price;
    Quantity quantity = 0;
    OrderLevel(){

    }

    /*Adds an order
    @param order Order to be added to the level
    */
    void push(Order &order){
        price = order.price();
        quantity += order.quantity();
        _data.push(order);
    }
    
    /*Adds an order
    @param order Order to be added to the level
    */
    void push(Order && order){ push(order); }

    /*Returns a read/write reference to the data at the first element of the level.*/
    Order &front() {return _data.front();}

    /*Removes first element
    @note No data is returned, and if the first element's data is needed, it should be retrieved before pop() is called.*/
    void pop(){
        quantity -= _data.front().quantity();
        _data.pop();
    }

    /*Returns true if no order is present at this level*/
    bool empty(){return _data.empty();}

    // Returns the number of order at the price level
    size_t size(){return _data.size();}

    /*Returns a read/write reference to the data at the last element of the level.*/
    Order &back(){return _data.back();}

    friend std::ostream &operator<<(std::ostream &out, const OrderLevel &other);
};

std::ostream &operator<<(std::ostream &out, const OrderLevel &other){
    out << other.price << "\t" << other.quantity;
    return out;
}

#endif
