# Auction

This is a simple demo for matching trade orders, where buy and sell orders follow the price-time priority matching rules.

## Rules for Price-Time Priority in Matching Buy and Sell Orders

**The rules of price-time priority** are commonly used in securities trading systems. The basic principles are:

### **1. Price Priority**
- For buy orders, higher prices are matched first (buyers are willing to pay more).  
- For sell orders, lower prices are matched first (sellers are willing to accept less).  

### **2. Time Priority**
- If prices are the same, orders are matched based on the time they were placed, with earlier orders being prioritized.  

---

### **Detailed Explanation**
- The system maintains a **buy order book** and a **sell order book**, each sorted by price and time priority.  
- Matching occurs when the highest buy price in the buy order book is greater than or equal to the lowest sell price in the sell order book.  
- When `buy price >= sell price`, a trade is executed, and the orders are matched.  

---

### **Example**
#### **Initial Order Book**
| **Buy Orders (Descending by Price)** | **Sell Orders (Ascending by Price)** |
|---------------------------------------|---------------------------------------|
| Price: 105, Quantity: 100, Time: 09:00:01 | Price: 106, Quantity: 50, Time: 09:00:05 |
| Price: 104, Quantity: 200, Time: 09:00:02 | Price: 107, Quantity: 100, Time: 09:00:06 |

#### **New Orders**
1. **New Buy Order**: Price 106, Quantity 50, Time 09:00:03  
   - The system inserts this order into the buy order book and attempts to match it.  
   - Since the buy price (106) >= sell price (106), a trade is executed.

##### **Matching Result**
- **Matched Orders**: Buy Order (106, 50) with Sell Order (106, 50).  
- **Trade Price**: 106, **Trade Quantity**: 50.  
- The updated order book becomes:

| **Buy Orders (Descending by Price)** | **Sell Orders (Ascending by Price)** |
|---------------------------------------|---------------------------------------|
| Price: 105, Quantity: 100, Time: 09:00:01 | Price: 107, Quantity: 100, Time: 09:00:06 |
| Price: 104, Quantity: 200, Time: 09:00:02 |                                       |

2. **New Sell Order**: Price 104, Quantity 150, Time 09:00:04  
   - The system inserts this order into the sell order book and attempts to match it.  
   - Since the sell price (104) <= buy price (105), a trade is executed.

##### **Matching Result**
- **Matched Orders**: Buy Order (105, 100) with Sell Order (104, 150).  
- **Trade Price**: 105, **Trade Quantity**: 100 (matches the buy order quantity).  
- The updated order book becomes:

| **Buy Orders (Descending by Price)** | **Sell Orders (Ascending by Price)** |
|---------------------------------------|---------------------------------------|
| Price: 104, Quantity: 200, Time: 09:00:02 | Price: 104, Quantity: 50, Time: 09:00:04 |
|                                       | Price: 107, Quantity: 100, Time: 09:00:06 |

---

### **Summary of Rules**
1. Matching follows the **price-time priority principle**:  
   - Higher buy prices and lower sell prices are matched first.  
   - When prices are equal, earlier orders take precedence.  
2. Remaining unmatched orders are kept in the order book for future matches.  

## Functionality of Classes:
* Order: Represents an order and contains attributes such as OrderId, price, quantity, and more.
* OrderQueue: Manages orders with the same price. It includes a first-in, first-out (FIFO) queue to reflect time priority when matching orders with the same price. Additionally, it contains a map with OrderId as the key to manage orders, which facilitates order cancellations based on OrderId. It also maintains a synchronized total quantity of all orders.
* OrderBook: Represents an order book, implemented as a map where the key is the price and the value is an OrderQueue. This structure allows access to orders based on price priority.
* ContinousAuction: The order processing module that supports both order matching and order cancellation.
