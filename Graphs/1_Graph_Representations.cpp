/*
 * Topic: Graph Representations
 * Description: Demonstrates how to represent a Graph using:
 *  1. Adjacency Matrix (2D Array)
 *  2. Adjacency List (Array of Vectors/Linked Lists)
 * 
 * Complexities:
 * - Adjacency Matrix: 
 *   Space = O(V^2), Edge Lookup = O(1), Add Edge = O(1)
 * - Adjacency List:
 *   Space = O(V + E), Edge Lookup = O(degree(V)), Add Edge = O(1)
 */

#include <iostream>
#include <vector>
using namespace std;

// 1. Adjacency Matrix Representation
class GraphMatrix {
private:
    int V;
    vector<vector<int>> matrix;

public:
    GraphMatrix(int vertices) : V(vertices) {
        // Initialize a V x V matrix with 0s
        matrix.assign(V, vector<int>(V, 0));
    }

    // Add an undirected edge
    void addEdge(int u, int v) {
        matrix[u][v] = 1;
        matrix[v][u] = 1; // Since it's undirected
    }

    void print() {
        cout << "Adjacency Matrix:\n";
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << "\n";
        }
    }
};

// 2. Adjacency List Representation
class GraphList {
private:
    int V;
    vector<vector<int>> adjList;

public:
    GraphList(int vertices) : V(vertices) {
        adjList.resize(V);
    }

    // Add an undirected edge
    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u); // Since it's undirected
    }

    void print() {
        cout << "Adjacency List:\n";
        for (int i = 0; i < V; i++) {
            cout << "Vertex " << i << ": ";
            for (int neighbor : adjList[i]) {
                cout << neighbor << " -> ";
            }
            cout << "NULL\n";
        }
    }
};

int main() {
    int V = 4; // 4 Vertices: 0, 1, 2, 3

    // Matrix
    GraphMatrix gm(V);
    gm.addEdge(0, 1);
    gm.addEdge(0, 2);
    gm.addEdge(1, 2);
    gm.addEdge(2, 3);
    gm.print();

    cout << "\n------------------------\n\n";

    // List
    GraphList gl(V);
    gl.addEdge(0, 1);
    gl.addEdge(0, 2);
    gl.addEdge(1, 2);
    gl.addEdge(2, 3);
    gl.print();

    return 0;
}
