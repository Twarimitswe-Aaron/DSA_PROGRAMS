/*
 * ============================================================================
 * FILE: 08_graphs.cpp
 * ============================================================================
 * GRAPHS - Non-linear data structure consisting of vertices (nodes) and edges.
 *
 * EXAM FREQUENCY: High. Graph problems are common in practical exams.
 *
 * Representations Covered:
 *   1. Adjacency Matrix (2D array) - O(V^2) space
 *   2. Adjacency List (vector of lists) - O(V+E) space (preferred for sparse)
 *
 * Traversals:
 *   3. BFS (Breadth-First Search) - Uses queue, finds shortest path
 *   4. DFS (Depth-First Search) - Uses stack/recursion
 *
 * Common Problems:
 *   5. Detect cycle in directed graph (DFS with recursion stack)
 *   6. Detect cycle in undirected graph (DFS with parent tracking)
 *   7. Topological Sort (DFS-based / Kahn's algorithm)
 *   8. Dijkstra's Shortest Path (using min-heap, O(E log V))
 *   9. Prim's MST (Minimum Spanning Tree)
 *   10. Bipartite Graph Check (BFS coloring)
 *
 * Graph Terminology:
 *   - V = number of vertices, E = number of edges
 *   - Directed vs Undirected edges
 *   - Weighted vs Unweighted edges
 *   - Connected vs Disconnected components
 *   - Cycle: path where first and last vertex are the same
 * ============================================================================
 */
#include <iostream>
#include <vector>
#include <list>       // For adjacency list
#include <queue>      // For BFS, Dijkstra
#include <stack>      // For DFS (iterative)
#include <climits>    // For INT_MAX (infinity in Dijkstra)
#include <algorithm>  // For reverse in topological sort
using namespace std;

// ============================================================================
// SECTION 1: GRAPH CLASS with BOTH representations
// ============================================================================

class Graph {
private:
    int V;                    // Number of vertices (0 to V-1)
    bool isDirected;          // true = directed graph, false = undirected

    // Representation 1: Adjacency Matrix (V x V)
    // matrix[u][v] = weight of edge u->v (1 for unweighted, 0 for no edge)
    vector<vector<int>> matrix;

    // Representation 2: Adjacency List (array of lists)
    // Each list stores {neighbor, weight} pairs
    vector<list<pair<int, int>>> adjList;

public:
    Graph(int vertices, bool directed = false) : V(vertices), isDirected(directed) {
        // Initialize adjacency matrix with zeros
        matrix.assign(V, vector<int>(V, 0));
        // Initialize adjacency list (empty lists)
        adjList.resize(V);
    }

