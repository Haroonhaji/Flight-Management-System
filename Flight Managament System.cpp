#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// ------------- CONSTANTS AND STRUCTURES -------------

const int maxCities = 5;                        // Maximum number of cities allowed
const int positiveInfinity = 999999;            // Used as a substitute for 'infinite' distance

// Structure to store each city's basic info
struct City {
    int cityId;                                 // Unique ID for each city
    string name;                                // Name of the city
};

// Node structure for the Binary Search Tree used to store flight times
struct BSTNode {
    int time;                                   // Flight time (e.g., 1730 for 5:30 PM)
    BSTNode *left;                              // Pointer to left child
    BSTNode *right;                             // Pointer to right child
};

// ---------------- GLOBAL VARIABLES ------------------

int totalCities = 0;                                            // Tracks total number of cities added
City cities[maxCities];                                         // Array of City objects
int routes[maxCities][maxCities];                               // Adjacency matrix for storing costs between cities
BSTNode *flightTimes[maxCities][maxCities];                     // Matrix of BSTs, each storing flight times for a city pair
                                                                // Non-null BSTs exist only between cities that are connected
                                                                // BST helps retrieve the flight time closest to desired time in O(log n)

// ---------------- UTILITY FUNCTIONS ------------------

int findCityIndex(int cityId) {
    // Returns index of city in 'cities' array corresponding to the given ID
    for (int i = 0; i < totalCities; i++) {
        if (cities[i].cityId == cityId)
            return i;
    }
    return -1;
}

BSTNode *insertFlightTime(BSTNode *root, int time) {
    // Inserts a new flight time into the BST
    if (!root) {
        BSTNode *newNode = new BSTNode();
        newNode->time = time;
        newNode->left = nullptr;
        newNode->right = nullptr;
        return newNode;
    }
    if (time < root->time)
        root->left = insertFlightTime(root->left, time);
    else if (time > root->time)
        root->right = insertFlightTime(root->right, time);
    return root;
}

void printFlightTimes(BSTNode *root) {
    // In-order traversal of BST to print times in ascending order
    if (!root) return;
    printFlightTimes(root->left);
    cout << root->time << " | ";
    printFlightTimes(root->right);
}

bool validCityPair(int fromId, int toId) {
    // Validates if both city IDs are present in the system
    int fromIndex = findCityIndex(fromId);
    int toIndex = findCityIndex(toId);
    return (fromIndex != -1 && toIndex != -1);
}

int findClosestTime(BSTNode *root, int targetTime, int closest) {
    // Finds flight time in BST that is closest to targetTime
    if (!root) return closest;
    if (abs(targetTime - root->time) < abs(targetTime - closest))
        closest = root->time;
    if (targetTime < root->time)
        return findClosestTime(root->left, targetTime, closest);
    else
        return findClosestTime(root->right, targetTime, closest);
}

// ---------------- SHORTEST PATH FUNCTION ------------------


// 2 ways to implement Dijkstra's Algo :


// Dijkstra’s Algorithm (Linear Array Version)

// void findPath(int startId, int endId) {
//     if (!validCityPair(startId, endId)) {
//         cout << "Invalid city IDs." << endl;
//         return;
//     }

//     int fromIndex = findCityIndex(startId);
//     int toIndex = findCityIndex(endId);

//     int dist[maxCities];                     // Stores shortest distance from source to each city
//     bool visited[maxCities] = {false};       // Tracks whether a city has been visited
//     int parent[maxCities];                   // For reconstructing path

//     for (int i = 0; i < totalCities; i++) {
//         dist[i] = positiveInfinity;
//         parent[i] = -1;
//     }

//     dist[fromIndex] = 0;

//     for (int i = 0; i < totalCities; i++) {
//         int u = -1;
//         for (int j = 0; j < totalCities; j++) {
//             // Select the unvisited city with the minimum distance
//             if (!visited[j] && (u == -1 || dist[j] < dist[u]))
//                 u = j;
//         }

//         if (u == -1 || dist[u] == positiveInfinity)
//             break;

//         visited[u] = true;

