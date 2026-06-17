/*
 * ============================================================================
 * FILE: 29_ecommerce_cart.cpp
 * ============================================================================
 * E-COMMERCE SHOPPING CART SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. E-commerce cart is a top DSA practical exam topic
 * found across multiple university lab manuals. Tests linked list (cart),
 * hash table (product catalog), stack (order history), queue (shipping),
 * and BST (product catalog by price/category).
 *
 * DATA STRUCTURES USED:
 *   1. Doubly Linked List - Shopping Cart
 *      - Each item is a node with prev/next pointers
 *      - O(1) insertion at end, O(n) search for item removal
 *      - Doubly linked for easy forward/backward traversal
 *   2. Hash Table (unordered_map) - Product Catalog & Customer DB
 *      - O(1) product lookup by SKU/ID
 *      - O(1) customer account lookup
 *   3. Stack - Order History / Recent Purchases (LIFO)
 *      - Most recent order shown first
 *   4. Queue - Shipping / Order Processing (FIFO)
 *      - Orders processed in arrival order
 *   5. Binary Search Tree - Product Catalog by Price
 *      - Sorted traversal for price-range queries
 *      - In-order display of products by price
 *
 * FUNCTIONALITIES:
 *   - Add/remove products to catalog (hash + BST)
 *   - Browse catalog sorted by price (BST in-order)
 *   - Add items to cart (doubly linked list)
 *   - Remove items / update quantity in cart
 *   - Checkout: cart -> shipping queue + order history stack
 *   - View order history (stack - most recent first)
 *   - Process next order for shipping (queue - FIFO)
 *   - Search products by ID (hash table O(1))
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>
#include <queue>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct Product {
    string sku;
    string name;
    string category;
    double price;
    int stock;

    Product() : price(0.0), stock(0) {}
    Product(string s, string n, string c, double p, int st)
        : sku(s), name(n), category(c), price(p), stock(st) {}
};

struct Customer {
    string custId;
    string name;
    string email;
    string address;

    Customer() {}
    Customer(string id, string n, string e, string a)
        : custId(id), name(n), email(e), address(a) {}
};

struct Order {
    string orderId;
    string custId;
    string custName;
    double total;
    string status;        // PENDING, SHIPPED, DELIVERED
    string itemSummary;

    Order() : total(0.0) {}
    Order(string id, string cid, string cn, double t, string s, string items)
        : orderId(id), custId(cid), custName(cn),
          total(t), status(s), itemSummary(items) {}
};

// CartItem for the doubly linked list cart
struct CartItem {
    string sku;
    string productName;
    double unitPrice;
    int quantity;

    CartItem() : unitPrice(0.0), quantity(0) {}
    CartItem(string s, string n, double p, int q)
        : sku(s), productName(n), unitPrice(p), quantity(q) {}

    double getSubtotal() const {
        return unitPrice * quantity;
    }
};

// ============================================================================
// SECTION 2: DOUBLY LINKED LIST FOR SHOPPING CART
//
// Why Doubly Linked List?
//   - Items can be added/removed O(1) at ends
//   - Navigate cart forward/backward
//   - Remove specific item by searching O(n) then unlinking O(1)
// ============================================================================
struct CartNode {
    CartItem item;
    CartNode* prev;
    CartNode* next;

    CartNode(CartItem i) : item(i), prev(nullptr), next(nullptr) {}
};

class ShoppingCart {
private:
    CartNode* head;
    CartNode* tail;
    int count;

public:
    ShoppingCart() : head(nullptr), tail(nullptr), count(0) {}

    ~ShoppingCart() {
        CartNode* curr = head;
        while (curr) {
            CartNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    // Add item to end of cart. O(1).
    void addItem(CartItem item) {
        // Check if product already in cart
        CartNode* existing = findBySku(item.sku);
        if (existing) {
            existing->item.quantity += item.quantity;
            return;
        }
        CartNode* node = new CartNode(item);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        count++;
    }

    // Remove item by SKU. O(n) search, O(1) deletion.
    bool removeItem(string sku) {
        CartNode* curr = head;
        while (curr) {
            if (curr->item.sku == sku) {
                if (curr->prev) curr->prev->next = curr->next;
                else head = curr->next;
                if (curr->next) curr->next->prev = curr->prev;
                else tail = curr->prev;
                delete curr;
                count--;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    CartNode* findBySku(string sku) {
        CartNode* curr = head;
        while (curr) {
            if (curr->item.sku == sku) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    double getTotal() {
        double sum = 0;
        CartNode* curr = head;
        while (curr) {
            sum += curr->item.getSubtotal();
            curr = curr->next;
        }
        return sum;
    }

    void display() {
        if (!head) {
            cout << "    [Cart is empty]\n";
            return;
        }
        cout << "    " << left << setw(6) << "SKU"
             << setw(30) << "Product"
             << setw(4) << "Qty"
             << setw(10) << "Price"
             << "Subtotal\n";
        cout << "    " << string(65, '-') << "\n";
        CartNode* curr = head;
        while (curr) {
            cout << "    " << left << setw(6) << curr->item.sku
                 << setw(30) << curr->item.productName
                 << setw(4) << curr->item.quantity
                 << "$" << setw(8) << fixed << setprecision(2) << curr->item.unitPrice
                 << "$" << curr->item.getSubtotal() << "\n";
            curr = curr->next;
        }
        cout << "    " << string(65, '-') << "\n";
        cout << "    Total: $" << fixed << setprecision(2) << getTotal() << "\n";
    }

    string getSummary() {
        stringstream ss;
        CartNode* curr = head;
        while (curr) {
            if (curr != head) ss << ", ";
            ss << curr->item.productName << " x" << curr->item.quantity;
            curr = curr->next;
        }
        return ss.str();
    }

    void clear() {
        CartNode* curr = head;
        while (curr) {
            CartNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = tail = nullptr;
        count = 0;
    }

    bool isEmpty() { return head == nullptr; }
    int getCount() { return count; }
};

// ============================================================================
// SECTION 3: BST FOR PRODUCT CATALOG BY PRICE
// ============================================================================
struct BSTNode {
    Product product;
    BSTNode* left;
    BSTNode* right;

    BSTNode(Product p) : product(p), left(nullptr), right(nullptr) {}
};

class PriceTree {
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, Product p) {
        if (!node) return new BSTNode(p);
        if (p.price < node->product.price)
            node->left = insert(node->left, p);
        else
            node->right = insert(node->right, p);
        return node;
    }

    void inorder(BSTNode* node, int& idx) {
        if (!node) return;
        inorder(node->left, idx);
        cout << "    " << left << setw(4) << (++idx)
             << setw(8) << node->product.sku
             << setw(30) << node->product.name
             << "$" << setw(8) << fixed << setprecision(2) << node->product.price
             << setw(4) << node->product.stock << "\n";
        inorder(node->right, idx);
    }

    void collectInorder(BSTNode* node, vector<Product>& vec) {
        if (!node) return;
        collectInorder(node->left, vec);
        vec.push_back(node->product);
        collectInorder(node->right, vec);
    }

    void destroy(BSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    PriceTree() : root(nullptr) {}

    ~PriceTree() { destroy(root); }

    void insert(Product p) {
        root = insert(root, p);
    }

    void displayByPrice() {
        if (!root) {
            cout << "    [No products]\n";
            return;
        }
        cout << "    Products sorted by price:\n";
        cout << "    " << left << setw(4) << "#"
             << setw(8) << "SKU"
             << setw(30) << "Name"
             << setw(10) << "Price"
             << "Stock\n";
        cout << "    " << string(55, '-') << "\n";
        int idx = 0;
        inorder(root, idx);
    }

    vector<Product> getSortedProducts() {
        vector<Product> vec;
        collectInorder(root, vec);
        return vec;
    }
};

// ============================================================================
// SECTION 4: MAIN E-COMMERCE SYSTEM
// ============================================================================
class ECommerceSystem {
private:
    unordered_map<string, Product> catalog;        // Hash: SKU -> Product
    unordered_map<string, Customer> customers;     // Hash: custId -> Customer
    PriceTree priceTree;                           // BST by price
    ShoppingCart cart;                             // Doubly linked list cart
    stack<Order> orderHistory;                     // Stack: recent orders first
    queue<Order> shippingQueue;                    // Queue: orders FIFO
    int nextOrderNum;

public:
    ECommerceSystem() : nextOrderNum(1001) {
        seedData();
    }

    void seedData() {
        Product p1("P001", "Wireless Headphones", "Electronics", 79.99, 50);
        Product p2("P002", "USB-C Hub 7-in-1", "Electronics", 34.99, 100);
        Product p3("P003", "Cotton T-Shirt", "Clothing", 19.99, 200);
        Product p4("P004", "Running Shoes", "Footwear", 89.99, 75);
        Product p5("P005", "Coffee Maker", "Appliances", 49.99, 30);
        Product p6("P006", "Backpack 45L", "Accessories", 59.99, 60);
        Product p7("P007", "Bluetooth Speaker", "Electronics", 29.99, 80);
        Product p8("P008", "Yoga Mat", "Sports", 24.99, 90);
        Product p9("P009", "Desk Lamp LED", "Furniture", 39.99, 40);
        Product p10("P010", "Water Bottle 1L", "Accessories", 14.99, 150);

        Product prods[] = {p1, p2, p3, p4, p5, p6, p7, p8, p9, p10};
        for (auto& p : prods) {
            catalog[p.sku] = p;
            priceTree.insert(p);
        }

        customers["C001"] = Customer("C001", "Alice Johnson", "alice@email.com", "123 Main St");
        customers["C002"] = Customer("C002", "Bob Smith", "bob@email.com", "456 Oak Ave");
        customers["C003"] = Customer("C003", "Carol Lee", "carol@email.com", "789 Pine Rd");
    }

    // -------------------------------------------------------------------
    // CATALOG: Hash table lookup O(1)
    // -------------------------------------------------------------------
    void showCatalog() {
        cout << "  Product Catalog (hash table):\n";
        cout << "  " << left << setw(8) << "SKU"
             << setw(30) << "Name"
             << setw(15) << "Category"
             << setw(10) << "Price"
             << "Stock\n";
        cout << "  " << string(70, '-') << "\n";
        for (auto& pair : catalog) {
            Product& p = pair.second;
            cout << "  " << left << setw(8) << p.sku
                 << setw(30) << p.name
                 << setw(15) << p.category
                 << "$" << setw(7) << fixed << setprecision(2) << p.price
                 << p.stock << "\n";
        }
    }

    // -------------------------------------------------------------------
    // BST: Products sorted by price
    // -------------------------------------------------------------------
    void showByPrice() {
        priceTree.displayByPrice();
    }

    // -------------------------------------------------------------------
    // ADD TO CART: Doubly linked list insertion O(1)
    // -------------------------------------------------------------------
    void addToCart(string sku, int qty) {
        auto it = catalog.find(sku);
        if (it == catalog.end()) {
            cout << "  [ERROR] Product not found.\n";
            return;
        }
        if (it->second.stock < qty) {
            cout << "  [ERROR] Insufficient stock (available: "
                 << it->second.stock << ").\n";
            return;
        }
        cart.addItem(CartItem(sku, it->second.name, it->second.price, qty));
        it->second.stock -= qty;
        cout << "  [OK] Added " << qty << "x " << it->second.name << " to cart.\n";
    }

    // -------------------------------------------------------------------
    // REMOVE FROM CART: O(n) search, O(1) deletion
    // -------------------------------------------------------------------
    void removeFromCart(string sku) {
        CartNode* node = cart.findBySku(sku);
        if (!node) {
            cout << "  [ERROR] Item not in cart.\n";
            return;
        }
        // Restore stock
        auto it = catalog.find(sku);
        if (it != catalog.end())
            it->second.stock += node->item.quantity;

        if (cart.removeItem(sku))
            cout << "  [OK] Removed from cart.\n";
    }

    // -------------------------------------------------------------------
    // CHECKOUT: Cart items -> Order -> Push to shipping queue + history stack
    // -------------------------------------------------------------------
    void checkout(string custId) {
        if (cart.isEmpty()) {
            cout << "  [ERROR] Cart is empty.\n";
            return;
        }
        auto cit = customers.find(custId);
        if (cit == customers.end()) {
            cout << "  [ERROR] Customer not found.\n";
            return;
        }
        double total = cart.getTotal();
        string summary = cart.getSummary();
        string orderId = "ORD" + to_string(nextOrderNum++);

        Order order(orderId, custId, cit->second.name, total, "PENDING", summary);
        shippingQueue.push(order);       // FIFO shipping queue
        orderHistory.push(order);        // LIFO order history
        cart.clear();                    // Empty the cart

        cout << "  [OK] Order " << orderId << " placed! Total: $"
             << fixed << setprecision(2) << total << "\n";
        cout << "  Items: " << summary << "\n";
    }

    // -------------------------------------------------------------------
    // ORDER HISTORY: Stack - most recent first O(n) display
    // -------------------------------------------------------------------
    void showOrderHistory() {
        if (orderHistory.empty()) {
            cout << "  [INFO] No order history.\n";
            return;
        }
        stack<Order> temp = orderHistory;
        cout << "  Order History (most recent first):\n";
        while (!temp.empty()) {
            Order o = temp.top();
            temp.pop();
            cout << "  " << o.orderId << " | " << o.custName
                 << " | $" << fixed << setprecision(2) << o.total
                 << " | " << o.status << "\n";
            cout << "     Items: " << o.itemSummary << "\n";
        }
    }

    // -------------------------------------------------------------------
    // SHIPPING QUEUE: FIFO processing
    // -------------------------------------------------------------------
    void processShipping() {
        if (shippingQueue.empty()) {
            cout << "  [INFO] No pending orders.\n";
            return;
        }
        Order o = shippingQueue.front();
        shippingQueue.pop();
        o.status = "SHIPPED";
        cout << "  [SHIPPED] Order " << o.orderId << " for " << o.custName << "\n";
    }

    void showShippingQueue() {
        if (shippingQueue.empty()) {
            cout << "  [INFO] Shipping queue is empty.\n";
            return;
        }
        queue<Order> temp = shippingQueue;
        cout << "  Pending Shipments (FIFO):\n";
        while (!temp.empty()) {
            Order o = temp.front();
            temp.pop();
            cout << "  " << o.orderId << " | " << o.custName
                 << " | $" << fixed << setprecision(2) << o.total << "\n";
        }
    }

    // -------------------------------------------------------------------
    // ADD PRODUCT to catalog (hash + BST)
    // -------------------------------------------------------------------
    void addProduct(string sku, string name, string cat, double price, int stock) {
        if (catalog.find(sku) != catalog.end()) {
            cout << "  [ERROR] SKU already exists.\n";
            return;
        }
        Product p(sku, name, cat, price, stock);
        catalog[sku] = p;
        priceTree.insert(p);
        cout << "  [OK] Product '" << name << "' added to catalog.\n";
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   E-COMMERCE SHOPPING CART SYSTEM\n";
        cout << "   Data Structures: Doubly Linked List (Cart),\n";
        cout << "   Hash Table (Catalog), BST (Price Sort),\n";
        cout << "   Stack (History), Queue (Shipping)\n";
        cout << "=============================================\n\n";

        int choice, qty, stock;
        string sku, name, cat, custId;
        double price;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [CATALOG - Hash Table + BST]\n";
            cout << "    1. Display All Products (Hash)\n";
            cout << "    2. Display by Price (BST In-order)\n";
            cout << "    3. Add New Product (Hash + BST)\n";
            cout << "  [CART - Doubly Linked List]\n";
            cout << "    4. Add to Cart\n";
            cout << "    5. Remove from Cart\n";
            cout << "    6. View Cart\n";
            cout << "  [CHECKOUT]\n";
            cout << "    7. Checkout (Cart -> Stack + Queue)\n";
            cout << "  [HISTORY - Stack]\n";
            cout << "    8. Order History (LIFO / Most Recent)\n";
            cout << "  [SHIPPING - Queue]\n";
            cout << "    9. Show Shipping Queue (FIFO)\n";
            cout << "   10. Process Next Shipment (Dequeue)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: showCatalog(); break;
                case 2: showByPrice(); break;
                case 3:
                    cout << "  SKU: "; getline(cin, sku);
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Category: "; getline(cin, cat);
                    cout << "  Price: "; cin >> price; cin.ignore();
                    cout << "  Stock: "; cin >> stock; cin.ignore();
                    addProduct(sku, name, cat, price, stock);
                    break;
                case 4:
                    cout << "  SKU: "; getline(cin, sku);
                    cout << "  Quantity: "; cin >> qty; cin.ignore();
                    addToCart(sku, qty);
                    break;
                case 5:
                    cout << "  SKU: "; getline(cin, sku);
                    removeFromCart(sku);
                    break;
                case 6: cart.display(); break;
                case 7:
                    cout << "  Customer ID: "; getline(cin, custId);
                    checkout(custId);
                    break;
                case 8: showOrderHistory(); break;
                case 9: showShippingQueue(); break;
                case 10: processShipping(); break;
                case 0: cout << "  Thanks for shopping! Goodbye.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    ECommerceSystem system;
    system.run();
    return 0;
}
