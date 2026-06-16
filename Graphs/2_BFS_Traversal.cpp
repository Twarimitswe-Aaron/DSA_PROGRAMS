/*
 * Topic: Breadth-First Search (BFS) for Graph
 * Description: Traverses the graph level by level starting from a source node using a Queue.
 *
 * Time Complexity: O(V + E) where V is vertices, E is edges.
 * Space Complexity: O(V) for visited array and queue.
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Undirected graph
    }

    // Perform BFS starting from vertex 'start'
    void BFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        // Mark the current node as visited and enqueue it
        visited[start] = true;
        q.push(start);

        cout << "BFS Traversal starting from " << start << ": ";

        while (!q.empty()) {
            // Dequeue a vertex from queue and print it
            int curr = q.front();
            q.pop();
            cout << curr << " ";

            // Get all adjacent vertices of the dequeued vertex.
            // If an adjacent has not been visited, mark it visited and enqueue it.
            for (int neighbor : adj[curr]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
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

    g.BFS(0);

    return 0;
}
