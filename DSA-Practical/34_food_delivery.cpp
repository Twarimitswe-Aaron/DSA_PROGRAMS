/*
 * ============================================================================
 * FILE: 34_food_delivery.cpp
 * ============================================================================
 * FOOD DELIVERY SYSTEM (Swiggy/Zomato-like) - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Food delivery is a popular DSA practical exam project.
 * Tests graphs (delivery areas/routes), hash tables (restaurant menu),
 * queues (orders), linked lists (cart), and stacks (order history/undo).
 *
 * DATA STRUCTURES USED:
 *   1. Graph (Adjacency List) - Delivery Area / Zone Map
 *      - Vertices: delivery zones/locations
 *      - Edges: roads between zones with distance
 *   2. Hash Table (unordered_map) - Restaurant & Menu Catalog
 *      - O(1) restaurant lookup
 *      - Each restaurant has a menu (hash of items)
 *   3. Linked List - Customer Cart
 *      - Items added dynamically as user browses
 *      - O(1) insert at head, O(n) remove specific item
 *   4. Queue - Incoming Orders (FIFO)
 *      - Orders processed in arrival sequence
 *      - Restaurants receive orders in order
 *   5. Stack - Order History (LIFO)
 *      - Most recent order shown first
 *      - Supports order cancellation (undo)
 *
 * FUNCTIONALITIES:
 *   - Add restaurants & menu items (hash table)
 *   - Browse restaurants & menus
 *   - Add/remove items from cart (linked list)
 *   - Place order (cart -> order queue + history stack)
 *   - View order history (stack)
 *   - Cancel last order (stack)
 *   - Find delivery distance (graph Dijkstra)
 *   - Process next order (queue)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>
#include <vector>
#include <iomanip>
#include <sstream>
#include <climits>
#include <cstdlib>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct MenuItem {
    string itemId;
    string name;
    double price;
    string category;      // "Appetizer", "Main Course", "Dessert", "Beverage"

    MenuItem() : price(0.0) {}
    MenuItem(string id, string n, double p, string cat)
        : itemId(id), name(n), price(p), category(cat) {}
};

struct Restaurant {
    string restId;
    string name;
    string locationId;     // Zone/location for delivery graph
    string cuisine;
    double rating;
    unordered_map<string, MenuItem> menu;  // Hash: itemId -> MenuItem

    Restaurant() : rating(0.0) {}
    Restaurant(string id, string n, string loc, string cu, double r)
        : restId(id), name(n), locationId(loc), cuisine(cu), rating(r) {}
};

// Cart item linked list node
struct CartNode {
    string itemId;
    string itemName;
    double price;
    int quantity;
    string restId;
    CartNode* next;

    CartNode(string iid, string in, double p, int q, string rid)
        : itemId(iid), itemName(in), price(p), quantity(q),
          restId(rid), next(nullptr) {}
};

struct Order {
    string orderId;
    string customerName;
    string restName;
    string items;           // Summary string
    double total;
    string status;          // "PLACED", "PREPARING", "OUT_DELIVERY", "DELIVERED"
    string deliveryLocId;

    Order() : total(0.0) {}
    Order(string oid, string cn, string rn, string is, double t, string st, string dl)
        : orderId(oid), customerName(cn), restName(rn),
          items(is), total(t), status(st), deliveryLocId(dl) {}
};

// ============================================================================
// SECTION 2: GRAPH - DELIVERY ZONE MAP
// ============================================================================
class DeliveryGraph {
private:
    unordered_map<string, vector<pair<string, int>>> adjList; // zoneId -> (neighbor, dist)
    unordered_map<string, string> zoneNames;                   // zoneId -> name

public:
    void addZone(string id, string name) {
        zoneNames[id] = name;
        if (adjList.find(id) == adjList.end())
            adjList[id] = vector<pair<string, int>>();
    }

    void addRoad(string from, string to, int dist, bool twoWay = true) {
        adjList[from].push_back({to, dist});
        if (twoWay)
            adjList[to].push_back({from, dist});
    }

    // Dijkstra: shortest path distance
    int shortestDist(string from, string to) {
        if (adjList.find(from) == adjList.end() ||
            adjList.find(to) == adjList.end()) return -1;

        unordered_map<string, int> dist;
        for (auto& pair : adjList) dist[pair.first] = INT_MAX;
        dist[from] = 0;

        priority_queue<pair<int, string>,
                       vector<pair<int, string>>,
                       greater<pair<int, string>>> pq;
        pq.push({0, from});

        while (!pq.empty()) {
            auto top = pq.top(); pq.pop();
            int d = top.first;
            string u = top.second;
            if (d > dist[u]) continue;
            if (u == to) return d;
            for (auto& edge : adjList[u]) {
                int nd = d + edge.second;
                if (nd < dist[edge.first]) {
                    dist[edge.first] = nd;
                    pq.push({nd, edge.first});
                }
            }
        }
        return (dist[to] == INT_MAX) ? -1 : dist[to];
    }

    string getZoneName(string id) {
        auto it = zoneNames.find(id);
        return (it != zoneNames.end()) ? it->second : "Unknown";
    }

    void showZones() {
        cout << "  Delivery Zones (Graph):\n";
        for (auto& pair : zoneNames) {
            cout << "    " << pair.first << " - " << pair.second << "\n";
        }
    }
};

// ============================================================================
// SECTION 3: LINKED LIST - CART
// ============================================================================
class Cart {
private:
    CartNode* head;
    int count;

public:
    Cart() : head(nullptr), count(0) {}
    ~Cart() {
        CartNode* curr = head;
        while (curr) {
            CartNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void addItem(string itemId, string name, double price, int qty, string restId) {
        // Check if already in cart -> increase qty
        CartNode* curr = head;
        while (curr) {
            if (curr->itemId == itemId && curr->restId == restId) {
                curr->quantity += qty;
                return;
            }
            curr = curr->next;
        }
        CartNode* node = new CartNode(itemId, name, price, qty, restId);
        node->next = head;
        head = node;
        count++;
    }

    bool removeItem(string itemId, string restId) {
        CartNode* curr = head;
        CartNode* prev = nullptr;
        while (curr) {
            if (curr->itemId == itemId && curr->restId == restId) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    double getTotal() {
        double sum = 0;
        CartNode* curr = head;
        while (curr) {
            sum += curr->price * curr->quantity;
            curr = curr->next;
        }
        return sum;
    }

    // Get unique restaurant ID from cart (assumes all from same restaurant)
    string getRestId() {
        return head ? head->restId : "";
    }

    string getSummary() {
        stringstream ss;
        CartNode* curr = head;
        while (curr) {
            if (curr != head) ss << ", ";
            ss << curr->itemName << " x" << curr->quantity;
            curr = curr->next;
        }
        return ss.str();
    }

    void display() {
        if (!head) {
            cout << "    [Cart is empty]\n";
            return;
        }
        cout << "    " << left << setw(6) << "ID"
             << setw(30) << "Item"
             << setw(4) << "Qty"
             << setw(10) << "Price"
             << "Subtotal\n";
        cout << "    " << string(60, '-') << "\n";
        CartNode* curr = head;
        while (curr) {
            cout << "    " << left << setw(6) << curr->itemId
                 << setw(30) << curr->itemName
                 << setw(4) << curr->quantity
                 << "$" << setw(7) << fixed << setprecision(2) << curr->price
                 << "$" << curr->price * curr->quantity << "\n";
            curr = curr->next;
        }
        cout << "    " << string(60, '-') << "\n";
        cout << "    Total: $" << fixed << setprecision(2) << getTotal() << "\n";
    }

    void clear() {
        CartNode* curr = head;
        while (curr) {
            CartNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = nullptr;
        count = 0;
    }

    bool isEmpty() { return head == nullptr; }
    int getCount() { return count; }
};

// ============================================================================
// SECTION 4: MAIN FOOD DELIVERY SYSTEM
// ============================================================================
class FoodDeliverySystem {
private:
    unordered_map<string, Restaurant> restaurants;  // Hash: restId -> Restaurant
    DeliveryGraph deliveryGraph;                     // Graph delivery zones
    Cart cart;                                       // Linked list cart
    queue<Order> orderQueue;                         // Queue: incoming orders
    stack<Order> orderHistory;                       // Stack: order history
    int nextOrderNum;

public:
    FoodDeliverySystem() : nextOrderNum(2001) {
        seedData();
    }

    void seedData() {
        // Delivery zones
        deliveryGraph.addZone("Z01", "Downtown");
        deliveryGraph.addZone("Z02", "Uptown");
        deliveryGraph.addZone("Z03", "Midtown");
        deliveryGraph.addZone("Z04", "East Side");
        deliveryGraph.addZone("Z05", "West Side");

        // Roads between zones
        deliveryGraph.addRoad("Z01", "Z02", 3);
        deliveryGraph.addRoad("Z01", "Z03", 2);
        deliveryGraph.addRoad("Z01", "Z04", 4);
        deliveryGraph.addRoad("Z02", "Z03", 2);
        deliveryGraph.addRoad("Z03", "Z04", 3);
        deliveryGraph.addRoad("Z03", "Z05", 2);
        deliveryGraph.addRoad("Z04", "Z05", 4);

        // Restaurants
        Restaurant r1("R001", "Pizza Palace", "Z01", "Italian", 4.5);
        r1.menu["M101"] = MenuItem("M101", "Margherita Pizza", 9.99, "Main Course");
        r1.menu["M102"] = MenuItem("M102", "Pepperoni Pizza", 12.99, "Main Course");
        r1.menu["M103"] = MenuItem("M103", "Garlic Bread", 4.99, "Appetizer");
        r1.menu["M104"] = MenuItem("M104", "Tiramisu", 6.99, "Dessert");
        restaurants["R001"] = r1;

        Restaurant r2("R002", "Burger Barn", "Z02", "American", 4.3);
        r2.menu["M201"] = MenuItem("M201", "Classic Burger", 8.99, "Main Course");
        r2.menu["M202"] = MenuItem("M202", "Cheese Burger", 10.99, "Main Course");
        r2.menu["M203"] = MenuItem("M203", "French Fries", 3.99, "Appetizer");
        r2.menu["M204"] = MenuItem("M204", "Milkshake", 4.99, "Beverage");
        restaurants["R002"] = r2;

        Restaurant r3("R003", "Sushi House", "Z03", "Japanese", 4.7);
        r3.menu["M301"] = MenuItem("M301", "California Roll", 12.99, "Main Course");
        r3.menu["M302"] = MenuItem("M302", "Salmon Sashimi", 15.99, "Main Course");
        r3.menu["M303"] = MenuItem("M303", "Miso Soup", 3.99, "Appetizer");
        r3.menu["M304"] = MenuItem("M304", "Green Tea Ice Cream", 4.99, "Dessert");
        restaurants["R003"] = r3;

        Restaurant r4("R004", "Taco Town", "Z04", "Mexican", 4.2);
        r4.menu["M401"] = MenuItem("M401", "Chicken Tacos", 7.99, "Main Course");
        r4.menu["M402"] = MenuItem("M402", "Beef Burrito", 9.99, "Main Course");
        r4.menu["M403"] = MenuItem("M403", "Nachos", 5.99, "Appetizer");
        r4.menu["M404"] = MenuItem("M404", "Churros", 4.49, "Dessert");
        restaurants["R004"] = r4;

        Restaurant r5("R005", "Curry House", "Z05", "Indian", 4.6);
        r5.menu["M501"] = MenuItem("M501", "Butter Chicken", 13.99, "Main Course");
        r5.menu["M502"] = MenuItem("M502", "Biryani", 11.99, "Main Course");
        r5.menu["M503"] = MenuItem("M503", "Naan Bread", 2.99, "Appetizer");
        r5.menu["M504"] = MenuItem("M504", "Gulab Jamun", 4.99, "Dessert");
        restaurants["R005"] = r5;
    }

    // -------------------------------------------------------------------
    // BROWSE RESTAURANTS: Hash table iteration
    // -------------------------------------------------------------------
    void showRestaurants() {
        cout << "  Restaurants (Hash Table):\n";
        cout << "  " << left << setw(8) << "ID"
             << setw(25) << "Name"
             << setw(12) << "Cuisine"
             << setw(12) << "Zone"
             << "Rating\n";
        cout << "  " << string(60, '-') << "\n";
        for (auto& pair : restaurants) {
            Restaurant& r = pair.second;
            cout << "  " << left << setw(8) << r.restId
                 << setw(25) << r.name
                 << setw(12) << r.cuisine
                 << setw(12) << deliveryGraph.getZoneName(r.locationId)
                 << r.rating << "\n";
        }
    }

    // -------------------------------------------------------------------
    // SHOW MENU: Hash table O(1) restaurant + O(n) menu
    // -------------------------------------------------------------------
    void showMenu(string restId) {
        auto it = restaurants.find(restId);
        if (it == restaurants.end()) {
            cout << "  [ERROR] Restaurant not found.\n";
            return;
        }
        Restaurant& r = it->second;
        cout << "  Menu for " << r.name << " (" << r.cuisine << "):\n";
        cout << "  " << left << setw(8) << "ID"
             << setw(30) << "Item"
             << setw(15) << "Category"
             << "Price\n";
        cout << "  " << string(58, '-') << "\n";
        for (auto& mpair : r.menu) {
            MenuItem& m = mpair.second;
            cout << "  " << left << setw(8) << m.itemId
                 << setw(30) << m.name
                 << setw(15) << m.category
                 << "$" << fixed << setprecision(2) << m.price << "\n";
        }
    }

    // -------------------------------------------------------------------
    // ADD TO CART: Linked list O(1)
    // -------------------------------------------------------------------
    void addToCart(string restId, string itemId, int qty) {
        auto rit = restaurants.find(restId);
        if (rit == restaurants.end()) {
            cout << "  [ERROR] Restaurant not found.\n";
            return;
        }
        auto mit = rit->second.menu.find(itemId);
        if (mit == rit->second.menu.end()) {
            cout << "  [ERROR] Menu item not found.\n";
            return;
        }
        cart.addItem(itemId, mit->second.name, mit->second.price, qty, restId);
        cout << "  [OK] Added " << qty << "x " << mit->second.name
             << " to cart.\n";
    }

    // -------------------------------------------------------------------
    // PLACE ORDER: Cart -> Queue + Stack, then clear cart
    // -------------------------------------------------------------------
    void placeOrder(string customerName, string deliveryZoneId) {
        if (cart.isEmpty()) {
            cout << "  [ERROR] Cart is empty.\n";
            return;
        }
        string restId = cart.getRestId();
        auto rit = restaurants.find(restId);
        string restName = (rit != restaurants.end()) ? rit->second.name : "Unknown";

        double total = cart.getTotal();
        string summary = cart.getSummary();
        string orderId = "FD" + to_string(nextOrderNum++);

        Order order(orderId, customerName, restName, summary, total, "PLACED", deliveryZoneId);
        orderQueue.push(order);     // Queue: process in order
        orderHistory.push(order);   // Stack: history

        cout << "  [ORDER PLACED] " << orderId << "\n";
        cout << "  Customer: " << customerName << "\n";
        cout << "  Restaurant: " << restName << "\n";
        cout << "  Items: " << summary << "\n";
        cout << "  Total: $" << fixed << setprecision(2) << total << "\n";
        cart.clear();
    }

    // -------------------------------------------------------------------
    // PROCESS ORDER: FIFO queue
    // -------------------------------------------------------------------
    void processNextOrder() {
        if (orderQueue.empty()) {
            cout << "  [INFO] No pending orders.\n";
            return;
        }
        Order o = orderQueue.front(); orderQueue.pop();
        cout << "  [PROCESSING] " << o.orderId << " | " << o.restName
             << " | " << o.items << "\n";
        // Update to preparing
        o.status = "PREPARING";
        o.status = "OUT_DELIVERY";
        cout << "  [OUT FOR DELIVERY] Order " << o.orderId << "\n";
    }

    // -------------------------------------------------------------------
    // ORDER HISTORY: Stack - most recent first
    // -------------------------------------------------------------------
    void showOrderHistory() {
        if (orderHistory.empty()) {
            cout << "  [INFO] No order history.\n";
            return;
        }
        stack<Order> temp = orderHistory;
        cout << "  Order History (Most Recent First - Stack):\n";
        while (!temp.empty()) {
            Order o = temp.top(); temp.pop();
            cout << "  " << o.orderId << " | " << o.customerName
                 << " | " << o.restName << " | $" << fixed << setprecision(2) << o.total
                 << " | " << o.status << "\n";
            cout << "     Items: " << o.items << "\n";
        }
    }

    // -------------------------------------------------------------------
    // CANCEL LAST ORDER: Stack pop
    // -------------------------------------------------------------------
    void cancelLastOrder() {
        if (orderHistory.empty()) {
            cout << "  [INFO] No orders to cancel.\n";
            return;
        }
        Order last = orderHistory.top(); orderHistory.pop();
        cout << "  [CANCELLED] " << last.orderId
             << " (" << last.customerName << " - " << last.restName << ")\n";
    }

    // -------------------------------------------------------------------
    // DELIVERY DISTANCE: Graph Dijkstra
    // -------------------------------------------------------------------
    void showDeliveryDistance(string restId, string zoneId) {
        auto rit = restaurants.find(restId);
        if (rit == restaurants.end()) {
            cout << "  [ERROR] Restaurant not found.\n";
            return;
        }
        int dist = deliveryGraph.shortestDist(rit->second.locationId, zoneId);
        if (dist < 0)
            cout << "  [INFO] No route available.\n";
        else
            cout << "  Delivery distance from " << rit->second.name
                 << " to " << deliveryGraph.getZoneName(zoneId)
                 << ": " << dist << " km\n";
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   FOOD DELIVERY SYSTEM\n";
        cout << "   Data Structures: Graph (Zones),\n";
        cout << "   Hash Table (Restaurants), Linked List (Cart),\n";
        cout << "   Queue (Orders), Stack (History)\n";
        cout << "=============================================\n\n";

        int choice, qty;
        string restId, itemId, custName, zoneId;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [HASH TABLE - Restaurants]\n";
            cout << "    1. Browse Restaurants\n";
            cout << "    2. View Restaurant Menu\n";
            cout << "  [GRAPH - Delivery Zones]\n";
            cout << "    3. Show Delivery Zones\n";
            cout << "    4. Check Delivery Distance\n";
            cout << "  [LINKED LIST - Cart]\n";
            cout << "    5. Add Item to Cart\n";
            cout << "    6. Remove from Cart\n";
            cout << "    7. View Cart\n";
            cout << "  [ORDERING]\n";
            cout << "    8. Place Order (Cart -> Queue + Stack)\n";
            cout << "    9. Process Next Order (Queue)\n";
            cout << "   10. Order History (Stack - Most Recent)\n";
            cout << "   11. Cancel Last Order (Stack Undo)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: showRestaurants(); break;
                case 2:
                    cout << "  Restaurant ID: "; getline(cin, restId);
                    showMenu(restId);
                    break;
                case 3: deliveryGraph.showZones(); break;
                case 4:
                    cout << "  Restaurant ID: "; getline(cin, restId);
                    cout << "  Delivery Zone ID: "; getline(cin, zoneId);
                    showDeliveryDistance(restId, zoneId);
                    break;
                case 5:
                    cout << "  Restaurant ID: "; getline(cin, restId);
                    cout << "  Item ID: "; getline(cin, itemId);
                    cout << "  Quantity: "; cin >> qty; cin.ignore();
                    addToCart(restId, itemId, qty);
                    break;
                case 6:
                    cout << "  Item ID: "; getline(cin, itemId);
                    cout << "  Restaurant ID: "; getline(cin, restId);
                    if (cart.removeItem(itemId, restId))
                        cout << "  [OK] Removed from cart.\n";
                    else
                        cout << "  [ERROR] Item not in cart.\n";
                    break;
                case 7: cart.display(); break;
                case 8:
                    cout << "  Customer Name: "; getline(cin, custName);
                    cout << "  Delivery Zone ID: "; getline(cin, zoneId);
                    placeOrder(custName, zoneId);
                    break;
                case 9: processNextOrder(); break;
                case 10: showOrderHistory(); break;
                case 11: cancelLastOrder(); break;
                case 0: cout << "  Thanks for ordering! Goodbye.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    FoodDeliverySystem system;
    system.run();
    return 0;
}
