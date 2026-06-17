/*
 * ============================================================================
 * FILE: 33_courier_tracking.cpp
 * ============================================================================
 * COURIER / PARCEL TRACKING SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Medium-High. Courier tracking is a practical system testing
 * hash tables (tracking number lookup), stacks (delivery history), queues
 * (processing pipeline), linked lists (parcel manifest), and BSTs
 * (delivery zone sorting).
 *
 * DATA STRUCTURES USED:
 *   1. Hash Table (unordered_map) - Parcel Lookup by Tracking #
 *      - O(1) parcel retrieval from tracking number
 *      - Primary lookup mechanism in courier systems
 *   2. Stack - Parcel Delivery History / Status Timeline (LIFO)
 *      - Each status update pushed on top
 *      - Most recent status shown first
 *   3. Queue - Parcel Processing Pipeline (FIFO)
 *      - Intake -> Sorting -> Out for Delivery -> Delivered
 *      - Each stage processes parcels in arrival order
 *   4. Linked List - Daily Parcel Manifest
 *      - All parcels scheduled for delivery on a given day
 *      - Dynamic addition as new parcels arrive
 *   5. Binary Search Tree - Delivery Zone Organization
 *      - Parcels sorted by delivery pincode/zone
 *      - Enables zone-wise sorting and delivery route planning
 *
 * FUNCTIONALITIES:
 *   - Register parcel (hash table + linked list)
 *   - Track parcel by ID (hash table O(1))
 *   - Update status (push to stack history)
 *   - View status timeline (stack: most recent first)
 *   - Process pipeline stage (queue: FIFO processing)
 *   - Daily manifest (linked list)
 *   - Sort by delivery zone (BST in-order)
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
#include <vector>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct Parcel {
    string trackingId;
    string senderName;
    string receiverName;
    string receiverAddress;
    string pincode;
    string weight;        // "0.5kg", "2kg"
    string currentStatus;

    Parcel() {}
    Parcel(string tid, string sn, string rn, string ra,
           string pin, string w, string st)
        : trackingId(tid), senderName(sn), receiverName(rn),
          receiverAddress(ra), pincode(pin), weight(w), currentStatus(st) {}
};

struct StatusUpdate {
    string trackingId;
    string status;
    string location;
    string timestamp;

    StatusUpdate() {}
    StatusUpdate(string tid, string st, string loc, string ts)
        : trackingId(tid), status(st), location(loc), timestamp(ts) {}
};

// Processing queue entry
struct ProcessEntry {
    string trackingId;
    string stage;          // "INTAKE", "SORTING", "OUT_DELIVERY"
    string receiverName;
    string pincode;

    ProcessEntry() {}
    ProcessEntry(string tid, string stg, string rn, string pin)
        : trackingId(tid), stage(stg), receiverName(rn), pincode(pin) {}
};

// BST node: parcel sorted by pincode
struct PincodeNode {
    string trackingId;
    string pincode;
    string receiverName;
    PincodeNode* left;
    PincodeNode* right;

    PincodeNode(string tid, string pin, string rn)
        : trackingId(tid), pincode(pin), receiverName(rn),
          left(nullptr), right(nullptr) {}
};

// Linked list node: daily manifest
struct ManifestNode {
    Parcel parcel;
    ManifestNode* next;

    ManifestNode(Parcel p) : parcel(p), next(nullptr) {}
};

// ============================================================================
// SECTION 2: BST - DELIVERY ZONE SORTING
// ============================================================================
class PincodeBST {
private:
    PincodeNode* root;

    PincodeNode* insert(PincodeNode* node, string tid, string pin, string rn) {
        if (!node) return new PincodeNode(tid, pin, rn);
        if (pin < node->pincode)
            node->left = insert(node->left, tid, pin, rn);
        else
            node->right = insert(node->right, tid, pin, rn);
        return node;
    }

    void inorder(PincodeNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << "    " << left << setw(14) << node->trackingId
             << setw(10) << node->pincode
             << node->receiverName << "\n";
        inorder(node->right);
    }

    void collect(PincodeNode* node, vector<PincodeNode*>& vec) {
        if (!node) return;
        collect(node->left, vec);
        vec.push_back(node);
        collect(node->right, vec);
    }

    void destroy(PincodeNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    PincodeBST() : root(nullptr) {}
    ~PincodeBST() { destroy(root); }

    void insert(string tid, string pin, string rn) {
        root = insert(root, tid, pin, rn);
    }

    void display() {
        if (!root) {
            cout << "    [No parcels in zone sort]\n";
            return;
        }
        cout << "    Parcels by Delivery Zone (BST - Sorted by Pincode):\n";
        cout << "    " << left << setw(14) << "Tracking ID"
             << setw(10) << "Pincode"
             << "Receiver\n";
        cout << "    " << string(35, '-') << "\n";
        inorder(root);
    }
};

// ============================================================================
// SECTION 3: LINKED LIST - DAILY MANIFEST
// ============================================================================
class Manifest {
private:
    ManifestNode* head;
    int count;

public:
    Manifest() : head(nullptr), count(0) {}
    ~Manifest() {
        ManifestNode* curr = head;
        while (curr) {
            ManifestNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void addParcel(Parcel p) {
        ManifestNode* node = new ManifestNode(p);
        node->next = head;
        head = node;
        count++;
    }

    void display() {
        if (!head) {
            cout << "    [No parcels in manifest]\n";
            return;
        }
        ManifestNode* curr = head;
        cout << "    Daily Manifest (Linked List):\n";
        cout << "    " << left << setw(14) << "Tracking"
             << setw(20) << "Sender"
             << setw(20) << "Receiver"
             << setw(8) << "Pincode"
             << "Status\n";
        cout << "    " << string(70, '-') << "\n";
        while (curr) {
            cout << "    " << left << setw(14) << curr->parcel.trackingId
                 << setw(20) << curr->parcel.senderName
                 << setw(20) << curr->parcel.receiverName
                 << setw(8) << curr->parcel.pincode
                 << curr->parcel.currentStatus << "\n";
            curr = curr->next;
        }
    }

    bool isEmpty() { return head == nullptr; }
    int getCount() { return count; }
};

// ============================================================================
// SECTION 4: MAIN COURIER SYSTEM
// ============================================================================
class CourierSystem {
private:
    unordered_map<string, Parcel> parcels;           // Hash: trackingId -> Parcel
    unordered_map<string, stack<StatusUpdate>> statusHistory; // Hash: id -> Status Stack
    queue<ProcessEntry> intakeQueue;                 // Queue: intake processing
    queue<ProcessEntry> sortingQueue;                // Queue: sorting stage
    queue<ProcessEntry> deliveryQueue;               // Queue: out for delivery
    Manifest manifest;                               // Linked list: daily manifest
    PincodeBST zoneSort;                             // BST: sort by pincode
    int nextTrackingNum;

    string getTimestamp() {
        time_t now = time(nullptr);
        string ts = ctime(&now);
        ts.erase(ts.length() - 1); // Remove newline
        return ts;
    }

public:
    CourierSystem() : nextTrackingNum(10001) {
        seedData();
    }

    void seedData() {
        registerParcel("Rahul Gupta", "Amit Sharma", "42 MG Road, Mumbai", "400001", "1.5kg");
        registerParcel("Priya Patel", "Sneha Reddy", "15 Jubilee Hills, Hyderabad", "500033", "0.5kg");
        registerParcel("Vikram Singh", "Neha Verma", "8 Sector 14, Noida", "201301", "2.0kg");
        registerParcel("Ananya Das", "Ravi Kumar", "100 BTM Layout, Bangalore", "560068", "1.0kg");
        registerParcel("Deepak Joshi", "Meera Nair", "55 Park Street, Kolkata", "700016", "3.0kg");
    }

    // -------------------------------------------------------------------
    // REGISTER PARCEL: Hash table insertion + linked list manifest + zone BST
    // -------------------------------------------------------------------
    string registerParcel(string sender, string receiver,
                          string address, string pincode, string weight) {
        string tid = "COU" + to_string(nextTrackingNum++);
        Parcel p(tid, sender, receiver, address, pincode, weight, "REGISTERED");
        parcels[tid] = p;                     // Hash table insertion
        manifest.addParcel(p);                // Linked list manifest
        zoneSort.insert(tid, pincode, receiver); // BST by pincode

        // Initial status
        statusHistory[tid].push(StatusUpdate(tid, "REGISTERED", "Sorting Center", getTimestamp()));

        cout << "  [REGISTERED] Parcel " << tid << " from " << sender
             << " to " << receiver << " (" << weight << ")\n";

        // Add to intake queue
        intakeQueue.push(ProcessEntry(tid, "INTAKE", receiver, pincode));
        return tid;
    }

    // -------------------------------------------------------------------
    // TRACK PARCEL: Hash table O(1) + Stack status timeline
    // -------------------------------------------------------------------
    void trackParcel(string trackingId) {
        auto pit = parcels.find(trackingId);
        if (pit == parcels.end()) {
            cout << "  [ERROR] Parcel not found.\n";
            return;
        }

        Parcel& p = pit->second;
        cout << "  Parcel: " << p.trackingId << "\n";
        cout << "  Sender: " << p.senderName << "\n";
        cout << "  Receiver: " << p.receiverName << "\n";
        cout << "  Address: " << p.receiverAddress << "\n";
        cout << "  Pincode: " << p.pincode << "\n";
        cout << "  Weight: " << p.weight << "\n";
        cout << "  Current Status: " << p.currentStatus << "\n";

        cout << "  Status Timeline (Stack - Most Recent First):\n";
        auto sit = statusHistory.find(trackingId);
        if (sit != statusHistory.end() && !sit->second.empty()) {
            stack<StatusUpdate> temp = sit->second;
            while (!temp.empty()) {
                StatusUpdate su = temp.top(); temp.pop();
                cout << "    [" << su.timestamp << "] " << su.status
                     << " @ " << su.location << "\n";
            }
        }
    }

    // -------------------------------------------------------------------
    // UPDATE STATUS: Push to stack (LIFO - most recent first)
    // -------------------------------------------------------------------
    void updateStatus(string trackingId, string status, string location) {
        if (parcels.find(trackingId) == parcels.end()) {
            cout << "  [ERROR] Parcel not found.\n";
            return;
        }
        parcels[trackingId].currentStatus = status;
        statusHistory[trackingId].push(
            StatusUpdate(trackingId, status, location, getTimestamp()));
        cout << "  [UPDATED] " << trackingId << " -> " << status
             << " at " << location << "\n";
    }

    // -------------------------------------------------------------------
    // PROCESS PIPELINE: Queue stages (FIFO)
    //   Intake -> Sorting -> Out for Delivery -> Delivered
    // -------------------------------------------------------------------
    void processIntake() {
        if (intakeQueue.empty()) {
            cout << "  [INFO] No parcels in intake.\n";
            return;
        }
        ProcessEntry e = intakeQueue.front(); intakeQueue.pop();
        updateStatus(e.trackingId, "SORTING", "Sorting Center");
        sortingQueue.push(ProcessEntry(e.trackingId, "SORTING", e.receiverName, e.pincode));
        cout << "  [INTAKE -> SORTING] " << e.trackingId << "\n";
    }

    void processSorting() {
        if (sortingQueue.empty()) {
            cout << "  [INFO] No parcels in sorting.\n";
            return;
        }
        ProcessEntry e = sortingQueue.front(); sortingQueue.pop();
        updateStatus(e.trackingId, "OUT FOR DELIVERY", "Local Hub");
        deliveryQueue.push(ProcessEntry(e.trackingId, "OUT_DELIVERY", e.receiverName, e.pincode));
        cout << "  [SORTING -> DELIVERY] " << e.trackingId << "\n";
    }

    void processDelivery() {
        if (deliveryQueue.empty()) {
            cout << "  [INFO] No parcels out for delivery.\n";
            return;
        }
        ProcessEntry e = deliveryQueue.front(); deliveryQueue.pop();
        updateStatus(e.trackingId, "DELIVERED", e.receiverName + "'s Address");
        cout << "  [DELIVERED] " << e.trackingId << " to " << e.receiverName << "\n";
    }

    void showPipeline() {
        cout << "  Processing Pipeline:\n";
        cout << "    Intake Queue: " << intakeQueue.size() << " parcels\n";
        cout << "    Sorting Queue: " << sortingQueue.size() << " parcels\n";
        cout << "    Delivery Queue: " << deliveryQueue.size() << " parcels\n";
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   COURIER / PARCEL TRACKING SYSTEM\n";
        cout << "   Data Structures: Hash Table (Parcels),\n";
        cout << "   Stack (Status History), Queue (Pipeline),\n";
        cout << "   Linked List (Manifest), BST (Zone Sort)\n";
        cout << "=============================================\n\n";

        int choice;
        string tid, sender, receiver, address, pincode, weight, status, location;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [HASH TABLE - Parcels]\n";
            cout << "    1. Register New Parcel (Hash + LL + BST)\n";
            cout << "    2. Track Parcel by ID (Hash O(1))\n";
            cout << "    3. Update Status (Stack Push)\n";
            cout << "  [LINKED LIST - Manifest]\n";
            cout << "    4. Show Daily Manifest\n";
            cout << "  [BST - Zone Sort]\n";
            cout << "    5. Parcels by Delivery Zone (Sorted by Pincode)\n";
            cout << "  [QUEUE - Processing Pipeline]\n";
            cout << "    6. Show Pipeline Status\n";
            cout << "    7. Process Intake (Dequeue)\n";
            cout << "    8. Process Sorting (Dequeue)\n";
            cout << "    9. Process Delivery (Dequeue)\n";
            cout << "  [STACK - Status Timeline]\n";
            cout << "    (View via Track Parcel option 2)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Sender Name: "; getline(cin, sender);
                    cout << "  Receiver Name: "; getline(cin, receiver);
                    cout << "  Receiver Address: "; getline(cin, address);
                    cout << "  Pincode: "; getline(cin, pincode);
                    cout << "  Weight: "; getline(cin, weight);
                    registerParcel(sender, receiver, address, pincode, weight);
                    break;
                case 2:
                    cout << "  Tracking ID: "; getline(cin, tid);
                    trackParcel(tid);
                    break;
                case 3:
                    cout << "  Tracking ID: "; getline(cin, tid);
                    cout << "  New Status: "; getline(cin, status);
                    cout << "  Location: "; getline(cin, location);
                    updateStatus(tid, status, location);
                    break;
                case 4: manifest.display(); break;
                case 5: zoneSort.display(); break;
                case 6: showPipeline(); break;
                case 7: processIntake(); break;
                case 8: processSorting(); break;
                case 9: processDelivery(); break;
                case 0: cout << "  Parcels delivered! Goodbye.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    CourierSystem system;
    system.run();
    return 0;
}
