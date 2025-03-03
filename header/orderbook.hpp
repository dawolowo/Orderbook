#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <iostream>
#include <vector>
#include <format>
#include <map>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <iomanip>
#include "alias.hpp"
#include "orderlevel.hpp"
#include "order.hpp"
#include "enums.hpp"
#include "levels_agg.hpp"
#include "time_sale.hpp"


// Contains the orderbook information. Used to take a snapshot at an instance since the order book changes quickly in the real world
struct OrderbookInfo{
    
private:
    LevelsInfo bids;
    LevelsInfo asks;
public:
    OrderbookInfo(const LevelsInfo &&bids, const LevelsInfo &&asks){
        this->bids = bids;
        this->asks = asks;
    }
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

/*An object that matches/ processes various orders.
@param initial_price Starting price of the asset.
@param dp number of decimal place
*/
class OrderBook{
    
private:    
    std::map<Price, OrderLevel, std::greater<Price>> _bids; // Keys in descending order
    std::map<Price, OrderLevel, std::less<Price>> _asks; // Keys in ascending order
    std::unordered_map<OrderId, Order *> _limit_orders;
    std::map<Price, OrderLevel, std::less<>> _buy_stops; // Ascending order
    std::map<Price, OrderLevel, std::greater<>> _sell_stops; // Descending order
    std::unordered_map<OrderId, Order *> _stop_orders;
    Price _last_price;
    int _decimal_place;
    std::vector<TimeSale> _time_and_sales;
    std::queue<Order> _market_orders;
    Quantity _bid_vol = 0, _ask_vol = 0;

    /*Matching algorithm. Uses FIFO algorithm*/
    void _match(){
        while (!_market_orders.empty() && !_bids.empty() && !_asks.empty()){
            Order &mo = _market_orders.front();
            if (mo.direction() == OrderDirection::buy && !_asks.empty()){
                _match_helper(mo, _asks);
            }
            else if (mo.direction() == OrderDirection::sell && !_bids.empty()){
                _match_helper(mo, _bids);
            }
            if (!_buy_stops.empty()){
                if (_last_price >= _buy_stops.begin()->first){
                    _stops_trigger(_buy_stops);
                }
            }
            if (!_sell_stops.empty()){
                if (_last_price <= _sell_stops.begin()->first){
                    _stops_trigger(_sell_stops);
                }
            }
        }
    }
    
    //Processes the stops when it's triggered
    void _stops_trigger(auto &stop_pool){
        OrderLevel &level = stop_pool.begin()->second;
        while (!level.empty()){
            Order &order = level.front();
            add_order(Order(order.price(), order.quantity(), OrderType::mo, order.direction()));
            level.pop();
        }
    }

    // helper function for match()
    void _match_helper(Order &mo, auto &market){
        Price best_price = market.begin()->first;
        if (mo.order_type() == OrderType::fok){
            if (mo.quantity() <= market.begin()->second.quantity &&(best_price >= mo.price() && mo.direction() == OrderDirection::sell ||
             best_price <= mo.price() && mo.direction() == OrderDirection::buy)) _fill(mo);
            _market_orders.pop();
        }
        else if (mo.order_type() == OrderType::ioc){
            if (best_price >= mo.price() && mo.direction() == OrderDirection::sell || best_price <= mo.price() && mo.direction() == OrderDirection::buy)
                _fill(mo);
            _market_orders.pop();
        }
        else if (mo.order_type() == OrderType::mo){
            _fill(mo);
            if (mo.quantity() == 0) _market_orders.pop();
        }
    }

    //Fills order. Fills only one level does not to another level
    void _fill(Order &mo){
        if (mo.direction() == OrderDirection::buy) _fill_helper(mo, _asks);
        else _fill_helper(mo, _bids);
    }

    // Helper to _fill()
    void _fill_helper(Order &mo, auto &market){
        OrderLevel &level = market.begin()->second;
        while (!level.empty() && mo.quantity() > 0){
            Quantity filled_volume, initial = mo.quantity();
            filled_volume = level.front().fill(mo.quantity());
            level.quantity -= filled_volume;
            _last_price = level.front().price();
            if (mo.direction() == OrderDirection::buy){
                _time_and_sales.push_back(TimeSale(time(0), _last_price, filled_volume, false));
                _ask_vol -= filled_volume;
            }
            else{
                _time_and_sales.push_back(TimeSale(time(0), _last_price, filled_volume, true));
                _bid_vol -= filled_volume;
            }                 
            if (level.front().quantity() == 0) level.pop();
        }
        if (level.empty()) market.erase(market.begin());
    }

    //Checks if an order is proper
    bool _proper_order(Order &order){
        if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::buy && order.price() > _last_price) return false;
        else if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::sell && order.price() < _last_price) return false;
        else if (order.order_type() == OrderType::stop && order.direction() == OrderDirection::buy && order.price() < _last_price) return false;
        else if (order.order_type() == OrderType::stop && order.direction() == OrderDirection::sell && order.price() > _last_price) return false;
        return true;
    }

    //Helper to cancel()
    void _cancel_helper(OrderId id, auto &pool){
        Order &order = *(pool[id]);
        Price price = order.price();
        if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::buy){
            _bids[price].quantity -= order.initial_quantity();
            _bid_vol -= order.initial_quantity();
            if (_bids[price].quantity == 0) _bids.erase(price);
        }   
        else if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::sell){
            _asks[price].quantity -= order.initial_quantity();
            _ask_vol -= order.initial_quantity();
            if (_asks[price].quantity == 0) _asks.erase(price);
        }
        else if (OrderType::stop == order.order_type() && OrderDirection::buy == order.direction()){
            _buy_stops[price].quantity -= order.initial_quantity();
            if (_buy_stops[price].quantity == 0) _buy_stops.erase(price);
        }
        else if (OrderType::stop == order.order_type() && OrderDirection::sell == order.direction()){
            _sell_stops[price].quantity -= order.initial_quantity();
            if (_sell_stops[price].quantity == 0) _sell_stops.erase(price);
        }
        order.cancel();
        pool.erase(id);
    }
