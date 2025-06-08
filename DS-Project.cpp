#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <fstream>
#include <climits>
#include <algorithm>
using namespace std;

// Base User class
class User {
protected:
    string name;
    int id;
public:
    User(string name, int id) : name(name), id(id) {}
    virtual void displayInfo() = 0;
    string getName() const { return name; }
};

// Driver class
class Driver : public User {
public:
    int location;
    bool available;
    string vehicleType;
    int availableIn; // duration in minutes if unavailable

    Driver(string name, int id, int loc, string vType)
        : User(name, id), location(loc), available(true), vehicleType(vType), availableIn(0) {}

    void displayInfo() override {
        cout << "Driver: " << name << " | Vehicle: " << vehicleType
             << " | Location: Node " << location << " | ";
        if (available)
            cout << "[Available]";
        else
            cout << "[Unavailable till " << availableIn << " min]";
        cout << endl;
    }

    bool isAvailable() {
        return available;
    }

    void markUnavailable(int duration) {
        available = false;
        availableIn = duration;
    }

    void makeAvailable() {
        available = true;
        availableIn = 0;
    }
};

// Rider class
class Rider : public User {
public:
    Rider(string name, int id) : User(name, id) {}

    void displayInfo() override {
        cout << "Rider: " << name << endl;
    }
};

// Graph for city map
class Graph {
public:
    unordered_map<int, vector<pair<int, int>>> adj;

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    unordered_map<int, int> dijkstra(int src) {
        unordered_map<int, int> dist;
        for (auto& pair : adj) dist[pair.first] = INT_MAX;
        dist[src] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            int d = pq.top().first;
            int node = pq.top().second;
            pq.pop();

            for (auto& neighbor : adj[node]) {
                int next = neighbor.first, weight = neighbor.second;
                if (dist[next] > d + weight) {
                    dist[next] = d + weight;
                    pq.push({dist[next], next});
                }
            }
        }
        return dist;
    }
};

// Ride Manager
class RideManager {
    vector<Driver> drivers;
    Graph cityMap;

public:
    void registerDriver(Driver d) {
        drivers.push_back(d);
    }

    void addRoad(int u, int v, int w) {
        cityMap.addEdge(u, v, w);
    }

    int getFareMultiplier(const std::string& vType) {
        std::string type = vType;
        transform(type.begin(), type.end(), type.begin(),
                  [](unsigned char c) { return std::tolower(c); });

        if (type == "bike") return 1;
        if (type == "auto") return 2;
        if (type == "car")  return 3;
        if (type == "suv")  return 4;

        return 1; // default
    }

