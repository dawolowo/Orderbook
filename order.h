#ifndef ORDER_H
#define ORDER_H
#include "alias_enums.h"
#include <format>

struct Order{
    private:
        Price price;
        Quantity initial_quantity;
        Quantity remaining_quantity;
        OrderType order_type;
        OrderDirection direction;
        OrderId order_id;
    public:
        Order() = default;
        Order(Price price, Quantity quantity, OrderType ordertype, OrderDirection side, OrderId id){
            this->price = price;
            this->initial_quantity = quantity;
            this->remaining_quantity = quantity;
            this->order_type = ordertype;
            this->direction = side;
            this->order_id = id;
        }
        OrderId get_order_id() const {
            return order_id;
        }
        OrderDirection get_direction() const{
            return direction;
        }
        Price get_price() const {
            return price;
        }
        Quantity get_initial_quantity() const{
            return initial_quantity;
        }
        Quantity get_filled_quantity() const{
            return initial_quantity- remaining_quantity;
        }
        Quantity get_remaining_quantity() const {
            return remaining_quantity;
        }
        void fill(Quantity quantity){
            // This will be utilized in the mathcing algorithm
            if (quantity > remaining_quantity){
                throw std::logic_error(std::format("Order Id #{} : quantity can not be greater than the unfilled quantity",get_order_id()));
            }
            remaining_quantity -= quantity;
        }
};
#endif