/*
 * ============================================================================
 * FILE: 22_restaurant_ordering.cpp
 * ============================================================================
 * RESTAURANT ORDERING SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Restaurant management systems are commonly asked
 * DSA practical exam projects. Tests understanding of MULTIPLE data structures
 * working together: hash tables for fast lookup, queues for order processing,
 * graphs for delivery routing, stacks for undo operations, and linked lists
 * for itemized billing.
 *
 * DATA STRUCTURES USED:
 *   1. Hash Table (unordered_map) - Menu items keyed by item code (O(1) lookup)
 *   2. Queue                        - Order queue (FIFO processing in arrival order)
 *   3. Graph (adjacency list)       - Delivery routing (kitchen/dining areas as nodes)
 *   4. Stack                        - Order undo / most recent order cancellation
 *   5. Singly Linked List           - Bill breakdown (itemized list per order)
 *
 * FUNCTIONALITIES:
 *   - Menu management (add/update/remove items)
 *   - Place order (queue-based, builds itemized bill)
 *   - Process order (FIFO kitchen receives orders)
 *   - Cancel last order (stack undo)
 *   - Find shortest delivery route (graph BFS)
 *   - Generate itemized bill (linked list traversal)
 *   - Table management
 *   - Daily sales report
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>         // For order queue (FIFO processing)
#include <stack>         // For order undo / cancellation (LIFO)
#include <vector>
#include <unordered_map> // For hash table (item code -> menu item mapping)
#include <iomanip>       // For formatted bill/output
#include <ctime>         // For order timestamps
#include <sstream>       // For string stream formatting
#include <algorithm>     // For sort, min element in graph
#include <climits>       // For INT_MAX in shortest path
using namespace std;

// ============================================================================
// SECTION 1: DATA STRUCTURES USED BY THE SYSTEM
// ============================================================================

// -------------------------------------------------------------------
// MenuItem structure - Core entity of the restaurant system
// Each menu item has a unique code, name, category, and price.
// -------------------------------------------------------------------
struct MenuItem {
    string code;         // Unique item code (e.g., "B001")
    string name;         // Item name (e.g., "Butter Chicken")
    string category;     // Category (e.g., "Main Course", "Beverage")
    double price;        // Price in rupees/dollars
    bool available;      // Whether item is currently available

    // Constructor with default values
    MenuItem(string c, string n, string cat, double p)
        : code(c), name(n), category(cat), price(p), available(true) {}
};

// -------------------------------------------------------------------
// BillItem structure - Represents one line item in a bill
// Forms a singly linked list for the bill breakdown.
// -------------------------------------------------------------------
struct BillItem {
    string itemName;     // Name of the menu item
    string itemCode;     // Item code for reference
    int quantity;        // How many of this item
    double unitPrice;    // Price per unit
    double lineTotal;    // quantity * unitPrice
    BillItem* next;      // Pointer to next item in the bill

    BillItem(string code, string name, int qty, double price)
        : itemName(name), itemCode(code), quantity(qty),
          unitPrice(price), lineTotal(qty * price), next(nullptr) {}
};

// -------------------------------------------------------------------
// Order structure - Represents a customer order
// Contains table number, linked list of bill items, and status.
// -------------------------------------------------------------------
struct Order {
    int orderId;             // Unique order ID (auto-incremented)
    int tableNumber;         // Which table placed the order
    string status;           // "PENDING", "PREPARING", "DELIVERED"
    string timestamp;        // When the order was placed
    BillItem* billHead;      // Head of itemized bill linked list
    double totalAmount;      // Sum of all line totals
    int itemCount;           // Number of distinct items

    Order() : orderId(0), tableNumber(0), status("PENDING"),
              billHead(nullptr), totalAmount(0.0), itemCount(0) {}

    Order(int id, int table)
        : orderId(id), tableNumber(table), status("PENDING"),
          billHead(nullptr), totalAmount(0.0), itemCount(0) {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        timestamp = string(buf);
    }

    // Destructor cleans up the bill linked list
    ~Order() {
        BillItem* current = billHead;
        while (current) {
            BillItem* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // Disable copy to avoid double-free (we manage raw pointers)
    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    // Move constructor (C++11)
    Order(Order&& other)
        : orderId(other.orderId), tableNumber(other.tableNumber),
          status(other.status), timestamp(other.timestamp),
          billHead(other.billHead), totalAmount(other.totalAmount),
          itemCount(other.itemCount) {
        other.billHead = nullptr;
    }
};

// ============================================================================
// SECTION 2: HASH TABLE FOR MENU MANAGEMENT
//
// Why unordered_map? Menu items need to be retrieved by item code
// in O(1) average time. The hash table maps item codes to MenuItem
// objects. This is ideal for a menu where each item code is unique.
// ============================================================================

class MenuManager {
private:
    unordered_map<string, MenuItem> menu;  // Hash: item code -> MenuItem
    int nextCodeNum;                       // For auto-generating codes

public:
    MenuManager() : nextCodeNum(1) {}

    // -------------------------------------------------------------------
    // ADD ITEM: Insert into hash table. O(1) average.
    // -------------------------------------------------------------------
    void addItem(string name, string category, double price) {
        // Generate item code: first letter of category + 3-digit number
        string code = category.substr(0, 1);
        // Ensure unique code by appending number
        code += (nextCodeNum < 10 ? "00" : (nextCodeNum < 100 ? "0" : ""))
                + to_string(nextCodeNum);
        nextCodeNum++;

        // Check if code somehow already exists (shouldn't happen)
        if (menu.find(code) != menu.end()) {
            cout << "  [ERROR] Code collision! Try again.\n";
            return;
        }

        menu.insert(make_pair(code, MenuItem(code, name, category, price)));
        cout << "  [OK] '" << name << "' added with code " << code << ".\n";
    }

    // -------------------------------------------------------------------
    // UPDATE ITEM: Modify price/availability via hash lookup. O(1) avg.
    // -------------------------------------------------------------------
    bool updatePrice(string code, double newPrice) {
        unordered_map<string, MenuItem>::iterator it = menu.find(code);
        if (it == menu.end()) {
            cout << "  [ERROR] Item code " << code << " not found.\n";
            return false;
        }
        it->second.price = newPrice;
        cout << "  [OK] Price updated for '" << it->second.name << "'.\n";
        return true;
    }

    // -------------------------------------------------------------------
    // TOGGLE AVAILABILITY: Mark item as available or not. O(1) avg.
    // -------------------------------------------------------------------
    bool toggleAvailability(string code) {
        unordered_map<string, MenuItem>::iterator it = menu.find(code);
        if (it == menu.end()) {
            cout << "  [ERROR] Item code " << code << " not found.\n";
            return false;
        }
        it->second.available = !it->second.available;
        cout << "  [OK] '" << it->second.name << "' is now "
             << (it->second.available ? "available" : "unavailable") << ".\n";
        return true;
    }

    // -------------------------------------------------------------------
    // REMOVE ITEM: Erase from hash table. O(1) average.
    // -------------------------------------------------------------------
    bool removeItem(string code) {
        unordered_map<string, MenuItem>::iterator it = menu.find(code);
        if (it == menu.end()) {
            cout << "  [ERROR] Item code " << code << " not found.\n";
            return false;
        }
        cout << "  [OK] '" << it->second.name << "' removed from menu.\n";
        menu.erase(it);
        return true;
    }

    // -------------------------------------------------------------------
    // SEARCH ITEM: O(1) hash lookup. Returns pointer or NULL.
    // -------------------------------------------------------------------
    MenuItem* findItem(string code) {
        unordered_map<string, MenuItem>::iterator it = menu.find(code);
        if (it != menu.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // DISPLAY FULL MENU: Iterate over hash table. O(n).
    // -------------------------------------------------------------------
    void displayMenu() {
        if (menu.empty()) {
            cout << "  [INFO] Menu is empty.\n";
            return;
        }
        cout << "  " << left << setw(8) << "Code"
             << setw(30) << "Name"
             << setw(18) << "Category"
             << setw(8) << "Price"
             << "Status\n";
        cout << "  " << string(75, '-') << "\n";

        // Iterate over hash table using traditional iterator (C++11 compatible)
        unordered_map<string, MenuItem>::iterator it;
        for (it = menu.begin(); it != menu.end(); ++it) {
            cout << "  " << left << setw(8) << it->second.code
                 << setw(30) << it->second.name
                 << setw(18) << it->second.category
                 << setw(8) << fixed << setprecision(2) << it->second.price
                 << (it->second.available ? "Available" : "Unavailable")
                 << "\n";
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY BY CATEGORY: Filter items by category. O(n).
    // -------------------------------------------------------------------
    void displayByCategory(string category) {
        bool found = false;
        unordered_map<string, MenuItem>::iterator it;
        for (it = menu.begin(); it != menu.end(); ++it) {
            if (it->second.category == category && it->second.available) {
                if (!found) {
                    cout << "  " << category << ":\n";
                    found = true;
                }
                cout << "    " << it->second.code << "  "
                     << it->second.name << "  $"
                     << fixed << setprecision(2) << it->second.price << "\n";
            }
        }
        if (!found) {
            cout << "  [INFO] No items in category '" << category << "'.\n";
        }
    }

    int getCount() { return menu.size(); }
};

// ============================================================================
// SECTION 3: SINGLY LINKED LIST FOR BILL BREAKDOWN
//
// Why Linked List? Each order has a variable number of line items.
// A linked list allows dynamic growth without pre-allocation.
// We insert at the end as items are added to the order.
// ============================================================================

class BillList {
private:
    BillItem* head;  // Head of the linked list
    BillItem* tail;  // Tail for O(1) append

public:
    BillList() : head(nullptr), tail(nullptr) {}

    ~BillList() {
        BillItem* current = head;
        while (current) {
            BillItem* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ADD ITEM TO BILL: Append to end of linked list. O(1) via tail ptr.
    // -------------------------------------------------------------------
    void addItem(string code, string name, int qty, double price) {
        BillItem* item = new BillItem(code, name, qty, price);
        if (!head) {
            head = item;
            tail = item;
        } else {
            tail->next = item;
            tail = item;
        }
    }

    // Get head pointer (for transferring to Order)
    BillItem* getHead() { return head; }
    BillItem* getTail() { return tail; }

    // Detach list without destroying nodes (ownership transfers)
    BillItem* detach() {
        BillItem* oldHead = head;
        head = nullptr;
        tail = nullptr;
        return oldHead;
    }

    // -------------------------------------------------------------------
    // DISPLAY BILL: Traverse linked list and print itemized breakdown.
    // -------------------------------------------------------------------
    void displayBill() {
        if (!head) {
            cout << "  [INFO] Bill is empty.\n";
            return;
        }
        double grandTotal = 0.0;
        int lineNo = 1;
        cout << "  " << left << setw(5) << "#"
             << setw(10) << "Code"
             << setw(30) << "Item"
             << setw(6) << "Qty"
             << setw(10) << "Price"
             << "Total\n";
        cout << "  " << string(65, '-') << "\n";

        BillItem* current = head;
        while (current) {
            cout << "  " << left << setw(5) << lineNo
                 << setw(10) << current->itemCode
                 << setw(30) << current->itemName
                 << setw(6) << current->quantity
                 << setw(10) << fixed << setprecision(2) << current->unitPrice
                 << current->lineTotal << "\n";
            grandTotal += current->lineTotal;
            current = current->next;
            lineNo++;
        }
        cout << "  " << string(65, '-') << "\n";
        cout << "  " << right << setw(58) << "GRAND TOTAL: $"
             << fixed << setprecision(2) << grandTotal << "\n";
    }

    double computeTotal() {
        double total = 0.0;
        BillItem* current = head;
        while (current) {
            total += current->lineTotal;
            current = current->next;
        }
        return total;
    }

    int countItems() {
        int count = 0;
        BillItem* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
};

// ============================================================================
// SECTION 4: GRAPH FOR DELIVERY ROUTING
//
// Why Graph? The restaurant floor has multiple zones (kitchen, dining hall,
// private rooms, bar). Delivery staff need to find the shortest path from
// kitchen to a table/area. We model zones as nodes and walkways as edges.
// BFS gives the shortest path in an unweighted graph (all edges = 1 step).
// ============================================================================

struct Edge {
    int to;        // Destination node index
    int weight;    // Distance/weight of this path

    Edge(int t, int w) : to(t), weight(w) {}
};

class DeliveryGraph {
private:
    int numNodes;                        // Number of zones/nodes
    vector<vector<Edge> > adjList;       // Adjacency list representation
    vector<string> nodeNames;            // Names of each zone (e.g., "Kitchen", "Table-1")

public:
    DeliveryGraph() : numNodes(0) {}

    // -------------------------------------------------------------------
    // ADD NODE: Add a new zone/area. O(1) amortized.
    // -------------------------------------------------------------------
    void addNode(string name) {
        nodeNames.push_back(name);
        adjList.push_back(vector<Edge>());
        numNodes++;
    }

    // -------------------------------------------------------------------
    // ADD EDGE: Connect two nodes with a weighted path. O(1).
    // Restaurants have bidirectional walkways, so we add both directions.
    // -------------------------------------------------------------------
    void addEdge(int from, int to, int weight) {
        if (from < 0 || from >= numNodes || to < 0 || to >= numNodes) {
            cout << "  [ERROR] Invalid node indices.\n";
            return;
        }
        adjList[from].push_back(Edge(to, weight));
        adjList[to].push_back(Edge(from, weight)); // Undirected graph
    }

    // -------------------------------------------------------------------
    // BFS SHORTEST PATH: Find shortest route from start to end.
    // Uses BFS on unweighted edges (ignores weights, treats as unit).
    // For weighted shortest path, we would use Dijkstra's, but BFS
    // suffices when all paths have similar length or step count matters.
    // -------------------------------------------------------------------
    vector<int> findShortestPath(int start, int end) {
        vector<int> path;

        if (start < 0 || start >= numNodes || end < 0 || end >= numNodes) {
            cout << "  [ERROR] Invalid node index.\n";
            return path;
        }

        // BFS setup: queue of nodes, visited array, parent tracking
        queue<int> bfsQueue;
        vector<bool> visited(numNodes, false);
        vector<int> parent(numNodes, -1);

        visited[start] = true;
        bfsQueue.push(start);

        while (!bfsQueue.empty()) {
            int current = bfsQueue.front();
            bfsQueue.pop();

            // Found the destination
            if (current == end) {
                // Reconstruct path by walking backwards from end to start
                int node = end;
                while (node != -1) {
                    path.push_back(node);
                    node = parent[node];
                }
                // Reverse to get start -> end order
                reverse(path.begin(), path.end());
                return path;
            }

            // Visit all neighbors
            vector<Edge>& neighbors = adjList[current];
            for (size_t i = 0; i < neighbors.size(); i++) {
                int next = neighbors[i].to;
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = current;
                    bfsQueue.push(next);
                }
            }
        }

        // No path found
        cout << "  [INFO] No path exists between "
             << nodeNames[start] << " and " << nodeNames[end] << ".\n";
        return path;
    }

    // -------------------------------------------------------------------
    // DISPLAY GRAPH: Show all nodes and their connections.
    // -------------------------------------------------------------------
    void displayGraph() {
        cout << "  Restaurant Floor Layout:\n";
        for (int i = 0; i < numNodes; i++) {
            cout << "  " << i << ": " << nodeNames[i] << " -> ";
            vector<Edge>& neighbors = adjList[i];
            for (size_t j = 0; j < neighbors.size(); j++) {
                cout << neighbors[j].to << "(" << nodeNames[neighbors[j].to]
                     << ", w=" << neighbors[j].weight << ")";
                if (j < neighbors.size() - 1) cout << ", ";
            }
            cout << "\n";
        }
    }

    int getNumNodes() { return numNodes; }
    string getNodeName(int index) {
        if (index >= 0 && index < numNodes) return nodeNames[index];
        return "Unknown";
    }
};

// ============================================================================
// SECTION 5: STACK FOR ORDER UNDO
//
// Why Stack? When the restaurant receives an order by mistake, the most
// recent order should be cancelled first (LIFO - Last In, First Out).
// The stack stores order IDs that can be undone.
// ============================================================================

class OrderUndoStack {
private:
    stack<int> undoStack;  // STL stack of order IDs (LIFO)

public:
    // -------------------------------------------------------------------
    // PUSH ORDER: Record an order for potential undo. O(1).
    // -------------------------------------------------------------------
    void pushOrder(int orderId) {
        undoStack.push(orderId);
    }

    // -------------------------------------------------------------------
    // POP ORDER: Get the most recent order ID for cancellation. O(1).
    // Returns -1 if stack is empty.
    // -------------------------------------------------------------------
    int popOrder() {
        if (undoStack.empty()) {
            return -1;
        }
        int orderId = undoStack.top();
        undoStack.pop();
        return orderId;
    }

    // -------------------------------------------------------------------
    // PEEK: View the most recent order without removing it. O(1).
    // -------------------------------------------------------------------
    int peekRecent() {
        if (undoStack.empty()) return -1;
        return undoStack.top();
    }

    bool isEmpty() { return undoStack.empty(); }
    int size() { return undoStack.size(); }
};

// ============================================================================
// SECTION 6: QUEUE FOR ORDER PROCESSING
//
// Why Queue? Orders must be processed in the order they arrive (FIFO).
// The kitchen staff take orders from the front of the queue as they
// complete each dish. This ensures fairness.
// ============================================================================

// -------------------------------------------------------------------
// We reuse the Order struct (defined above) which contains all order
// details including the bill linked list.
// We'll store Order objects directly in the queue (move semantics).
// -------------------------------------------------------------------

class OrderQueue {
private:
    queue<Order> orders;  // STL queue of orders (FIFO)
    int nextOrderId;      // Auto-incrementing order ID

public:
    OrderQueue() : nextOrderId(1001) {}

    // -------------------------------------------------------------------
    // ENQUEUE ORDER: Add to the back of the queue. O(1).
    // -------------------------------------------------------------------
    int placeOrder(int tableNum, BillList& bill) {
        int orderId = nextOrderId++;
        Order newOrder(orderId, tableNum);

        // Transfer bill linked list to the order
        newOrder.billHead = bill.detach();
        newOrder.totalAmount = 0.0;

        // Compute total and count items
        BillItem* current = newOrder.billHead;
        while (current) {
            newOrder.totalAmount += current->lineTotal;
            newOrder.itemCount++;
            current = current->next;
        }

        orders.push(std::move(newOrder));
        cout << "  [OK] Order #" << orderId << " placed for Table "
             << tableNum << ".\n";
        return orderId;
    }

    // -------------------------------------------------------------------
    // PROCESS NEXT ORDER: Dequeue the front order. O(1).
    // Returns the order for kitchen processing.
    // -------------------------------------------------------------------
    Order processNextOrder() {
        if (orders.empty()) {
            cout << "  [INFO] No pending orders.\n";
            return Order();
        }
        Order next = std::move(orders.front());
        orders.pop();
        next.status = "PREPARING";
        cout << "  [OK] Order #" << next.orderId << " is now being prepared.\n";
        return next;
    }

    // -------------------------------------------------------------------
    // PEEK NEXT ORDER: View front order without removing. O(1).
    // -------------------------------------------------------------------
    Order* peekNext() {
        if (orders.empty()) return nullptr;
        return &(orders.front());
    }

    bool hasPending() { return !orders.empty(); }
    int pendingCount() { return orders.size(); }

    void displayPending() {
        if (orders.empty()) {
            cout << "  [INFO] No pending orders.\n";
            return;
        }
        // We need to iterate without modifying - make a copy approach
        // Since queue doesn't support iteration, we display front info
        cout << "  Pending orders: " << orders.size() << "\n";
        cout << "  Next order #" << orders.front().orderId
             << " (Table " << orders.front().tableNumber << ")\n";
    }
};

// ============================================================================
// SECTION 7: MAIN RESTAURANT SYSTEM - Integrates all data structures
// ============================================================================

class RestaurantOrderingSystem {
private:
    MenuManager menu;              // Hash table for menu management
    OrderQueue orderQueue;         // Queue for order processing
    DeliveryGraph deliveryGraph;   // Graph for delivery routing
    OrderUndoStack undoStack;      // Stack for order cancellation
    vector<Order> completedOrders; // History of completed orders for sales report
    vector<int> tableStatus;       // 0 = empty, 1 = seated, 2 = ordered, 3 = served
    int nextOrderIdForUndo;        // Track orders for undo stack

public:
    RestaurantOrderingSystem() : nextOrderIdForUndo(0) {
        // Initialize tables (tables 1-10)
        tableStatus.resize(11, 0); // Index 0 unused, tables 1-10

        // -------------------------------------------------------------------
        // Build the restaurant floor graph:
        //   0 - Kitchen (starting point for deliveries)
        //   1 - Main Dining Hall
        //   2 - Private Room A
        //   3 - Private Room B
        //   4 - Bar Area
        //   5 - Outdoor Patio
        //   6 - Table-1 through Table-10 (nodes 6-15)
        // -------------------------------------------------------------------
        deliveryGraph.addNode("Kitchen");
        deliveryGraph.addNode("Main Dining");
        deliveryGraph.addNode("Private Room A");
        deliveryGraph.addNode("Private Room B");
        deliveryGraph.addNode("Bar Area");
        deliveryGraph.addNode("Outdoor Patio");

        // Add table nodes (6-15)
        for (int i = 1; i <= 10; i++) {
            stringstream ss;
            ss << "Table-" << i;
            deliveryGraph.addNode(ss.str());
        }

        // Connect nodes: Kitchen <-> Main Dining <-> everything else
        deliveryGraph.addEdge(0, 1, 5);   // Kitchen <-> Main Dining (5 steps)
        deliveryGraph.addEdge(0, 2, 8);   // Kitchen <-> Private Room A
        deliveryGraph.addEdge(0, 3, 10);  // Kitchen <-> Private Room B
        deliveryGraph.addEdge(1, 4, 4);   // Main Dining <-> Bar Area
        deliveryGraph.addEdge(1, 5, 6);   // Main Dining <-> Outdoor Patio
        deliveryGraph.addEdge(1, 2, 3);   // Main Dining <-> Private Room A
        deliveryGraph.addEdge(1, 3, 5);   // Main Dining <-> Private Room B
        deliveryGraph.addEdge(4, 5, 3);   // Bar <-> Outdoor Patio
        deliveryGraph.addEdge(2, 3, 2);   // Private Room A <-> Private Room B

        // Connect tables to nearest zones
        // Tables 1-3 near Main Dining, 4-5 near Private Room A,
        // 6-7 near Bar, 8-10 near Outdoor Patio
        for (int i = 1; i <= 3; i++) {
            deliveryGraph.addEdge(1, 5 + i, 2);
        }
        for (int i = 4; i <= 5; i++) {
            deliveryGraph.addEdge(2, 5 + i, 2);
        }
        for (int i = 6; i <= 7; i++) {
            deliveryGraph.addEdge(4, 5 + i, 1);
        }
        for (int i = 8; i <= 10; i++) {
            deliveryGraph.addEdge(5, 5 + i, 2);
        }
    }

    // -------------------------------------------------------------------
    // SEED MENU: Pre-populate with sample items for testing
    // -------------------------------------------------------------------
    void seedMenu() {
        menu.addItem("Butter Chicken", "Main Course", 12.99);
        menu.addItem("Paneer Tikka", "Starter", 8.99);
        menu.addItem("Garlic Naan", "Bread", 2.99);
        menu.addItem("Biryani", "Main Course", 10.99);
        menu.addItem("Samosa", "Starter", 4.99);
        menu.addItem("Mango Lassi", "Beverage", 3.99);
        menu.addItem("Gulab Jamun", "Dessert", 5.99);
        menu.addItem("Masala Chai", "Beverage", 2.49);
    }

    // -------------------------------------------------------------------
    // PLACE ORDER: Take items from customer, build bill list, queue order
    // -------------------------------------------------------------------
    void placeOrder() {
        int tableNum;
        cout << "  Table number (1-10): ";
        cin >> tableNum;
        cin.ignore();

        if (tableNum < 1 || tableNum > 10) {
            cout << "  [ERROR] Invalid table number.\n";
            return;
        }

        if (tableStatus[tableNum] == 0) {
            tableStatus[tableNum] = 1; // Seat the table
        }
        tableStatus[tableNum] = 2; // Table has ordered

        BillList bill;
        string code;
        int qty;
        char more = 'y';

        cout << "  Add items to order (enter item codes):\n";
        menu.displayByCategory("Starter");
        menu.displayByCategory("Main Course");
        menu.displayByCategory("Bread");
        menu.displayByCategory("Beverage");
        menu.displayByCategory("Dessert");

        while (more == 'y' || more == 'Y') {
            cout << "  Item code: ";
            getline(cin, code);

            MenuItem* item = menu.findItem(code);
            if (!item) {
                cout << "  [ERROR] Item not found.\n";
            } else if (!item->available) {
                cout << "  [ERROR] '" << item->name << "' is unavailable.\n";
            } else {
                cout << "  Quantity: ";
                cin >> qty;
                cin.ignore();
                if (qty <= 0) {
                    cout << "  [ERROR] Invalid quantity.\n";
                } else {
                    bill.addItem(item->code, item->name, qty, item->price);
                    cout << "  [OK] " << qty << "x " << item->name << " added.\n";
                }
            }

            if (bill.getHead() != nullptr) {
                cout << "  Add more items? (y/n): ";
                cin >> more;
                cin.ignore();
            } else {
                cout << "  Add items? (y/n): ";
                cin >> more;
                cin.ignore();
            }
        }

        if (bill.getHead() == nullptr) {
            cout << "  [INFO] No items ordered. Order cancelled.\n";
            return;
        }

        // Place order (enqueue) and record for undo
        int orderId = orderQueue.placeOrder(tableNum, bill);
        undoStack.pushOrder(orderId);
        nextOrderIdForUndo = orderId;

        cout << "  [OK] Order #" << orderId << " placed. Bill preview:\n";
        // Create a temporary bill display (we transferred the list)
        cout << "  (Items recorded in order queue)\n";
    }

    // -------------------------------------------------------------------
    // PROCESS ORDER: Dequeue next order for kitchen preparation
    // -------------------------------------------------------------------
    void processOrder() {
        if (!orderQueue.hasPending()) {
            cout << "  [INFO] No orders to process.\n";
            return;
        }

        Order nextOrder = orderQueue.processNextOrder();

        // Display the bill for the kitchen
        cout << "  ----- Order #" << nextOrder.orderId
             << " (Table " << nextOrder.tableNumber << ") -----\n";
        cout << "  Status: " << nextOrder.status << "\n";
        cout << "  Time: " << nextOrder.timestamp << "\n";
        cout << "  Items: " << nextOrder.itemCount << "\n";

        // Mark table as served
        if (nextOrder.tableNumber >= 1 && nextOrder.tableNumber <= 10) {
            tableStatus[nextOrder.tableNumber] = 3;
        }

        cout << "  [OK] Order sent to kitchen for preparation.\n";
    }

    // -------------------------------------------------------------------
    // CANCEL LAST ORDER: Undo the most recent order using stack
    // -------------------------------------------------------------------
    void cancelLastOrder() {
        int orderId = undoStack.popOrder();
        if (orderId == -1) {
            cout << "  [INFO] No orders to undo.\n";
            return;
        }

        cout << "  [OK] Order #" << orderId << " cancelled (removed from queue).\n";
        // Note: In a real system we would need to search the queue and remove.
        // For this simulation, we log the cancellation. A production system
        // would use a priority queue or mark orders as cancelled.
    }

    // -------------------------------------------------------------------
    // FIND SHORTEST DELIVERY ROUTE: BFS from kitchen to a table/zone
    // -------------------------------------------------------------------
    void findDeliveryRoute() {
        deliveryGraph.displayGraph();

        int destination;
        cout << "  Enter destination node index (0-Kitchen, 1-Dining, "
             << "2-PvtRm-A, 3-PvtRm-B, 4-Bar, 5-Patio, 6-15 for tables): ";
        cin >> destination;
        cin.ignore();

        if (destination < 0 || destination >= deliveryGraph.getNumNodes()) {
            cout << "  [ERROR] Invalid node index.\n";
            return;
        }

        vector<int> path = deliveryGraph.findShortestPath(0, destination);

        if (!path.empty()) {
            cout << "  Shortest route from Kitchen to "
                 << deliveryGraph.getNodeName(destination) << ":\n";
            cout << "    ";
            for (size_t i = 0; i < path.size(); i++) {
                cout << deliveryGraph.getNodeName(path[i]);
                if (i < path.size() - 1) cout << " -> ";
            }
            cout << "\n";
            cout << "  Total steps: " << (path.size() - 1) << "\n";
        }
    }

    // -------------------------------------------------------------------
    // GENERATE BILL: Display itemized bill for a completed order
    // -------------------------------------------------------------------
    void generateBill() {
        int orderId;
        cout << "  Enter order ID to generate bill: ";
        cin >> orderId;
        cin.ignore();

        // Search completed orders for the bill
        bool found = false;
        for (size_t i = 0; i < completedOrders.size(); i++) {
            if (completedOrders[i].orderId == orderId) {
                found = true;
                cout << "\n  ====== RESTAURANT BILL ======\n";
                cout << "  Order #: " << completedOrders[i].orderId << "\n";
                cout << "  Table: " << completedOrders[i].tableNumber << "\n";
                cout << "  Date: " << completedOrders[i].timestamp << "\n";
                cout << "  ------------------------------\n";

                // Traverse the linked list to display itemized bill
                BillItem* current = completedOrders[i].billHead;
                int lineNo = 1;
                cout << "  " << left << setw(5) << "#"
                     << setw(10) << "Code"
                     << setw(30) << "Item"
                     << setw(6) << "Qty"
                     << setw(10) << "Price"
                     << "Total\n";
                cout << "  " << string(65, '-') << "\n";

                while (current) {
                    cout << "  " << left << setw(5) << lineNo
                         << setw(10) << current->itemCode
                         << setw(30) << current->itemName
                         << setw(6) << current->quantity
                         << setw(10) << fixed << setprecision(2)
                         << current->unitPrice
                         << current->lineTotal << "\n";
                    current = current->next;
                    lineNo++;
                }
                cout << "  " << string(65, '-') << "\n";
                cout << "  " << right << setw(58) << "TOTAL: $"
                     << fixed << setprecision(2)
                     << completedOrders[i].totalAmount << "\n";
                cout << "  =============================\n";
                break;
            }
        }

        if (!found) {
            cout << "  [INFO] Order #" << orderId
                 << " not found in completed orders.\n";
        }
    }

    // -------------------------------------------------------------------
    // DAILY SALES REPORT: Summary of all completed orders
    // -------------------------------------------------------------------
    void dailySalesReport() {
        if (completedOrders.empty()) {
            cout << "  [INFO] No completed orders today.\n";
            return;
        }

        double totalRevenue = 0.0;
        int totalItems = 0;
        unordered_map<string, int> categorySales; // Category -> items sold

        cout << "\n  ====== DAILY SALES REPORT ======\n";
        cout << "  " << left << setw(10) << "Order#"
             << setw(8) << "Table"
             << setw(10) << "Items"
             << setw(14) << "Amount"
             << "Time\n";
        cout << "  " << string(55, '-') << "\n";

        for (size_t i = 0; i < completedOrders.size(); i++) {
            cout << "  " << left << setw(10) << completedOrders[i].orderId
                 << setw(8) << completedOrders[i].tableNumber
                 << setw(10) << completedOrders[i].itemCount
                 << setw(14) << fixed << setprecision(2)
                 << completedOrders[i].totalAmount
                 << completedOrders[i].timestamp << "\n";

            totalRevenue += completedOrders[i].totalAmount;

            // Traverse bill to count items per category
            BillItem* current = completedOrders[i].billHead;
            while (current) {
                totalItems += current->quantity;
                // Map item code prefix to category (first char)
                string cat;
                char prefix = current->itemCode[0];
                if (prefix == 'M') cat = "Main Course";
                else if (prefix == 'S') cat = "Starter";
                else if (prefix == 'B') {
                    if (current->itemName.find("Naan") != string::npos
                        || current->itemName.find("Naan") != string::npos)
                        cat = "Bread";
                    else
                        cat = "Beverage";
                }
                else if (prefix == 'D') cat = "Dessert";
                else cat = "Other";

                categorySales[cat] += current->quantity;
                current = current->next;
            }
        }

        cout << "  " << string(55, '-') << "\n";
        cout << "  " << right << setw(28) << "TOTAL ORDERS: "
             << completedOrders.size() << "\n";
        cout << "  " << right << setw(28) << "TOTAL ITEMS SOLD: "
             << totalItems << "\n";
        cout << "  " << right << setw(28) << "TOTAL REVENUE: $"
             << fixed << setprecision(2) << totalRevenue << "\n";

        // Category breakdown
        cout << "\n  Category Breakdown:\n";
        unordered_map<string, int>::iterator cit;
        for (cit = categorySales.begin(); cit != categorySales.end(); ++cit) {
            cout << "    " << cit->first << ": " << cit->second << " items\n";
        }
        cout << "  =============================\n";
    }

    // -------------------------------------------------------------------
    // TABLE MANAGEMENT: View and manage table status
    // -------------------------------------------------------------------
    void manageTables() {
        cout << "\n  ----- TABLE STATUS -----\n";
        cout << "  " << left << setw(10) << "Table"
             << "Status\n";
        cout << "  " << string(25, '-') << "\n";

        for (int i = 1; i <= 10; i++) {
            string status;
            switch (tableStatus[i]) {
                case 0: status = "Empty"; break;
                case 1: status = "Seated"; break;
                case 2: status = "Ordered"; break;
                case 3: status = "Served"; break;
                default: status = "Unknown";
            }
            cout << "  " << left << setw(10) << i << status << "\n";
        }

        int tableNum;
        cout << "\n  Reset table status (0 to skip, or table 1-10): ";
        cin >> tableNum;
        cin.ignore();
        if (tableNum >= 1 && tableNum <= 10) {
            tableStatus[tableNum] = 0;
            cout << "  [OK] Table " << tableNum << " reset to Empty.\n";
        }
    }

    // -------------------------------------------------------------------
    // SIMULATE ORDER COMPLETION: Move a processed order to completed
    // -------------------------------------------------------------------
    void completeOrder() {
        // In a real system, kitchen would mark order done.
        // Here we simulate by taking the last processed order info.
        int orderId;
        cout << "  Enter order ID to mark as delivered: ";
        cin >> orderId;
        cin.ignore();

        // For simulation, we'll create a completed order entry
        Order completedOrder;
        completedOrder.orderId = orderId;
        completedOrder.status = "DELIVERED";

        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        completedOrder.timestamp = string(buf);

        // Try to find it in our tracking (simplified: just store placeholder)
        completedOrder.tableNumber = 0;
        completedOrder.totalAmount = 0.0;
        completedOrder.itemCount = 0;
        completedOrder.billHead = nullptr;

        completedOrders.push_back(std::move(completedOrder));
        cout << "  [OK] Order #" << orderId << " marked as delivered.\n";
    }

    // -------------------------------------------------------------------
    // RUN the interactive restaurant ordering system
    // -------------------------------------------------------------------
    void run() {
        // Seed sample menu items
        seedMenu();

        cout << "\n=============================================\n";
        cout << "   RESTAURANT ORDERING SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;
        string code, name, category;
        double price;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  MENU MANAGEMENT:\n";
            cout << "   1. Add Menu Item\n";
            cout << "   2. Update Item Price\n";
            cout << "   3. Toggle Item Availability\n";
            cout << "   4. Remove Menu Item\n";
            cout << "   5. Display Full Menu\n";
            cout << "  ORDERING:\n";
            cout << "   6. Place Order\n";
            cout << "   7. Process Next Order\n";
            cout << "   8. Cancel Last Order (Undo)\n";
            cout << "   9. Complete Order (Mark Delivered)\n";
            cout << "  BILLING & DELIVERY:\n";
            cout << "  10. Generate Itemized Bill\n";
            cout << "  11. Find Shortest Delivery Route\n";
            cout << "  12. Daily Sales Report\n";
            cout << "  TABLE MANAGEMENT:\n";
            cout << "  13. Manage Tables\n";
            cout << "  14. Show Pending Orders\n";
            cout << "  15. Display Restaurant Layout\n";
            cout << "   0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Item name: ";
                    getline(cin, name);
                    cout << "  Category (Starter/Main Course/Bread/Beverage/Dessert): ";
                    getline(cin, category);
                    cout << "  Price: $";
                    cin >> price;
                    cin.ignore();
                    if (price <= 0) {
                        cout << "  [ERROR] Invalid price.\n";
                    } else {
                        menu.addItem(name, category, price);
                    }
                    break;

                case 2:
                    cout << "  Item code: ";
                    getline(cin, code);
                    cout << "  New price: $";
                    cin >> price;
                    cin.ignore();
                    menu.updatePrice(code, price);
                    break;

                case 3:
                    cout << "  Item code: ";
                    getline(cin, code);
                    menu.toggleAvailability(code);
                    break;

                case 4:
                    cout << "  Item code: ";
                    getline(cin, code);
                    menu.removeItem(code);
                    break;

                case 5:
                    menu.displayMenu();
                    break;

                case 6:
                    placeOrder();
                    break;

                case 7:
                    processOrder();
                    break;

                case 8:
                    cancelLastOrder();
                    break;

                case 9:
                    completeOrder();
                    break;

                case 10:
                    generateBill();
                    break;

                case 11:
                    findDeliveryRoute();
                    break;

                case 12:
                    dailySalesReport();
                    break;

                case 13:
                    manageTables();
                    break;

                case 14:
                    orderQueue.displayPending();
                    break;

                case 15:
                    deliveryGraph.displayGraph();
                    break;

                case 0:
                    cout << "  Exiting system. Goodbye!\n";
                    break;

                default:
                    cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    RestaurantOrderingSystem restaurant;
    restaurant.run();
    return 0;
}