    // -------------------------------------------------------------------
    // ADD EDGE to BOTH representations (always keep them in sync)
    //
    // For undirected graphs: add edge in both directions (u->v and v->u)
    // weight = 1 for unweighted graphs
    // -------------------------------------------------------------------
    void addEdge(int u, int v, int weight = 1) {
        // Update adjacency matrix
        matrix[u][v] = weight;
        // Update adjacency list
        adjList[u].push_back({v, weight});

        if (!isDirected) {
            // For undirected, add reverse edge too
            matrix[v][u] = weight;
            adjList[v].push_back({u, weight});
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY both representations
    // -------------------------------------------------------------------

    void printMatrix() {
        cout << "  Adjacency Matrix:\n";
        for (int i = 0; i < V; i++) {
            cout << "    ";
            for (int j = 0; j < V; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << "\n";
        }
    }

    void printAdjList() {
        cout << "  Adjacency List:\n";
        for (int i = 0; i < V; i++) {
            cout << "    " << i << ": ";
            for (auto& neighbor_pair : adjList[i]) { int neighbor = neighbor_pair.first; int weight = neighbor_pair.second;
                cout << "(" << neighbor << "," << weight << ") ";
            }
            cout << "\n";
        }
    }

    // ==================================================================
    // SECTION 2: BFS (Breadth-First Search)
    //
    // How it works:
    //   1. Start from source vertex, mark as visited, enqueue it
    //   2. Dequeue a vertex, visit all its unvisited neighbors, enqueue them
    //   3. Repeat until queue is empty
    //
    // Properties:
    //   - Visits vertices in order of increasing distance from source
    //   - Finds shortest path (in terms of number of edges) in unweighted graphs
    //   - O(V + E) time, O(V) space (for visited array)
    // ==================================================================
    void BFS(int source) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[source] = true;
        q.push(source);

        cout << "  BFS traversal from " << source << ": ";

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << u << " ";

            // Visit all neighbors of u
            for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << "\n";
    }

    // -------------------------------------------------------------------
    // BFS Shortest Path (in unweighted graph)
    // Returns distance from source to all vertices (-1 if unreachable)
    // -------------------------------------------------------------------
    vector<int> shortestPath(int source) {
        vector<int> dist(V, -1);    // -1 = unreachable
        queue<int> q;

        dist[source] = 0;            // Distance to source is 0
        q.push(source);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                if (dist[v] == -1) {  // Not visited yet
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
            }
        }
        return dist;
    }

    // ==================================================================
    // SECTION 3: DFS (Depth-First Search)
    //
    // How it works:
    //   Recursive: Visit node, then recursively visit each unvisited neighbor.
    //   Iterative: Use stack (LIFO) - push node, pop, push unvisited neighbors.
    //
    // Properties:
    //   - Explores as far as possible before backtracking
    //   - Useful for: topological sort, cycle detection, connected components
    //   - O(V + E) time, O(V) space (visited + recursion stack)
    // ==================================================================

    // Recursive DFS helper
    void dfsRecursive(int u, vector<bool>& visited) {
        visited[u] = true;
        cout << u << " ";

        for (auto& neighbor_pair : adjList[u]) {
            int v = neighbor_pair.first;
            if (!visited[v]) {
                dfsRecursive(v, visited);
            }
        }
    }

    void DFS(int source) {
        vector<bool> visited(V, false);
        cout << "  DFS (recursive) from " << source << ": ";
        dfsRecursive(source, visited);
        cout << "\n";
    }

    // Iterative DFS using an explicit stack
    void DFSIterative(int source) {
        vector<bool> visited(V, false);
        stack<int> st;

        st.push(source);

        cout << "  DFS (iterative) from " << source << ": ";

        while (!st.empty()) {
            int u = st.top();
            st.pop();

            if (!visited[u]) {
                visited[u] = true;
                cout << u << " ";

                // Push neighbors in reverse order to mimic recursion order
                // (since stack is LIFO, reversing gives natural DFS order)
                vector<int> neighbors;
                for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                    if (!visited[v]) neighbors.push_back(v);
                }
                for (auto it = neighbors.rbegin(); it != neighbors.rend(); it++) {
                    st.push(*it);
                }
            }
        }
        cout << "\n";
    }

