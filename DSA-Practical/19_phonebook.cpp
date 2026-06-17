/*
 * ============================================================================
 * FILE: 19_phonebook.cpp - PHONEBOOK MANAGEMENT SYSTEM (DSA Practical Exam)
 * ============================================================================
 *
 * DATA STRUCTURES USED:
 *   1. Trie (Prefix Tree)     - Name autocomplete: O(L) prefix search, 26 children
 *   2. Hash Table (unordered_map) - Phone lookup: O(1) average retrieval
 *   3. Singly Linked List     - Sorted contact storage: dynamic, traversable
 *   4. Stack (LIFO)           - Call history (most recent first) + Undo delete
 *
 * FEATURES: Add/Search/Update/Delete contacts, autocomplete by name prefix,
 * instant phone lookup, categorized display, call history, undo delete.
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <stack>         // For call history and undo delete (LIFO containers)
#include <vector>        // For returning multiple search results from Trie
#include <unordered_map> // For hash table (phone number -> contact lookup)
#include <iomanip>       // For setw(), left/right formatted tabular output
#include <ctime>         // For time() in call record timestamps
#include <algorithm>     // For sort() if needed (though we maintain sorted list)
#include <cctype>        // For tolower() in Trie key sanitization
#include <cstdlib>       // For rand(), srand() in call duration simulation
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// Contact - Core entity of the phonebook system
// Each contact stores name, phone number, email, and category.
// This structure is shared across ALL data structures:
//   - Trie indexes contacts by name (for prefix search)
//   - Hash table indexes contacts by phone (for O(1) lookup)
//   - Linked list stores contacts in sorted order (for display)
// -------------------------------------------------------------------
struct Contact {
    string name;            // Contact's full name (unique - acts as primary key)
    string phoneNumber;     // Phone number (unique - acts as secondary key in hash)
    string email;           // Email address
    string category;        // Category: "Family", "Friends", "Work", "General"

    // Default constructor (required for stack operations)
    Contact() : name(""), phoneNumber(""), email(""), category("General") {}

    // Parameterized constructor
    Contact(string n, string p, string e, string c)
        : name(n), phoneNumber(p), email(e), category(c) {}

    // Display contact in tabular format
    void display() const {
        cout << "  " << left << setw(20) << name
             << setw(15) << phoneNumber
             << setw(25) << email
             << setw(12) << category << "\n";
    }
};

// -------------------------------------------------------------------
// CallRecord - Logs every outgoing call made through the system
// Stored in a stack container so that the most recent call is always
// on top, giving us "most recent first" display for free (LIFO).
// -------------------------------------------------------------------
struct CallRecord {
    string contactName;     // Name of the person who was called
    string phoneNumber;     // Phone number that was dialed
    string duration;        // Simulated call duration (e.g., "5 min")
    string timestamp;       // Date and time when the call was made

    CallRecord(string n, string p)
        : contactName(n), phoneNumber(p) {
        // Generate current timestamp automatically
        time_t now = time(nullptr);
        timestamp = ctime(&now);
        if (!timestamp.empty() && timestamp.back() == '\n')
            timestamp.pop_back();
        // Simulate call duration between 1 and 30 minutes (for demo purposes)
        int mins = (rand() % 30) + 1;
        duration = to_string(mins) + " min";
    }

    // Display call record with index number for console listing
    void display(int index) const {
        cout << "  " << right << setw(3) << index << ". "
             << left << setw(18) << contactName
             << setw(14) << phoneNumber
             << setw(8) << duration
             << timestamp << "\n";
    }
};

// ============================================================================
// SECTION 2: TRIE (PREFIX TREE) FOR NAME AUTOCOMPLETE
//
// Why Trie? "Search as you type" needs prefix lookup. Hash tables cannot
// do prefix search; BST needs O(log n) per character. Trie: O(L) traverse
// prefix + O(M) collect matches. Each node: 26 children (a-z), endOfWord
// flag, and Contact* pointer (non-null only at terminals).
// ============================================================================

struct TrieNode {
    TrieNode* children[26];  // One pointer per lowercase letter (a=0, b=1, ..., z=25)
    bool endOfWord;          // true when this node completes a valid contact name
    Contact* contact;        // Points to the actual Contact in the linked list

    TrieNode() {
        for (int i = 0; i < 26; i++)
            children[i] = nullptr;  // No children initially
        endOfWord = false;
        contact = nullptr;
    }
};

class ContactTrie {
private:
    TrieNode* root;  // Root node represents the empty string

    // Sanitize: lowercase alphabetic only (fits 26-child structure)
    string sanitize(const string& name) const {
        string result;
        for (size_t i = 0; i < name.length(); i++) {
            char c = tolower(name[i]);
            if (c >= 'a' && c <= 'z') result += c;
        }
        return result;
    }

    // DFS: collect all Contact pointers from subtree (autocomplete core)
    void collectAll(TrieNode* node, vector<Contact*>& results) const {
        if (!node) return;
        if (node->endOfWord && node->contact)
            results.push_back(node->contact);
        for (int i = 0; i < 26; i++)
            if (node->children[i])
                collectAll(node->children[i], results);
    }

    // Recursive cleanup for destructor
    void destroy(TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < 26; i++)
            if (node->children[i])
                destroy(node->children[i]);
        delete node;
    }

public:
    ContactTrie() { root = new TrieNode(); }
    ~ContactTrie() { destroy(root); }

    // INSERT: Add name to Trie. O(L). Walk, create nodes as needed,
    // mark terminal with endOfWord and Contact pointer.
    void insert(const string& name, Contact* contactPtr) {
        string key = sanitize(name);
        TrieNode* current = root;
        for (size_t i = 0; i < key.length(); i++) {
            int idx = key[i] - 'a';
            if (!current->children[idx])
                current->children[idx] = new TrieNode();
            current = current->children[idx];
        }
        current->endOfWord = true;
        current->contact = contactPtr;
    }

    // SEARCH BY PREFIX: Traverse to prefix node, collect all subtree
    // contacts. O(L + M). This is the autocomplete engine.
    vector<Contact*> searchByPrefix(const string& prefix) const {
        vector<Contact*> results;
        string key = sanitize(prefix);
        TrieNode* current = root;
        for (size_t i = 0; i < key.length(); i++) {
            int idx = key[i] - 'a';
            if (!current->children[idx]) return results;
            current = current->children[idx];
        }
        collectAll(current, results);
        return results;
    }

    // REMOVE: Unmark endOfWord. Don't delete nodes (shared prefixes).
    // O(L).
    bool remove(const string& name) {
        string key = sanitize(name);
        TrieNode* current = root;
        for (size_t i = 0; i < key.length(); i++) {
            int idx = key[i] - 'a';
            if (!current->children[idx]) return false;
            current = current->children[idx];
        }
        if (current->endOfWord) {
            current->endOfWord = false;
            current->contact = nullptr;
            return true;
        }
        return false;
    }
};

// ============================================================================
// SECTION 3: SINGLY LINKED LIST FOR CONTACT STORAGE
//
// Why Linked List? Dynamic sizing (unknown contact count), sorted
// alphabetical insertion O(n), traversal O(n). Acceptable since n is
// small for a personal phonebook. Speed comes from Trie and hash table.
// ============================================================================

struct ContactNode {
    Contact data;        // The actual Contact object
    ContactNode* next;   // Pointer to the next node in the list

    ContactNode(Contact c) : data(c), next(nullptr) {}
};

class ContactList {
private:
    ContactNode* head;  // Pointer to the first node (nullptr if list is empty)
    int count;          // Number of contacts in the list

public:
    ContactList() : head(nullptr), count(0) {}

    ~ContactList() {
        ContactNode* current = head;
        while (current) {
            ContactNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // ADD CONTACT (Sorted): Insert in alphabetical order. O(n).
    void addSorted(Contact c) {
        ContactNode* newNode = new ContactNode(c);
        if (!head || c.name < head->data.name) {
            newNode->next = head;
            head = newNode;
        } else {
            ContactNode* current = head;
            while (current->next && current->next->data.name < c.name)
                current = current->next;
            newNode->next = current->next;
            current->next = newNode;
        }
        count++;
    }

    // REMOVE BY NAME: O(n). Returns copy of Contact for undo stack.
    Contact* removeByName(const string& name) {
        if (!head) return nullptr;
        ContactNode* toDelete = nullptr;
        if (head->data.name == name) {
            toDelete = head;
            head = head->next;
        } else {
            ContactNode* current = head;
            while (current->next && current->next->data.name != name)
                current = current->next;
            if (!current->next) return nullptr;
            toDelete = current->next;
            current->next = toDelete->next;
        }
        Contact* removedContact = new Contact(toDelete->data);
        delete toDelete;
        count--;
        return removedContact;
    }

    // FIND BY NAME: Linear search. O(n). Returns pointer or nullptr.
    Contact* findByName(const string& name) const {
        ContactNode* current = head;
        while (current) {
            if (current->data.name == name) return &(current->data);
            current = current->next;
        }
        return nullptr;
    }

    // DISPLAY ALL: Traverse sorted list. O(n).
    void displayAll() const {
        if (!head) { cout << "  [INFO] Phonebook is empty.\n"; return; }
        cout << "  " << left << setw(5) << "#"
             << setw(20) << "Name"
             << setw(15) << "Phone"
             << setw(25) << "Email"
             << setw(12) << "Category\n";
        cout << "  " << string(80, '-') << "\n";
        ContactNode* current = head;
        int idx = 1;
        while (current) {
            cout << "  " << left << setw(5) << idx;
            current->data.display();
            current = current->next;
            idx++;
        }
    }

    // DISPLAY BY CATEGORY: Filtered traversal. O(n).
    void displayByCategory(const string& category) const {
        ContactNode* current = head;
        bool found = false;
        int idx = 1;
        while (current) {
            if (current->data.category == category) {
                if (!found) {
                    cout << "  Contacts in category '" << category << "':\n";
                    cout << "  " << left << setw(5) << "#"
                         << setw(20) << "Name"
                         << setw(15) << "Phone"
                         << setw(25) << "Email\n";
                    cout << "  " << string(65, '-') << "\n";
                    found = true;
                }
                cout << "  " << left << setw(5) << idx
                     << setw(20) << current->data.name
                     << setw(15) << current->data.phoneNumber
                     << setw(25) << current->data.email << "\n";
                idx++;
            }
            current = current->next;
        }
        if (!found)
            cout << "  [INFO] No contacts in category '" << category << "'.\n";
    }

    int getCount() const { return count; }
    bool isEmpty() const { return head == nullptr; }
};

// ============================================================================
// SECTION 4: HASH TABLE (unordered_map) FOR PHONE NUMBER LOOKUP
//
// Why Hash Table? Phone numbers are unique identifiers. unordered_map
// provides O(1) average lookup for instant "who owns this number?" search.
// Used directly in PhoneBook class (no wrapper needed — it's already STL).
// Key: phone string, Value: Contact* pointer (into linked list).
// ============================================================================

// ============================================================================
// SECTION 5: STACK FOR CALL HISTORY AND UNDO DELETE
//
// Why Stack? Both features follow LIFO: most recent call first in history,
// most recently deleted contact restored first by undo. STL stack<>
// (wrapping deque) is used directly in the PhoneBook class.
// ============================================================================

// ============================================================================
// SECTION 6: PHONEBOOK SYSTEM - Integrates all data structures
// ============================================================================

class PhoneBook {
private:
    ContactTrie nameTrie;                    // Trie for prefix-based name autocomplete
    unordered_map<string, Contact*> phoneMap; // Hash table: phone# -> Contact* (O(1))
    ContactList contactList;                 // Singly linked list: sorted contact storage
    stack<CallRecord> callHistory;           // Stack: call history (most recent first)
    stack<Contact> undoStack;                // Stack: deleted contacts (for undo)

    // Insert into linked list, Trie, and hash table (shared helper for undo)
    void addContactToStructures(Contact c) {
        contactList.addSorted(c);
        Contact* contactPtr = contactList.findByName(c.name);
        if (contactPtr) {
            nameTrie.insert(c.name, contactPtr);
            phoneMap[c.phoneNumber] = contactPtr;
        }
    }

    bool isPhoneTaken(const string& phone) const {
        return phoneMap.find(phone) != phoneMap.end();
    }

public:
    PhoneBook() {
        // Seed the random number generator for simulated call durations
        srand(static_cast<unsigned>(time(nullptr)));
    }

    // -------------------------------------------------------------------
    // FEATURE 1: ADD CONTACT
    // Prompts for name, phone, email, and category.
    // Checks for duplicates before inserting into all data structures.
    // -------------------------------------------------------------------
    void addContact() {
        string name, phone, email, category;
        cout << "  Enter name: "; getline(cin, name);
        cout << "  Enter phone: "; getline(cin, phone);
        cout << "  Enter email: "; getline(cin, email);
        cout << "  Enter category (General/Family/Friends/Work): "; getline(cin, category);

        if (name.empty() || phone.empty()) {
            cout << "  [ERROR] Name and phone are required.\n";
            return;
        }

        // Duplicate checks: name (primary key) and phone (secondary key)
        if (contactList.findByName(name)) {
            cout << "  [ERROR] Contact '" << name << "' already exists.\n";
            return;
        }
        if (isPhoneTaken(phone)) {
            cout << "  [ERROR] Phone number '" << phone << "' already exists.\n";
            return;
        }

        if (category.empty()) category = "General";

        addContactToStructures(Contact(name, phone, email, category));
        cout << "  [OK] Contact '" << name << "' added successfully.\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 2: SEARCH BY NAME PREFIX (Trie Autocomplete)
    // As the user enters a name (or partial name), the Trie finds all
    // contacts whose names start with that prefix. This simulates the
    // "search as you type" feature found in modern phonebooks.
    // -------------------------------------------------------------------
    void searchByName() {
        string prefix;
        cout << "  Enter name (or prefix to autocomplete): "; getline(cin, prefix);
        if (prefix.empty()) {
            cout << "  [INFO] No prefix entered.\n";
            return;
        }

        vector<Contact*> results = nameTrie.searchByPrefix(prefix);

        if (results.empty()) {
            cout << "  [INFO] No contacts matching '" << prefix << "'.\n";
            return;
        }

        cout << "  Found " << results.size() << " contact(s):\n";
        cout << "  " << left << setw(5) << "#"
             << setw(20) << "Name"
             << setw(15) << "Phone"
             << setw(25) << "Email"
             << setw(12) << "Category\n";
        cout << "  " << string(80, '-') << "\n";
        for (size_t i = 0; i < results.size(); i++) {
            cout << "  " << left << setw(5) << (i + 1);
            results[i]->display();
        }
    }

    // -------------------------------------------------------------------
    // FEATURE 3: SEARCH BY PHONE NUMBER (Hash Table O(1))
    // Uses unordered_map to find the contact associated with a phone
    // number in constant time. Returns the contact details instantly.
    // -------------------------------------------------------------------
    void searchByPhone() {
        string phone;
        cout << "  Enter phone number: "; getline(cin, phone);

        auto it = phoneMap.find(phone);
        if (it == phoneMap.end()) {
            cout << "  [INFO] No contact found with phone '" << phone << "'.\n";
            return;
        }

        Contact* c = it->second;
        cout << "  Contact found:\n";
        cout << "  " << left << setw(20) << "Name"
             << setw(15) << "Phone"
             << setw(25) << "Email"
             << setw(12) << "Category\n";
        cout << "  " << string(72, '-') << "\n";
        c->display();
    }

    // -------------------------------------------------------------------
    // FEATURE 4: DISPLAY ALL CONTACTS (Alphabetically Sorted)
    // The linked list maintains sorted order on insertion, so displaying
    // is simply a linear traversal. No sorting needed at display time.
    // -------------------------------------------------------------------
    void displayAll() {
        contactList.displayAll();
        cout << "  Total contacts: " << contactList.getCount() << "\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 5: MAKE A CALL
    // Finds a contact by name and pushes a CallRecord onto the call
    // history stack. The record includes name, number, duration, and
    // timestamp (auto-generated).
    // -------------------------------------------------------------------
    void makeCall() {
        string name;
        cout << "  Enter contact name to call: "; getline(cin, name);

        Contact* c = contactList.findByName(name);
        if (!c) {
            cout << "  [ERROR] Contact '" << name << "' not found.\n";
            return;
        }

        // Push a new call record onto the stack
        callHistory.push(CallRecord(c->name, c->phoneNumber));
        cout << "  [OK] Calling " << c->name << " (" << c->phoneNumber << ")...\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 6: VIEW CALL HISTORY (Stack - Most Recent First)
    // Displays all call records from the stack in LIFO order.
    // We copy the stack to iterate without destroying the original data.
    // -------------------------------------------------------------------
    void viewCallHistory() {
        if (callHistory.empty()) {
            cout << "  [INFO] No call history. Make a call first.\n";
            return;
        }

        // Create a copy to traverse (stack has no iterator)
        stack<CallRecord> temp = callHistory;

        cout << "  Recent calls (most recent first):\n";
        cout << "  " << left << setw(5) << "#"
             << setw(18) << "Name"
             << setw(14) << "Phone"
             << setw(8) << "Duration"
             << "Timestamp\n";
        cout << "  " << string(75, '-') << "\n";

        int index = 1;
        while (!temp.empty()) {
            temp.top().display(index);
            temp.pop();
            index++;
        }
    }

    // -------------------------------------------------------------------
    // FEATURE 7: DELETE CONTACT
    // Removes a contact from all data structures. Before removing, saves
    // a copy of the Contact onto the undo stack so it can be restored.
    // Deletion order:
    //   1. Save to undo stack (for potential restoration)
    //   2. Remove from hash table (phoneMap)
    //   3. Remove from Trie
    //   4. Remove from linked list (frees the node and its Contact)
    // -------------------------------------------------------------------
    void deleteContact() {
        string name;
        cout << "  Enter contact name to delete: "; getline(cin, name);

        Contact* c = contactList.findByName(name);
        if (!c) {
            cout << "  [ERROR] Contact '" << name << "' not found.\n";
            return;
        }

        // Step 1: Push a copy onto the undo stack before deleting
        undoStack.push(*c);

        // Step 2: Remove from hash table (phone -> contact mapping)
        phoneMap.erase(c->phoneNumber);

        // Step 3: Remove from Trie (unmark endOfWord)
        nameTrie.remove(name);

        // Step 4: Remove from linked list (frees allocated memory)
        contactList.removeByName(name);

        cout << "  [OK] Contact '" << name << "' deleted.\n";
        cout << "  [INFO] Use 'Undo Delete' (option 8) to restore.\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 8: UNDO DELETE (Stack - LIFO Restoration)
    // Pops the most recently deleted contact from the undo stack and
    // re-inserts it into all data structures. Checks for conflicts
    // (name or phone already taken by a different contact).
    // -------------------------------------------------------------------
    void undoDelete() {
        if (undoStack.empty()) {
            cout << "  [INFO] No deleted contacts to restore.\n";
            return;
        }

        Contact c = undoStack.top();
        undoStack.pop();

        // Check for conflicts that would prevent restoration
        if (contactList.findByName(c.name)) {
            cout << "  [ERROR] A contact named '" << c.name
                 << "' already exists. Cannot undo.\n";
            return;
        }
        if (isPhoneTaken(c.phoneNumber)) {
            cout << "  [ERROR] Phone number '" << c.phoneNumber
                 << "' is already in use. Cannot undo.\n";
            return;
        }

        // Re-insert into all data structures
        addContactToStructures(c);
        cout << "  [OK] Contact '" << c.name << "' restored successfully.\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 9: UPDATE CONTACT DETAILS
    // Allows updating any field of a contact. If the name changes, we
    // must update the Trie (remove old key, insert new one). If the
    // phone changes, we must update the hash table similarly.
    // -------------------------------------------------------------------
    void updateContact() {
        string name;
        cout << "  Enter contact name to update: "; getline(cin, name);

        Contact* c = contactList.findByName(name);
        if (!c) {
            cout << "  [ERROR] Contact '" << name << "' not found.\n";
            return;
        }

        cout << "  Updating contact: " << name << "\n";
        cout << "  (Press Enter to keep current value)\n";

        string newName, newPhone, newEmail, newCategory;
        cout << "  Name [" << c->name << "]: "; getline(cin, newName);
        cout << "  Phone [" << c->phoneNumber << "]: "; getline(cin, newPhone);
        cout << "  Email [" << c->email << "]: "; getline(cin, newEmail);
        cout << "  Category [" << c->category << "]: "; getline(cin, newCategory);

        // Handle name change: must update Trie key
        if (!newName.empty() && newName != c->name) {
            // Check new name is not taken
            if (contactList.findByName(newName)) {
                cout << "  [ERROR] Contact '" << newName << "' already exists. Name not changed.\n";
            } else {
                nameTrie.remove(c->name);  // Remove old key
                c->name = newName;          // Update name
                nameTrie.insert(c->name, c); // Insert new key
            }
        }

        // Handle phone change: must update hash table key
        if (!newPhone.empty() && newPhone != c->phoneNumber) {
            if (isPhoneTaken(newPhone)) {
                cout << "  [ERROR] Phone '" << newPhone << "' already exists. Phone not changed.\n";
            } else {
                phoneMap.erase(c->phoneNumber);  // Remove old key
                c->phoneNumber = newPhone;        // Update phone
                phoneMap[c->phoneNumber] = c;     // Insert new key
            }
        }

        if (!newEmail.empty()) c->email = newEmail;
        if (!newCategory.empty()) c->category = newCategory;

        cout << "  [OK] Contact updated successfully.\n";
    }

    // -------------------------------------------------------------------
    // FEATURE 10: DISPLAY BY CATEGORY
    // Filters and displays all contacts belonging to a given category
    // (e.g., "Family", "Friends", "Work", "General").
    // Uses the linked list traversal filtered by category field.
    // -------------------------------------------------------------------
    void displayByCategory() {
        string category;
        cout << "  Enter category (Family, Friends, Work, General): "; getline(cin, category);
        if (category.empty()) {
            cout << "  [INFO] No category entered.\n";
            return;
        }
        contactList.displayByCategory(category);
    }

    // ====================================================================
    // RUN - Main interactive loop for the phonebook system
    // Displays a menu and processes user choices in a loop.
    // Each menu option maps to one of the features above.
    // ====================================================================
    void run() {
        cout << "\n=============================================\n";
        cout << "   PHONEBOOK MANAGEMENT SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;

        do {
            cout << "\n------ PHONEBOOK MENU ------\n";
            cout << "  1.  Add Contact\n";
            cout << "  2.  Search by Name (Prefix Autocomplete)\n";
            cout << "  3.  Search by Phone Number (Instant)\n";
            cout << "  4.  Display All Contacts (Sorted)\n";
            cout << "  5.  Make a Call\n";
            cout << "  6.  View Call History\n";
            cout << "  7.  Delete Contact\n";
            cout << "  8.  Undo Delete\n";
            cout << "  9.  Update Contact\n";
            cout << " 10.  Display by Category\n";
            cout << "  0.  Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();  // Clear newline from input buffer

            switch (choice) {
                case 1:  addContact();       break;
                case 2:  searchByName();     break;
                case 3:  searchByPhone();    break;
                case 4:  displayAll();       break;
                case 5:  makeCall();         break;
                case 6:  viewCallHistory();  break;
                case 7:  deleteContact();    break;
                case 8:  undoDelete();       break;
                case 9:  updateContact();    break;
                case 10: displayByCategory();break;
                case 0:
                    cout << "  Exiting Phonebook. Goodbye!\n";
                    break;
                default:
                    cout << "  [ERROR] Invalid choice. Please enter 0-10.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Program entry point
// ============================================================================
int main() {
    PhoneBook phonebook;
    phonebook.run();
    return 0;
}
