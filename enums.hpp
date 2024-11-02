#ifndef ENUMS_HPP
#define ENUMS_HPP

/*Type of order being placed*/
enum class OrderType{
    fok, // Fill Or Kill
    ioc, // immediate or cancel
    limit,
    stop,
    mo, // market order
};
/*Direction of Order*/
enum class OrderDirection{
    buy,
    sell
};

#endif
