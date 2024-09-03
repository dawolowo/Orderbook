#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <iostream>
#include <vector>
#include <format>
#include <map>
#include <unordered_map>
#include "alias_enums.h"
#include "orderlevel.h"
#include "order.h"

OrderId ID = 0;

OrderId get_order_id(){
    ID++;
    return ID;
}

struct OrderbookInfo{
    // Contains the orderbook information. It can be used to take a snapshot at an instance since the order book changes quickly in the real world
    private:
        LevelsInfo bids;
        LevelsInfo asks;
    public:
        OrderbookInfo(const LevelsInfo &bids, const LevelsInfo &asks){
            this->bids = bids;
            this->asks = asks;
        }
        const LevelsInfo &get_bids() const{
            return bids;
        }
        const LevelsInfo &get_asks() const{
            return asks;
        }

};


struct OrderBook{
    private:
        std::map<Price, Quantity, std::greater<Price>> bids;
        std::map<Price, Quantity, std::less<Price>> asks;
        std::unordered_map<OrderId, Order> orders;

    public:
        OrderBook(){

        }
        void add_order(Order order){
            if (order.get_direction() == OrderDirection::buy){
                bids[order.get_price()] += order.get_initial_quantity();
            }
            else if (order.get_direction() == OrderDirection::sell){
                asks[order.get_price()] += order.get_initial_quantity();
            }
            orders[order.get_order_id()] = order;
        }

        void cancel_order(OrderId id){
            if (orders.find(id) == orders.end()) throw std::logic_error(std::format("Cannot cancel an order with invalid order id:{}", id));
            const Order &order = orders[id];
            if (order.get_direction() == OrderDirection::buy){
                bids[order.get_price()] -= order.get_initial_quantity();
                if (bids[order.get_price()] == 0) bids.erase(order.get_price());
            }
            else if (order.get_direction() == OrderDirection::sell){
                asks[order.get_price()] -= order.get_initial_quantity();
                if (asks[order.get_price()] == 0) asks.erase(order.get_price());
            }
            orders.erase(id);
        }

        void modify_order(OrderId id, Order new_order){
            cancel_order(id);
            add_order(new_order);
        }

        bool is_empty() const{
            return bids.empty() && asks.empty();
        }

        LevelsInfo get_bids() const {
            // I'm using LevelsInfo instead of a map because it gives flexibility. e.g checking a against a price level, checking against a quantity
            LevelsInfo info;
            for (auto &it : bids){
                info.push_back(OrderLevel(it.first, it.second));
            }
            return info;
        }

        LevelsInfo get_asks() const{
            LevelsInfo info;
            for (auto &it : asks){
                info.push_back(OrderLevel(it.first, it.second));
            }
            return info;
        }

        OrderbookInfo orderbook_info() const{
            // Takes a snapshot of the orderbook
            return OrderbookInfo(get_bids(), get_asks());
        }

        friend std::ostream &operator<<(std::ostream &out, const OrderBook &ob);

        
};

std::ostream &operator<<(std::ostream &out, const OrderBook &ob){
    if (ob.is_empty()) out << "Empty OrderBook";
    else {
        out << "\tPRICE\tQUANTITY\nASKS:\n";
        for (auto level = ob.asks.rbegin(); level != ob.asks.rend(); level++){
            out << "\t" << level->first << "\t" << level->second << "\n";
        }
        out << "\nBIDS:\n";
        for (auto level = ob.bids.begin(); level != ob.bids.end(); level++){
            out << "\t" << level->first << "\t" << level->second << "\n";
        }
    }
    return out;
}

#endif