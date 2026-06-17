/*
 * ============================================================================
 * FILE: 16_supermarket_billing.cpp
 * ============================================================================
 * SUPERMARKET BILLING SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Supermarket/retail billing is a classic
 * DSA practical exam project testing MULTIPLE data structures together.
 *
 * DATA STRUCTURES USED:
 *   1. Binary Search Tree  - Product catalog sorted by name (O(log n) search)
 *   2. Hash Table          - Barcode lookup (O(1) price/item retrieval)
 *   3. Queue               - Billing counter (FIFO customer processing)
 *   4. Singly Linked List  - Shopping cart (dynamic per-customer item list)
 *   5. Stack               - Bill history / purchase undo (LIFO)
 *
 * FUNCTIONALITIES:
 *   - Add products with name, price, barcode, stock quantity
 *   - Scan barcode to get product details instantly (hash table)
 *   - Browse products alphabetically (BST in-order traversal)
 *   - Add items to cart (linked list)
 *   - Process billing queue (FIFO - first customer in line gets billed)
 *   - Generate bill with itemized list and total
 *   - View purchase history (stack - LIFO)
 *   - Undo last purchase (stack pop restores stock)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>         // For billing counter queue (FIFO customer processing)
#include <stack>         // For bill history / undo (LIFO)
#include <vector>
#include <unordered_map> // For hash table (barcode -> product O(1) lookup)
#include <iomanip>       // For formatted bill output (setw, setprecision)
#include <ctime>         // For date/time stamps on bills
#include <sstream>       // For string stream operations
using namespace std;

// ============================================================================
// SECTION 1: CORE DATA STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// Product: Core entity. Each product has a unique barcode, name, price, stock.
// -------------------------------------------------------------------
struct Product {
    string barcode;
    string name;
    double price;
    int stock;

    Product(string b, string n, double p, int s)
        : barcode(b), name(n), price(p), stock(s) {}
};

// -------------------------------------------------------------------
// CartItem: A single line item in a shopping cart (product + quantity).
// Stores a pointer to the Product to avoid duplication.
// -------------------------------------------------------------------
struct CartItem {
    Product* product;    // Pointer to product in catalog (no copy)
    int quantity;        // Number of units

    CartItem(Product* p, int q) : product(p), quantity(q) {}
};

// -------------------------------------------------------------------
// Bill: Records a completed purchase. Stored in a stack for undo/review.
// -------------------------------------------------------------------
struct Bill {
    int billNumber;
    vector<CartItem> items;
    double total;
    string date;
    string customerName;

    Bill(int bn, vector<CartItem> it, double t, string cn)
        : billNumber(bn), items(it), total(t), customerName(cn) {
        time_t now = time(nullptr);
        date = ctime(&now);
        if (!date.empty() && date[date.length()-1] == '\n')
            date.pop_back();
    }
};

// ============================================================================
// SECTION 2: BST FOR PRODUCT CATALOG
//
// Why BST? Products must be displayed alphabetically and searched by name.
// BST stores items sorted by name automatically. In-order traversal gives
// alphabetical output without extra sorting. O(log n) search/insert/delete.
// ============================================================================

struct ProductNode {
    Product product;
    ProductNode* left;
    ProductNode* right;

    ProductNode(Product p) : product(p), left(nullptr), right(nullptr) {}
};

class ProductCatalog {
private:
    ProductNode* root;

    // Insert by name (alphabetical order)
    ProductNode* insert(ProductNode* node, Product p) {
        if (!node) return new ProductNode(p);
        if (p.name < node->product.name)
            node->left = insert(node->left, p);
        else if (p.name > node->product.name)
            node->right = insert(node->right, p);
        return node;
    }

    // Search by name (recursive BST search)
    ProductNode* search(ProductNode* node, string name) {
        if (!node || node->product.name == name) return node;
        if (name < node->product.name)
            return search(node->left, name);
        return search(node->right, name);
    }

    // Find minimum node (for deletion with two children)
    ProductNode* findMin(ProductNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    // Delete node from BST (three cases: leaf, one child, two children)
    ProductNode* deleteNode(ProductNode* node, string name) {
        if (!node) return nullptr;
        if (name < node->product.name)
            node->left = deleteNode(node->left, name);
        else if (name > node->product.name)
            node->right = deleteNode(node->right, name);
        else {
            if (!node->left) {
                ProductNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                ProductNode* temp = node->left;
                delete node;
                return temp;
            }
            ProductNode* successor = findMin(node->right);
            node->product = successor->product;
            node->right = deleteNode(node->right, successor->product.name);
        }
        return node;
    }

    // In-order traversal: displays products in alphabetical order
    void inorderDisplay(ProductNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "  " << left << setw(15) << node->product.barcode
             << setw(30) << node->product.name
             << right << setw(8) << fixed << setprecision(2) << node->product.price
             << "  Stock: " << node->product.stock << "\n";
        inorderDisplay(node->right);
    }

    void inorderCollect(ProductNode* node, vector<Product*>& v) {
        if (!node) return;
        inorderCollect(node->left, v);
        v.push_back(&node->product);
        inorderCollect(node->right, v);
    }

    void destroy(ProductNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    ProductCatalog() : root(nullptr) {}
    ~ProductCatalog() { destroy(root); }

    bool addProduct(string barcode, string name, double price, int stock) {
        if (search(root, name)) {
            cout << "  [ERROR] Product '" << name << "' already exists!\n";
            return false;
        }
        root = insert(root, Product(barcode, name, price, stock));
        return true;
    }

    Product* findByName(string name) {
        ProductNode* node = search(root, name);
        return node ? &(node->product) : nullptr;
    }

    Product* findByBarcode(string barcode) {
        vector<Product*> all = getAllProducts();
        for (Product* p : all) {
            if (p->barcode == barcode) return p;
        }
        return nullptr;
    }

    void updateStock(string name, int delta) {
        Product* p = findByName(name);
        if (p) { p->stock += delta; if (p->stock < 0) p->stock = 0; }
    }

    vector<Product*> getAllProducts() {
        vector<Product*> v;
        inorderCollect(root, v);
        return v;
    }

    void displayAllProducts() {
        if (!root) { cout << "  [INFO] No products in catalog.\n"; return; }
        cout << "  " << left << setw(15) << "Barcode"
             << setw(30) << "Product Name"
             << right << setw(8) << "Price" << "  Stock\n";
        cout << "  " << string(65, '-') << "\n";
        inorderDisplay(root);
    }

    bool removeProduct(string name) {
        if (!search(root, name)) {
            cout << "  [ERROR] Product '" << name << "' not found.\n";
            return false;
        }
        root = deleteNode(root, name);
        return true;
    }
};

// ============================================================================
// SECTION 3: HASH TABLE FOR BARCODE LOOKUP
//
// Why Hash Table? Barcode scanning must be INSTANT (O(1)). A hash table
// maps barcode strings directly to Product pointers, bypassing the BST's
// O(log n) name-based search. This mirrors real POS system architecture.
// ============================================================================

class BarcodeLookup {
private:
    unordered_map<string, Product*> barcodeMap;  // barcode -> Product*

public:
    void addMapping(string barcode, Product* product) {
        barcodeMap[barcode] = product;
    }

    void removeMapping(string barcode) {
        barcodeMap.erase(barcode);
    }

    Product* scanBarcode(string barcode) {
        if (barcodeMap.find(barcode) != barcodeMap.end())
            return barcodeMap[barcode];
        return nullptr;
    }

    bool hasBarcode(string barcode) {
        return barcodeMap.find(barcode) != barcodeMap.end();
    }

    int size() { return barcodeMap.size(); }
};

// ============================================================================
// SECTION 4: SINGLY LINKED LIST FOR SHOPPING CART
//
// Why Linked List? Each customer's cart is a dynamic collection. We don't
// know item count beforehand. Linked list allows O(1) head insertion and
// O(n) traversal (fine for typical < 50 items). Insertion at head is
// efficient; each new item added is linked at the front.
// ============================================================================

struct CartNode {
    CartItem item;
    CartNode* next;

    CartNode(CartItem i) : item(i), next(nullptr) {}
};

class ShoppingCart {
private:
    CartNode* head;      // Head of linked list (most recently added item)
    int itemCount;       // Distinct products in cart
    int totalUnits;      // Total quantity of all items

public:
    ShoppingCart() : head(nullptr), itemCount(0), totalUnits(0) {}
    ~ShoppingCart() { clear(); }

    void addItem(Product* product, int quantity) {
        if (quantity <= 0) { cout << "  [ERROR] Quantity must be positive.\n"; return; }
        if (product->stock < quantity) {
            cout << "  [ERROR] Insufficient stock! Only "
                 << product->stock << " available.\n"; return;
        }
        CartNode* existing = findProduct(product->name);
        if (existing) {
            existing->item.quantity += quantity;
            totalUnits += quantity;
            cout << "  [OK] Updated '" << product->name
                 << "' (total: " << existing->item.quantity << ").\n";
            return;
        }
        CartNode* newNode = new CartNode(CartItem(product, quantity));
        newNode->next = head;
        head = newNode;
        itemCount++;
        totalUnits += quantity;
        cout << "  [OK] Added " << quantity << " x '" << product->name << "' to cart.\n";
    }

    CartNode* findProduct(string name) {
        CartNode* temp = head;
        while (temp) {
            if (temp->item.product->name == name) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    bool removeItem(string productName) {
        CartNode* temp = head;
        CartNode* prev = nullptr;
        while (temp) {
            if (temp->item.product->name == productName) {
                if (prev) prev->next = temp->next;
                else head = temp->next;
                totalUnits -= temp->item.quantity;
                itemCount--;
                delete temp;
                return true;
            }
            prev = temp;
            temp = temp->next;
        }
        return false;
    }

    bool updateQuantity(string productName, int newQuantity) {
        CartNode* node = findProduct(productName);
        if (!node) return false;
        if (newQuantity <= 0) return removeItem(productName);
        int diff = newQuantity - node->item.quantity;
        if (node->item.product->stock < diff) {
            cout << "  [ERROR] Insufficient stock!\n"; return false;
        }
        totalUnits += diff;
        node->item.quantity = newQuantity;
        return true;
    }

    vector<CartItem> getItems() {
        vector<CartItem> items;
        CartNode* temp = head;
        while (temp) { items.push_back(temp->item); temp = temp->next; }
        return items;
    }

    double calculateTotal() {
        double total = 0.0;
        CartNode* temp = head;
        while (temp) {
            total += temp->item.product->price * temp->item.quantity;
            temp = temp->next;
        }
        return total;
    }

    void displayCart() {
        if (!head) { cout << "  [INFO] Cart is empty.\n"; return; }
        cout << "  " << left << setw(30) << "Item"
             << right << setw(8) << "Qty"
             << setw(10) << "Price" << setw(12) << "Total\n";
        cout << "  " << string(60, '-') << "\n";
        CartNode* temp = head;
        double grandTotal = 0.0;
        while (temp) {
            double lineTotal = temp->item.product->price * temp->item.quantity;
            grandTotal += lineTotal;
            cout << "  " << left << setw(30) << temp->item.product->name
                 << right << setw(8) << temp->item.quantity
                 << setw(10) << fixed << setprecision(2) << temp->item.product->price
                 << setw(12) << fixed << setprecision(2) << lineTotal << "\n";
            temp = temp->next;
        }
        cout << "  " << string(60, '-') << "\n";
        cout << "  " << right << setw(50) << "GRAND TOTAL: Rs. "
             << fixed << setprecision(2) << grandTotal << "\n";
    }

    void clear() {
        CartNode* temp = head;
        while (temp) { CartNode* toDelete = temp; temp = temp->next; delete toDelete; }
        head = nullptr; itemCount = 0; totalUnits = 0;
    }

    bool isEmpty() { return head == nullptr; }
    int getTotalUnits() { return totalUnits; }
};

// ============================================================================
// SECTION 5: QUEUE FOR BILLING COUNTER
//
// Why Queue? Customers at checkout must be served FIFO (First-In-First-Out).
// The first person in line gets billed first. This exactly matches the
// queue data structure. Each customer has a name and a cart pointer.
// ============================================================================

struct Customer {
    string name;
    ShoppingCart* cart;
    int queueNumber;

    Customer(string n, ShoppingCart* c, int q)
        : name(n), cart(c), queueNumber(q) {}
};

class BillingQueue {
private:
    queue<Customer> customers;  // STL queue wraps FIFO logic
    int nextQueueNumber;        // Sequential token numbers

public:
    BillingQueue() : nextQueueNumber(1) {}

    void addCustomer(string name, ShoppingCart* cart) {
        customers.push(Customer(name, cart, nextQueueNumber++));
        cout << "  [OK] " << name << " queued. Token #" << (nextQueueNumber - 1) << "\n";
    }

    Customer processNext() {
        if (customers.empty()) return Customer("NONE", nullptr, -1);
        Customer c = customers.front();
        customers.pop();
        return c;
    }

    bool hasCustomers() { return !customers.empty(); }
    int waitingCount() { return customers.size(); }

    void displayQueue() {
        if (customers.empty()) { cout << "  [INFO] No customers waiting.\n"; return; }
        queue<Customer> temp = customers;
        cout << "  Waiting (" << customers.size() << "):\n";
        while (!temp.empty()) {
            Customer& c = temp.front();
            int items = c.cart ? c.cart->getTotalUnits() : 0;
            cout << "  Token #" << c.queueNumber << " - " << c.name
                 << " (" << items << " items)\n";
            temp.pop();
        }
    }
};

// ============================================================================
// SECTION 6: STACK FOR BILL HISTORY / PURCHASE UNDO
//
// Why Stack? Viewing recent purchases or undoing the last one requires
// LIFO (Last-In-First-Out) order - the most recent purchase is on top.
// Popping from the stack reverses the transaction and restores stock.
// ============================================================================

class BillHistory {
private:
    stack<Bill> history;     // STL stack (LIFO)
    int nextBillNumber;

public:
    BillHistory() : nextBillNumber(1001) {}

    void addBill(vector<CartItem> items, double total, string customerName) {
        int billNo = nextBillNumber++;
        history.push(Bill(billNo, items, total, customerName));
        for (CartItem& ci : items) {
            ci.product->stock -= ci.quantity;
            if (ci.product->stock < 0) ci.product->stock = 0;
        }
        cout << "  [OK] Bill #" << billNo << " for " << customerName
             << ". Total: Rs. " << fixed << setprecision(2) << total << "\n";
    }

    bool undoLastPurchase() {
        if (history.empty()) {
            cout << "  [ERROR] No purchase history to undo.\n";
            return false;
        }
        Bill last = history.top();
        history.pop();
        for (CartItem& ci : last.items) {
            ci.product->stock += ci.quantity;
        }
        cout << "  [OK] Purchase #" << last.billNumber
             << " (Rs. " << fixed << setprecision(2) << last.total
             << ") undone. Stock restored.\n";
        return true;
    }

    void viewRecent(int count = 5) {
        if (history.empty()) { cout << "  [INFO] No bills in history.\n"; return; }
        stack<Bill> temp = history;
        int shown = 0;
        cout << "\n  " << left << setw(12) << "Bill #"
             << setw(25) << "Customer"
             << setw(15) << "Total" << "Date\n";
        cout << "  " << string(85, '-') << "\n";
        while (!temp.empty() && shown < count) {
            Bill& b = temp.top();
            cout << "  " << left << setw(12) << b.billNumber
                 << setw(25) << b.customerName
                 << setw(15) << fixed << setprecision(2) << b.total
                 << b.date << "\n";
            temp.pop();
            shown++;
        }
    }

    bool viewBillDetails(int billNumber) {
        stack<Bill> temp = history;
        while (!temp.empty()) {
            Bill& b = temp.top();
            if (b.billNumber == billNumber) {
                cout << "\n  ========== BILL #" << b.billNumber << " ==========\n";
                cout << "  Customer: " << b.customerName << "\n";
                cout << "  Date: " << b.date << "\n  " << string(45, '-') << "\n";
                cout << "  " << left << setw(25) << "Item"
                     << right << setw(6) << "Qty"
                     << setw(10) << "Price" << setw(10) << "Total\n";
                cout << "  " << string(45, '-') << "\n";
                for (CartItem& ci : b.items) {
                    double lt = ci.product->price * ci.quantity;
                    cout << "  " << left << setw(25) << ci.product->name
                         << right << setw(6) << ci.quantity
                         << setw(10) << fixed << setprecision(2) << ci.product->price
                         << setw(10) << fixed << setprecision(2) << lt << "\n";
                }
                cout << "  " << string(45, '-') << "\n";
                cout << "  " << right << setw(41) << "GRAND TOTAL: Rs. "
                     << fixed << setprecision(2) << b.total << "\n\n";
                return true;
            }
            temp.pop();
        }
        cout << "  [ERROR] Bill #" << billNumber << " not found.\n";
        return false;
    }

    int getBillCount() { return history.size(); }
};

// ============================================================================
// SECTION 7: MAIN SUPERMARKET SYSTEM - Integrates all data structures
// ============================================================================

class SupermarketBillingSystem {
private:
    ProductCatalog catalog;       // BST: products sorted by name
    BarcodeLookup barcodeLookup; // Hash table: O(1) barcode lookup
    ShoppingCart currentCart;     // Linked list: current customer's cart
    BillingQueue billingQueue;   // Queue: FIFO customer processing
    BillHistory billHistory;     // Stack: purchase history / undo

    // Add product to both BST and hash table (cross-structure consistency)
    void addProduct(string barcode, string name, double price, int stock) {
        if (barcodeLookup.hasBarcode(barcode)) {
            cout << "  [ERROR] Barcode " << barcode << " already exists!\n";
            return;
        }
        if (catalog.addProduct(barcode, name, price, stock)) {
            Product* p = catalog.findByName(name);
            if (p) barcodeLookup.addMapping(barcode, p);
        }
    }

    // Scan barcode -> O(1) hash table lookup -> show + optionally add to cart
    void scanBarcode(string barcode) {
        Product* p = barcodeLookup.scanBarcode(barcode);
        if (!p) {
            cout << "  [ERROR] No product found with barcode " << barcode << ".\n";
            return;
        }
        cout << "  [OK] " << p->name << " | Rs. "
             << fixed << setprecision(2) << p->price
             << " | Stock: " << p->stock << "\n";
        char addNow;
        cout << "  Add to cart? (y/n): ";
        cin >> addNow;
        if (addNow == 'y' || addNow == 'Y') {
            int qty;
            cout << "  Quantity: "; cin >> qty;
            currentCart.addItem(p, qty);
        }
    }

    // Checkout: generate bill (adds to stack + deducts stock) then clear cart
    void checkout(string customerName) {
        if (currentCart.isEmpty()) {
            cout << "  [ERROR] Cart is empty! Nothing to checkout.\n";
            return;
        }
        double total = currentCart.calculateTotal();
        vector<CartItem> items = currentCart.getItems();
        billHistory.addBill(items, total, customerName);
        billHistory.viewBillDetails(1000 + billHistory.getBillCount());
        currentCart.clear();
    }

    // Dequeue next customer from queue and process their checkout
    void processNextCustomer() {
        Customer next = billingQueue.processNext();
        if (next.name == "NONE") {
            cout << "  [INFO] No customers in queue.\n";
            return;
        }
        cout << "  [INFO] Now serving: " << next.name
             << " (Token #" << next.queueNumber << ")\n";
        if (!currentCart.isEmpty()) {
            checkout(next.name);
        } else {
            cout << "  [INFO] " << next.name << " has no items.\n";
        }
    }

public:
    SupermarketBillingSystem() {}

    // -------------------------------------------------------------------
    // RUN the interactive supermarket billing system
    // -------------------------------------------------------------------
    void run() {
        cout << "\n=============================================\n";
        cout << "   SUPERMARKET BILLING SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;
        string name, barcode, customerName;
        double price;
        int stock, qty, billNum;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Add Product to Catalog\n";
            cout << "  2. Scan Barcode (Hash Table O(1))\n";
            cout << "  3. Browse Products (BST In-Order)\n";
            cout << "  4. Add Item to Cart (Linked List)\n";
            cout << "  5. Remove Item from Cart\n";
            cout << "  6. View Cart\n";
            cout << "  7. Add Customer to Billing Queue\n";
            cout << "  8. Process Next Customer (FIFO)\n";
            cout << "  9. Checkout (Generate Bill)\n";
            cout << " 10. View Purchase History (Stack LIFO)\n";
            cout << " 11. View Bill Details\n";
            cout << " 12. Undo Last Purchase (Stack Pop)\n";
            cout << " 13. Remove Product from Catalog\n";
            cout << " 14. Show Waiting Customers\n";
            cout << " 15. Clear Cart\n";
            cout << "  0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Product Name: "; getline(cin, name);
                    cout << "  Barcode: "; getline(cin, barcode);
                    cout << "  Price: Rs. "; cin >> price;
                    cout << "  Stock: "; cin >> stock;
                    cin.ignore();
                    addProduct(barcode, name, price, stock);
                    break;

                case 2:
                    cout << "  Scan Barcode: "; getline(cin, barcode);
                    scanBarcode(barcode);
                    break;

                case 3:
                    catalog.displayAllProducts();
                    break;

                case 4: {
                    cout << "  Product Name: "; getline(cin, name);
                    Product* p = catalog.findByName(name);
                    if (!p) { cout << "  [ERROR] Product not found.\n"; break; }
                    cout << "  Quantity: "; cin >> qty;
                    cin.ignore();
                    currentCart.addItem(p, qty);
                    break;
                }

                case 5:
                    cout << "  Product Name: "; getline(cin, name);
                    if (currentCart.removeItem(name))
                        cout << "  [OK] Removed from cart.\n";
                    else
                        cout << "  [ERROR] Not in cart.\n";
                    break;

                case 6:
                    currentCart.displayCart();
                    break;

                case 7:
                    cout << "  Customer Name: "; getline(cin, customerName);
                    if (customerName.empty()) customerName = "Guest";
                    billingQueue.addCustomer(customerName, &currentCart);
                    break;

                case 8:
                    processNextCustomer();
                    break;

                case 9:
                    cout << "  Customer Name: "; getline(cin, customerName);
                    if (customerName.empty()) customerName = "Guest";
                    checkout(customerName);
                    break;

                case 10:
                    billHistory.viewRecent();
                    break;

                case 11:
                    cout << "  Bill Number: "; cin >> billNum;
                    cin.ignore();
                    billHistory.viewBillDetails(billNum);
                    break;

                case 12:
                    billHistory.undoLastPurchase();
                    break;

                case 13: {
                    cout << "  Product Name: "; getline(cin, name);
                    Product* p = catalog.findByName(name);
                    if (p) barcodeLookup.removeMapping(p->barcode);
                    if (catalog.removeProduct(name))
                        cout << "  [OK] Product removed.\n";
                    break;
                }

                case 14:
                    billingQueue.displayQueue();
                    break;

                case 15:
                    currentCart.clear();
                    cout << "  [OK] Cart cleared.\n";
                    break;

                case 0:
                    cout << "  Exiting system. Thank you!\n";
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
    SupermarketBillingSystem supermarket;
    supermarket.run();
    return 0;
}
