# Order Matching Engine
This project is a demonstration of what the orderbook is, how orders are matched and effectively how price of an asset is determined. The matching algorithm utilized is the FIFO. It supports 5 order types (limit, stop, ioc, fok, mo).

ioc = immediate or cancel, 
fok = fill or kill, 
mo = market order, 
## Design
It is an header-only project, the only .cpp file is the test.

File details in `header`:
* `alias.hpp`: This is where all alias to a type is.
* `enums.hpp`: This is where all enumeration (`OrderDirection`, `OrderType`) is.
* `levels_agg.hpp`: This file contains `LevelsAgg` struct which contains quantity of orders at a price level.
* `order.hpp`: This file contains `Order` class, a representation of an realife order.
* `orderbook.hpp`: This file contains `OrderBook` class, the heart of this project. The `OrderBook` class contains the matching algorithm and some methods like last_price, spread, best_bid etc.
  This file also contains `OrderbookInfo` struct which is what is used to take a snapshot of the orderbook at an instance.
* `orderlevel.hpp`: It contains `OrderLevel` class, a queue like object that contains all orders at a level and the quantity of orders at that level.
* `time_sale.hpp`: It contains a `TimeSale` struct that stores the time and sales (i.e details of every transaction that occurs)

