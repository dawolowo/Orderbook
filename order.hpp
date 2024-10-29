#ifndef ORDER_HPP
#define ORDER_HPP
#include "alias.hpp"
#include "enums.hpp"
#include <format>

OrderId ID = 0;

OrderId get_order_id(){
    return ID++;
}

/*Object that represents a realife order
@param price price
@param quantity volume of order
@param ordertype type of order
@param side direction of trade. buy/sell
*/
class Order {
    
private:
    Price _price;
    Quantity _initial_quantity;
    Quantity _quantity;
    OrderType _order_type;
    OrderDirection _direction;
    OrderId _order_id;

public:
    
    Order() = default;
    Order(Price price, Quantity quantity, OrderType ordertype, OrderDirection side){
        this->_price = price;
        this->_initial_quantity = quantity;
        this->_quantity = quantity;
        this->_order_type = ordertype;
        this->_direction = side;
        this->_order_id = get_order_id();
    }
    OrderType order_type()const {
        return _order_type;
    }
    OrderId order_id() const {
        return _order_id;
    }
    OrderDirection direction() const{
        return _direction;
    }
    Price price() const {
        return _price;
    }
    Quantity initial_quantity() const{
        return _initial_quantity;
    }
    Quantity &quantity() {
        return _quantity;
    }

   //Sets the order's quantity and initial quantity which effectively cancels it
    void cancel(){
        _initial_quantity =_quantity = 0;
    }

    /*Fills the order. Returns the volume filled*/
    Quantity fill(Quantity &quantity){
        Quantity volume = std::min(quantity, this->_quantity);
        if (quantity > this->_quantity){
            quantity -= this->_quantity;
            this->_quantity = 0;
        }
        else {
            this->_quantity -= quantity;
            quantity = 0;
        }
        return volume;
    }

    friend int operator<=>(const Order& other1, const Order &other2){
        if (other1._price < other2._price) return -1;
        else if (other1._price > other2._price) return 1;
        return 0;
    }
};

#endif