    // -------------------------------------------------------------------
    // DFS to explore all components (for disconnected graphs)
    // -------------------------------------------------------------------
    void DFSAll() {
        vector<bool> visited(V, false);
        cout << "  DFS (all components):\n";
        int componentId = 0;
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                cout << "    Component " << ++componentId << ": ";
                dfsRecursive(i, visited);
                cout << "\n";
            }
        }
    }

    // ==================================================================
    // SECTION 4: CYCLE DETECTION
    // ==================================================================

    // -------------------------------------------------------------------
    // CYCLE IN UNDIRECTED GRAPH (DFS with parent tracking)
    // If we find a visited neighbor that is NOT the parent, we have a cycle.
    // -------------------------------------------------------------------
    bool hasCycleUndirected() {
        vector<bool> visited(V, false);
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                if (hasCycleUndirectedDFS(i, -1, visited))
                    return true;
            }
        }
        return false;
    }

    bool hasCycleUndirectedDFS(int u, int parent, vector<bool>& visited) {
        visited[u] = true;
        for (auto& neighbor_pair : adjList[u]) {
            int v = neighbor_pair.first;
            if (!visited[v]) {
                if (hasCycleUndirectedDFS(v, u, visited))
                    return true;
            } else if (v != parent) {
                // Visited neighbor that is NOT parent = back edge = cycle
                return true;
            }
        }
        return false;
    }

    // -------------------------------------------------------------------
    // CYCLE IN DIRECTED GRAPH (DFS with recursion stack tracking)
    // A back edge to a node currently in the recursion stack indicates a cycle.
    // -------------------------------------------------------------------
    bool hasCycleDirected() {
        if (!isDirected) return hasCycleUndirected();

        vector<bool> visited(V, false);
        vector<bool> recStack(V, false);  // Tracks nodes in current recursion path

        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                if (hasCycleDirectedDFS(i, visited, recStack))
                    return true;
            }
        }
        return false;
    }

    bool hasCycleDirectedDFS(int u, vector<bool>& visited, vector<bool>& recStack) {
        visited[u] = true;
        recStack[u] = true;

        for (auto& neighbor_pair : adjList[u]) {
            int v = neighbor_pair.first;
            if (!visited[v]) {
                if (hasCycleDirectedDFS(v, visited, recStack))
                    return true;
            } else if (recStack[v]) {
                // v is in the current recursion stack = back edge = cycle
                return true;
            }
        }
        recStack[u] = false;  // Remove from recursion stack when backtracking
        return false;
    }

    // ==================================================================
    // SECTION 5: TOPOLOGICAL SORT (Only for Directed Acyclic Graphs)
    //
    // Topological order = linear ordering where for every edge u->v,
    // u appears before v. Used for:
    //   - Course prerequisites scheduling
    //   - Build systems (makefiles)
    //   - Task scheduling
    //
    // Two approaches:
    //   A. DFS-based: push to stack after visiting all descendants
    //   B. Kahn's Algorithm (BFS-based): remove vertices with in-degree 0
    // ==================================================================

    // Approach A: DFS-based Topological Sort
    void topologicalSortDFS(int u, vector<bool>& visited, stack<int>& st) {
        visited[u] = true;
        for (auto& neighbor_pair : adjList[u]) {
            int v = neighbor_pair.first;
            if (!visited[v]) {
                topologicalSortDFS(v, visited, st);
            }
        }
        st.push(u);  // Push after all descendants are processed
    }

    vector<int> topologicalSort() {
        if (!isDirected) {
            cout << "  Topological sort only defined for directed graphs.\n";
            return {};
        }

        vector<bool> visited(V, false);
        stack<int> st;  // Stores the topological order (reversed)

        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                topologicalSortDFS(i, visited, st);
            }
        }

        // Extract order from stack
        vector<int> order;
        while (!st.empty()) {
            order.push_back(st.top());
            st.pop();
        }
        return order;
    }

    // Approach B: Kahn's Algorithm (BFS-based)
    vector<int> topologicalSortKahn() {
        vector<int> inDegree(V, 0);
        // Compute in-degree of each vertex
        for (int u = 0; u < V; u++) {
            for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                inDegree[v]++;
            }
        }

        queue<int> q;
        // Enqueue all vertices with in-degree 0
        for (int i = 0; i < V; i++) {
            if (inDegree[i] == 0) q.push(i);
        }

        vector<int> order;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);

            // Reduce in-degree of all neighbors (removing edges from u)
            for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                if (--inDegree[v] == 0) q.push(v);
            }
        }

        // If order doesn't contain all vertices, there was a cycle
        if ((int)order.size() != V) {
            cout << "  Graph has a cycle! Topological sort not possible.\n";
            return {};
        }
        return order;
    }

    // ==================================================================
    // SECTION 6: DIJKSTRA'S SHORTEST PATH (from source to all vertices)
    //
    // Works on weighted graphs with NON-NEGATIVE edge weights.
    // Uses a min-heap (priority queue) to efficiently pick the next vertex
    // with the smallest distance.
    //
    // Time: O((V + E) log V) with binary heap
    // ==================================================================
    vector<int> dijkstra(int source) {
        vector<int> dist(V, INT_MAX);   // Initialize all distances to infinity
        // Min-heap: stores {distance, vertex}, ordered by distance
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[source] = 0;
        pq.push({0, source});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            // Skip if we already found a shorter path (stale entry in heap)
            if (d > dist[u]) continue;

            // Relax all neighbors
            for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first; int w = neighbor_pair.second;
                if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist;
    }

    // ==================================================================
    // SECTION 7: BIPARTITE GRAPH CHECK
    //
    // A graph is bipartite if vertices can be colored with 2 colors such
    // that no two adjacent vertices share the same color.
    // Equivalent: graph has no odd-length cycles.
    // Uses BFS to assign colors (0 or 1).
    // ==================================================================
    bool isBipartite() {
        vector<int> color(V, -1);  // -1 = uncolored, 0/1 = colors
        queue<int> q;

        for (int start = 0; start < V; start++) {
            if (color[start] != -1) continue;  // Already colored (part of visited component)

            color[start] = 0;  // Start with color 0
            q.push(start);

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                for (auto& neighbor_pair : adjList[u]) { int v = neighbor_pair.first;
                    if (color[v] == -1) {
                        color[v] = 1 - color[u];  // Assign opposite color
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        return false;  // Same color on adjacent vertices = not bipartite
                    }
                }
            }
        }
        return true;
    }
};

