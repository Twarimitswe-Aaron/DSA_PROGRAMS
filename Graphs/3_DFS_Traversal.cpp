/*
 * Topic: Depth-First Search (DFS) for Graph
 * Description: Traverses the graph by exploring as far as possible along each branch before backtracking, using Recursion (Call Stack).
 *
 * Time Complexity: O(V + E) where V is vertices, E is edges.
 * Space Complexity: O(V) for visited array and recursion stack.
 */

#include <iostream>
#include <vector>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;

    // Recursive helper function
    void DFSUtil(int v, vector<bool>& visited) {
        // Mark the current node as visited and print it
        visited[v] = true;
        cout << v << " ";

        // Recur for all the vertices adjacent to this vertex
        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                DFSUtil(neighbor, visited);
            }
        }
    }

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Undirected graph
    }

    // DFS starting from vertex 'start'
    void DFS(int start) {
        vector<bool> visited(V, false);
        cout << "DFS Traversal starting from " << start << ": ";
        
        // Call the recursive helper function
        DFSUtil(start, visited);
        cout << endl;
    }
};

int main() {
    Graph g(5); // 5 vertices (0 to 4)
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 4);

    g.DFS(0);

    return 0;
}