//         if (u == toIndex)
//             break;                          // Optimization: Exit early if destination is reached

//         // Relax all adjacent edges
//         for (int v = 0; v < totalCities; v++) {
//             if (routes[u][v] > 0 && !visited[v] && dist[u] + routes[u][v] < dist[v]) {
//                 dist[v] = dist[u] + routes[u][v];
//                 parent[v] = u;
//             }
//         }
//     }

//     if (dist[toIndex] == positiveInfinity) {
//         cout << "No route exists between the cities." << endl;
//         return;
//     }

//     // Reconstruct and display the shortest path
//     cout << "Shortest route cost: " << dist[toIndex] << "\nPath: ";
//     int path[maxCities];
//     int count = 0;
//     int current = toIndex;
//     while (current != -1) {
//         path[count++] = current;
//         current = parent[current];
//     }
//     for (int i = count - 1; i >= 0; i--) {
//         cout << cities[path[i]].name;
//         if (i > 0)
//             cout << " ==> ";
//     }
//     cout << endl;
// }


// Dijkstra’s Algorithm (Priority Queue Version)

void findPath(int startId, int endId) {
    if (!validCityPair(startId, endId)) {
        cout << "Invalid city IDs.\n";
        return;
    }

    int fromIdx = findCityIndex(startId);
    int toIdx   = findCityIndex(endId);

    const int INF = positiveInfinity;
    vector<int> dist(totalCities, INF);         // Stores shortest distances from source
    vector<int> parent(totalCities, -1);        // To reconstruct the path
    vector<bool> visited(totalCities, false);   // Mark visited nodes

    using PII = pair<int, int>;                 // (distance, cityIndex)
    priority_queue<PII, vector<PII>, greater<PII>> pq;

    dist[fromIdx] = 0;
    pq.push({0, fromIdx});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (visited[u]) continue;               // Skip if already visited (lazy deletion)
        visited[u] = true;

        if (u == toIdx) break;                  // Early exit if target reached

        for (int v = 0; v < totalCities; ++v) {
            int cost = routes[u][v];
            if (cost > 0 && !visited[v] && d + cost < dist[v]) {
                dist[v] = d + cost;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[toIdx] == INF) {
        cout << "No route exists between the cities.\n";
        return;
    }

    // Reconstruct path
    cout << "Shortest route cost: " << dist[toIdx] << "\nPath: ";
    vector<int> path;
    for (int cur = toIdx; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
        cout << cities[path[i]].name;
        if (i < path.size() - 1)
            cout << " ==> ";
    }
    cout << '\n';
}


// ------------------ MAIN FUNCTION -----------------------

int main() {
    // Initialize route and flightTimes matrices
    for (int i = 0; i < maxCities; i++)
        for (int j = 0; j < maxCities; j++) {
            routes[i][j] = -1;
            flightTimes[i][j] = nullptr;
        }

    int choice;
    do {
        cout << "\nMain Menu\n1. Admin\n2. User\n3. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
            // ---------- ADMIN INTERFACE ----------
            int adminChoice;
            do {
                cout << "\nAdmin Menu\n1. Add City\n2. Add/Update Route\n3. Add Flight Time\n4. Display Cities and Routes\n5. Display Flight Times\n6. Exit\n";
                cin >> adminChoice;

                if (adminChoice == 1) {
                    int cityId;
                    string cityName;
                    cout << "Enter City ID: ";
                    cin >> cityId;
                    cout << "Enter City Name: ";
                    cin >> cityName;

                    if (totalCities >= maxCities) {
                        cout << "Cannot add more cities." << endl;
                        continue;
                    }

                    if (findCityIndex(cityId) != -1) {
                        cout << "City with ID " << cityId << " already exists." << endl;
                        continue;
                    }

                    cities[totalCities++] = {cityId, cityName};
                    cout << "City added successfully." << endl;
                }

                else if (adminChoice == 2) {
                    int fromId, toId, cost;
                    cout << "Enter Source ID: ";
                    cin >> fromId;
                    cout << "Enter Destination ID: ";
                    cin >> toId;
                    cout << "Enter Cost: ";
                    cin >> cost;

                    if (validCityPair(fromId, toId)) {
                        int fromIndex = findCityIndex(fromId);
                        int toIndex = findCityIndex(toId);
                        routes[fromIndex][toIndex] = cost;
                        cout << "Route updated successfully." << endl;
                    } else {
                        cout << "Invalid city IDs." << endl;
                    }
                }

                else if (adminChoice == 3) {
                    int fromId, toId, time;
                    cout << "Enter Source ID: ";
                    cin >> fromId;
                    cout << "Enter Destination ID: ";
                    cin >> toId;
                    cout << "Enter Flight Time: ";
                    cin >> time;

                    if (!validCityPair(fromId, toId)) {
                        cout << "Invalid city IDs." << endl;
                    } else {
                        int fromIndex = findCityIndex(fromId);
                        int toIndex = findCityIndex(toId);
                        flightTimes[fromIndex][toIndex] = insertFlightTime(flightTimes[fromIndex][toIndex], time);
                        cout << "Flight time added successfully." << endl;
                    }
                }

                else if (adminChoice == 4) {
                    cout << "\nCities:\n";
                    for (int i = 0; i < totalCities; i++) {
                        cout << "City ID: " << cities[i].cityId << ", Name: " << cities[i].name << endl;
                    }

                    cout << "\nRoutes:\n";
                    for (int i = 0; i < totalCities; i++) {
                        for (int j = 0; j < totalCities; j++) {
                            if (routes[i][j] > 0) {
                                cout << "From " << cities[i].name << " to " << cities[j].name << " : Cost = " << routes[i][j] << endl;
                            }
                        }
                    }
                }

                else if (adminChoice == 5) {
                    int fromId, toId;
                    cout << "Enter Source ID: ";
                    cin >> fromId;
                    cout << "Enter Destination ID: ";
                    cin >> toId;

                    if (!validCityPair(fromId, toId)) {
                        cout << "Invalid city IDs." << endl;
                    } else {
                        int fromIndex = findCityIndex(fromId);
                        int toIndex = findCityIndex(toId);
                        cout << "Flight times from " << cities[fromIndex].name << " to " << cities[toIndex].name << ": ";
                        if (flightTimes[fromIndex][toIndex])
                            printFlightTimes(flightTimes[fromIndex][toIndex]);
                        else
                            cout << "No flight times available.";
                        cout << endl;
                    }
                }

            } while (adminChoice != 6);
        }

        else if (choice == 2) {
            // ---------- USER INTERFACE ----------
            int userChoice;
            do {
                cout << "\nUser Menu\n1. Search Flight\n2. View All Cities\n3. Flight Time\n4. Exit\n";
                cin >> userChoice;

                if (userChoice == 1) {
                    int startId, endId;
                    cout << "Enter source city ID: ";
                    cin >> startId;
                    cout << "Enter destination city ID: ";
                    cin >> endId;
                    findPath(startId, endId);
                }

                else if (userChoice == 2) {
                    for (int i = 0; i < totalCities; i++) {
                        cout << "City ID: " << cities[i].cityId << ", Name: " << cities[i].name << endl;
                    }
                }

                else if (userChoice == 3) {
                    int startId, endId, flightTime;
                    cout << "Enter source city ID: ";
                    cin >> startId;
                    cout << "Enter destination city ID: ";
                    cin >> endId;
                    cout << "Enter desired time: ";
                    cin >> flightTime;

                    int fromIndex = findCityIndex(startId);
                    int toIndex = findCityIndex(endId);

                    if (fromIndex == -1 || toIndex == -1) {
                        cout << "Invalid city IDs." << endl;
                    } else {
                        BSTNode *bstTree = flightTimes[fromIndex][toIndex];
                        int time = findClosestTime(bstTree, flightTime, positiveInfinity);
                        if (time != positiveInfinity)
                            cout << "Closest flight found: " << time << endl;
                        else
                            cout << "No flights." << endl;
                    }
                }

            } while (userChoice != 4);
        }

    } while (choice != 3);

    cout << "Exiting program." << endl;
    return 0;
}