// ============================================================================
// SAMPLE main() to demonstrate all graph operations
// ============================================================================
int main() {
    cout << "========== GRAPHS (08_graphs.cpp) ==========\n\n";

    cout << "===== UNDIRECTED GRAPH (5 vertices) =====\n";
    Graph g(5, false);
    g.addEdge(0, 1);
    g.addEdge(0, 4);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.printAdjList();
    g.printMatrix();

    cout << "\nTraversals:\n";
    g.BFS(0);
    g.DFS(0);
    g.DFSIterative(0);
    g.DFSAll();

    cout << "\nShortest paths from 0:\n";
    vector<int> dist = g.shortestPath(0);
    for (int i = 0; i < 5; i++) {
        cout << "  Distance to " << i << ": " << dist[i] << "\n";
    }

    cout << "\nCycle detection (undirected): "
         << (g.hasCycleUndirected() ? "Has cycle" : "No cycle") << "\n";

    cout << "\n===== DIRECTED GRAPH (6 vertices) =====\n";
    Graph dg(6, true);
    dg.addEdge(5, 2);
    dg.addEdge(5, 0);
    dg.addEdge(4, 0);
    dg.addEdge(4, 1);
    dg.addEdge(2, 3);
    dg.addEdge(3, 1);

    cout << "Topological Sort (DFS):\n  ";
    vector<int> topo = dg.topologicalSort();
    for (int v : topo) cout << v << " ";
    cout << "\n";

    cout << "Topological Sort (Kahn):\n  ";
    topo = dg.topologicalSortKahn();
    for (int v : topo) cout << v << " ";
    cout << "\n";

    cout << "\n===== DIJKSTRA (Weighted Directed Graph) =====\n";
    Graph wg(5, true);
    wg.addEdge(0, 1, 4);
    wg.addEdge(0, 2, 1);
    wg.addEdge(2, 1, 2);
    wg.addEdge(1, 3, 1);
    wg.addEdge(2, 3, 5);
    wg.addEdge(3, 4, 3);

    vector<int> shortest = wg.dijkstra(0);
    cout << "Shortest paths from 0:\n";
    for (int i = 0; i < 5; i++) {
        cout << "  To " << i << ": " << (shortest[i] == INT_MAX ? -1 : shortest[i]) << "\n";
    }

    cout << "\n===== BIPARTITE CHECK =====\n";
    Graph bp(4, false);
    bp.addEdge(0, 1); bp.addEdge(1, 2);
    bp.addEdge(2, 3); bp.addEdge(3, 0);
    cout << "  Graph (even cycle): " << (bp.isBipartite() ? "Bipartite" : "Not bipartite") << "\n";

    Graph notBp(3, false);
    notBp.addEdge(0, 1); notBp.addEdge(1, 2); notBp.addEdge(2, 0);
    cout << "  Graph (triangle):  " << (notBp.isBipartite() ? "Bipartite" : "Not bipartite") << "\n";

    cout << "\n========== END OF GRAPH OPERATIONS ==========\n";

    return 0;
}