    string toLowerCase(const string& str) {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                  [](unsigned char c) { return tolower(c); });
        return lowerStr;
    }

    void bookRide(Rider r, int pickupNode, int dropNode) {
        cout << "\nAvailable Vehicle Types: Bike, Auto, Car, SUV";
        cout << "\nEnter your preferred vehicle type: ";
        string vType;
        cin >> vType;

        cout << "\nBooking ride for " << r.getName() << " from Node " << pickupNode << " to Node " << dropNode << endl;

        Driver* nearest = nullptr;
        int minDist = INT_MAX;

        for (auto& d : drivers) {
            if (!d.isAvailable() || toLowerCase(d.vehicleType) != toLowerCase(vType)) continue;

            auto distFromDriver = cityMap.dijkstra(d.location);
            if (distFromDriver.find(pickupNode) != distFromDriver.end() && distFromDriver[pickupNode] < minDist) {
                minDist = distFromDriver[pickupNode];
                nearest = &d;
            }
        }

        if (nearest) {
            auto rideDistances = cityMap.dijkstra(pickupNode);
            int pickupToDropDist = rideDistances[dropNode];
            int duration = pickupToDropDist * 5; // in minutes
            int costPerUnit = 10;
            int fare = pickupToDropDist * costPerUnit * getFareMultiplier(nearest->vehicleType);

            nearest->markUnavailable(duration);

            cout << "--------------------------------------\n";
            cout << " Ride Details\n";
            cout << " Rider: " << r.getName() << "\n";
            cout << " Vehicle Type: " << nearest->vehicleType << "\n";
            cout << " Pickup Node: " << pickupNode << "\n";
            cout << " Drop-off Node: " << dropNode << "\n";
            cout << " Distance Travelled: " << pickupToDropDist << "kms\n";
            cout << " Estimated Duration: " << duration << " minutes\n";
            cout << " Fare: " << fare << "\n";
            cout << "--------------------------------------\n";

            // First show updated driver availability
            showDrivers();

            cout << " ------After Ride------\n";
            cout << "\n========== Ride Receipt ==========\n";
            cout << "Driver: " << nearest->getName() << "\n";
            cout << "Vehicle: " << nearest->vehicleType << "\n";
            cout << "Start Location: Node " << pickupNode << "\n";
            cout << "Destination: Node " << dropNode << "\n";
            cout << "Distance: " << pickupToDropDist << " units\n";
            cout << "Duration: " << duration << " minutes\n";
            cout << "Fare Paid: " << fare << "\n";
            cout << "==================================\n";

            // Ask for review
            int rating;
            do {
                cout << "\nWe hope you had a great ride!";
                cout << "\nPlease rate your experience (1 to 5 stars): ";
                cin >> rating;
                if (rating < 1 || rating > 5) {
                    cout << "Invalid input. Please enter a number between 1 and 5.\n";
                }
            } while (rating < 1 || rating > 5);

            cout << "Thank you for rating us " << rating << " star" << (rating > 1 ? "s" : "") << "!\n HAVE A NICE DAY";

        } else {
            cout << "Sorry! No available drivers with a " << vType << " nearby.\n";
        }
    }

    void showDrivers() {
        cout << "\n--- Available Drivers ---\n";
        for (auto& d : drivers) {
            d.displayInfo();
        }
    }

    void showDriverStats() {
        unordered_map<string, int> vehicleCounts;
        int availableCount = 0;

        for (auto& d : drivers) {
            vehicleCounts[d.vehicleType]++;
            if (d.isAvailable()) availableCount++;
        }

        cout << "\n===== Driver Statistics =====\n";
        cout << "Total Drivers: " << drivers.size() << "\n";
        cout << "Available Drivers: " << availableCount << "\n";
        cout << "Vehicle Type Distribution:\n";
        for (auto& pair : vehicleCounts) {
            cout << "  - " << pair.first << ": " << pair.second << "\n";
        }
        cout << "=============================\n";
    }

    void showMap() {
        cout << R"(
--- Sample ASCII City Map ---
         (1)----4----(2)---5---(4)
          |           |         |
          2           5         3
          |           |         |
         (3)----1----(6)---3---(5)
                      \
                       2
                        \
                        (7)---2---(8)
)" << "\n";
    }
};

// Main
int main() {
    cout << "==============================\n";
    cout << "         GoRide                 \n";
    cout << "    Ride Matching System\n";
    cout << "==============================\n" << endl;

    RideManager system;

    // Add roads
    system.addRoad(1, 2, 4);
    system.addRoad(1, 3, 2);
    system.addRoad(2, 4, 5);
    system.addRoad(3, 4, 7);
    system.addRoad(4, 5, 3);
    system.addRoad(3, 6, 1);
    system.addRoad(6, 5, 3);
    system.addRoad(6, 7, 2);
    system.addRoad(7, 8, 2);

    // Register drivers
    system.registerDriver(Driver("Deepak", 101, 2, "Bike"));
    system.registerDriver(Driver("Bunny", 102, 5, "Car"));
    system.registerDriver(Driver("Pranav", 103, 4, "SUV"));
    system.registerDriver(Driver("Sai", 104, 3, "Auto"));
    system.registerDriver(Driver("Kishore", 105, 6, "Bike"));
    system.registerDriver(Driver("Deekshith", 106, 7, "Car"));
    system.registerDriver(Driver("Yashwanth", 107, 8, "Auto"));

    // Rider creation
    string riderName;
    int riderId;
    cout << "Enter Rider Name: ";
    getline(cin, riderName);
    cout << "Enter Rider ID: ";
    cin >> riderId;
    cin.ignore();
    Rider r(riderName, riderId);

    // Show map and drivers
    system.showMap();
    system.showDrivers();
    system.showDriverStats();

    // Book a ride
    int pickupNode, dropNode;
    cout << "\nEnter Rider's Pickup Node (1-8): ";
    cin >> pickupNode;
    cout << "Enter Rider's Drop Node (1-8): ";
    cin >> dropNode;

    system.bookRide(r, pickupNode, dropNode);

    return 0;
}