public:

    OrderBook(Price initial_price, int dp){
        _last_price = initial_price;
        _decimal_place = dp;
    }

    //Returns the last traded price
    Price last_price() const {return _last_price;}

    //Returns the best bid
    Price best_bid(){
        if (_bids.empty()) return 0;
        return _bids.begin()->first;
    }

    //Returns the best ask
    Price best_ask(){
        if (_asks.empty()) return 0;
        return _asks.begin()->first;
    }

    //Returns the spread of the market
    Price spread(){return best_ask() - best_bid();}

    //Matches orders in the orderbook
    void match(){_match();}

    /*Adds and order to the orderbook
    @param order Order to be added
    */
    void add_order(Order &&order) {add_order(order);}

    /*Adds and order to the orderbook
    @param order Order to be added
    */
    void add_order(Order &order){
        if (!_proper_order(order)) throw std::logic_error("Cause: add_order()\tCheck the order to ensure it is proper");
        order.set_dp(_decimal_place);
        Price price = order.price();
        OrderId id = order.order_id();
        if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::buy){
            _bids[price].push(order);
            _limit_orders[id] = &_bids[price].back();
            _bid_vol += order.initial_quantity();
        }
        else if (order.order_type() == OrderType::limit && order.direction() == OrderDirection::sell){
            _asks[price].push(order);
            _limit_orders[id] = &_asks[price].back();
            _ask_vol += order.initial_quantity();
        }
        else if (order.order_type() == OrderType::stop && order.direction() == OrderDirection::buy){
            _buy_stops[price].push(order);
            _stop_orders[id] = &_buy_stops[price].back();
        }
        else if (order.order_type() == OrderType::stop && order.direction() == OrderDirection::sell){
            _sell_stops[price].push(order);
            _stop_orders[id] = &_sell_stops[price].back();
        }
        else if (order.order_type() == OrderType::fok || order.order_type() == OrderType::mo || order.order_type() == OrderType::ioc){
            _market_orders.push(order);
        }
    }

    /*Cancels an order
    @param id Order id of the order being cancelled
    */
    void cancel_order(OrderId id){
        if (_limit_orders.find(id) != _limit_orders.end()){
            _cancel_helper(id, _limit_orders);
            
        }
        else if (_stop_orders.find(id) != _stop_orders.end()){
            _cancel_helper(id, _stop_orders);
        }
        else throw std::logic_error(std::format("Cannot cancel an order with invalid order id:{}", id));
    }
    
    /*Modifies an order
    @param id order id of the order being modified
    @param new_order new order
    */
    void modify_order(OrderId id, Order &&new_order){
        modify_order(id, new_order);
    }

    /*Modifies an order
    @param id order id of the order being modified
    @param new_order new order
    */
    void modify_order(OrderId id, Order &new_order){
        cancel_order(id);
        add_order(new_order);
    }
    
    //Return true if no order is present in the orderbook
    bool empty() const{
        return _bids.empty() && _asks.empty();
    }
    //Prints the orderbook
    void print(){
        std::ostream &out = std::cout;
        if (empty()) out << "Empty OrderBook";
        else {
            int bars = (_bids.size() + _asks.size()) * 8; // To dynamically determine the number of bars
            Quantity vol = _bid_vol+_ask_vol;
            out << std::setprecision(_decimal_place) << std::fixed;
            out << "\033[91m"; // set color to red
            for (auto level = _asks.rbegin(); level != _asks.rend(); level++){
                out << "\t" << level->first << "\t" << level->second.quantity << "\t";
                for (int i = 0; i <= level->second.quantity * bars / vol; i++)
                    out << "⬜";
                out << "\n";
            }
            out << "\033[33m"; // set color to yellow
            out << "\nPRICE => " << _last_price << "\n\n";
            out << "\033[92m"; // set color to green

            for (auto level = _bids.begin(); level != _bids.end(); level++){
                out << "\t" << level->first << "\t" << level->second.quantity << "\t";
                for (int i = 0; i <= level->second.quantity * bars / vol; i++)
                    out << "⬜";
                out << "\n";
            }
            out << "\033[0m" ; // reset
        }
    }
    
    //Returns const refernce of the time and sales data
    const std::vector<TimeSale> &time_sales(){return _time_and_sales;}

    //Returns the bids aggregate of all price levels
    LevelsInfo get_bids() const {
        // I'm using LevelsInfo instead of a map because it gives flexibility. e.g checking a against a price level, checking against a quantity
        LevelsInfo info;
        for (auto &it : _bids){
            info.push_back(LevelsAgg(it.first, it.second.quantity));
        }
        return info;
    }

    //Returns the asks aggregate of all price levels
    LevelsInfo get_asks() const{
        LevelsInfo info;
        for (auto &it : _asks){
            info.push_back(LevelsAgg(it.first, it.second.quantity));
        }
        return info;
    }

    /*Returns a snapshot of the orderbook */
    OrderbookInfo orderbook_info() const{
        return OrderbookInfo(get_bids(), get_asks());
    }
      
};


#endif
