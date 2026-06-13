/*
=======================================================
  SMART RIDE ALLOCATION SYSTEM
  ITM Skills University | B.Tech CSE 2025-29 | Sem II
  Subject: Data Structures & Algorithms with C++
=======================================================

  Algorithms used and their purpose:
  ---------------------------------------------------
  1. Insertion Sort  : Add a new driver into the
                       pool in sorted order by ID.

  2. Bubble Sort     : Sort available drivers by
                       score to show a ranked list.

  3. Selection Sort  : Pick the best driver from
                       an unsorted list by repeatedly
                       finding the minimum score.

  4. Binary Search   : Find a driver quickly by ID
                       (works because pool is sorted).

  5. Linear Search   : Find all drivers in a given
                       area (must scan every record).
  ---------------------------------------------------
  Score = distance + (5 - rating) + surgeFactor
  Lower score means a better driver for the rider.
=======================================================
*/

#include <iostream>
#include <string>
#include <cstdlib>   // for rand()
using namespace std;

// -------------------------------------------------------
// Driver record — stores all details of one driver
// -------------------------------------------------------
struct Driver {
    int    id;
    string name;
    float  distance;     // km from rider
    float  rating;       // out of 5.0
    float  surge;        // 1.0 = no surge
    string area;
    bool   available;
};

// -------------------------------------------------------
// Global driver pool — plain array, max 50 drivers
// -------------------------------------------------------
const int MAX = 50;
Driver pool[MAX];
int total = 0;

// -------------------------------------------------------
// Ride History — stores details of every booked ride
// -------------------------------------------------------
struct Ride {
    int    rideID;
    string riderName;
    string driverName;
    string pickup;
    string destination;
};

const int MAX_RIDES = 100;
Ride history[MAX_RIDES];
int totalRides = 0;

// -------------------------------------------------------
// Calculate suitability score for a driver
// Lower score = better match
// -------------------------------------------------------
float getScore(Driver d) {
    return d.distance + (5.0 - d.rating) + d.surge;
}

// -------------------------------------------------------
// Print one driver's details on one line
// -------------------------------------------------------
void printDriver(Driver d) {
    cout << "  ID:"       << d.id
         << "  Name:"     << d.name
         << "  Dist:"     << d.distance << "km"
         << "  Rating:"   << d.rating
         << "  Surge:"    << d.surge
         << "  Area:"     << d.area
         << "  Score:"    << getScore(d)
         << "  Status:"   << (d.available ? "Available" : "Busy")
         << endl;
}

void printLine() {
    cout << "  --------------------------------------------------" << endl;
}

// Convert any string to all lowercase
// So "Andheri", "ANDHERI", "andheri" all become "andheri"
string toLower(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] + 32;
        }
    }
    return s;
}

// =======================================================
// ALGORITHM 1 — INSERTION SORT
//
// Called every time a new driver is added.
// Keeps the pool array sorted by Driver ID.
//
// How it works:
//   Take the last added driver (position i).
//   Compare with drivers to its left.
//   Shift any driver with a bigger ID one step right.
//   Place the new driver in the gap.
//
// This is efficient because the pool is almost sorted
// after each new addition — only one driver is out of place.
// =======================================================
void insertionSort() {
    for (int i = 1; i < total; i++) {
        Driver key = pool[i];
        int j = i - 1;
        while (j >= 0 && pool[j].id > key.id) {
            pool[j + 1] = pool[j];
            j--;
        }
        pool[j + 1] = key;
    }
}

// Add a new driver and sort the pool by ID
void addDriver(int id, string name, float dist,
               float rating, float surge, string area) {
    if (total >= MAX) {
        cout << "  Pool is full!" << endl;
        return;
    }
    // Check for duplicate ID
    for (int i = 0; i < total; i++) {
        if (pool[i].id == id) {
            cout << "  Driver ID " << id << " already exists. Cannot add duplicate." << endl;
            return;
        }
    }
    pool[total].id        = id;
    pool[total].name      = name;
    pool[total].distance  = dist;
    pool[total].rating    = rating;
    pool[total].surge     = surge;
    pool[total].area      = area;
    pool[total].available = true;
    total++;

    insertionSort();   // Keep pool sorted by ID after every addition
}

// =======================================================
// ALGORITHM 2 — BUBBLE SORT
//
// Used to rank available drivers by score BEFORE showing
// the leaderboard to the rider.
//
// How it works:
//   Compare drivers next to each other (j and j+1).
//   If left driver has a worse (higher) score, swap them.
//   After each full pass, the worst driver reaches the end.
//   Repeat for all pairs until no swaps are needed.
// =======================================================
void bubbleSort(Driver arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (getScore(arr[j]) > getScore(arr[j + 1])) {
                Driver temp  = arr[j];
                arr[j]       = arr[j + 1];
                arr[j + 1]   = temp;
            }
        }
    }
}

// =======================================================
// ALGORITHM 3 — SELECTION SORT
//
// Used to find and dispatch the BEST driver.
// Works on a separate unsorted copy of available drivers.
//
// How it works:
//   Divide array into sorted (left) and unsorted (right).
//   In each pass, scan unsorted section for minimum score.
//   Swap that driver to the front of the unsorted section.
//   After all passes, arr[0] is the best driver — dispatch.
//
// Key difference from Bubble Sort:
//   Bubble Sort swaps adjacent pairs many times.
//   Selection Sort finds the minimum FIRST, then swaps ONCE.
//   That means fewer total swaps — better when swaps are costly.
// =======================================================
void selectionSort(Driver arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;   // Assume first unsorted driver is best
        for (int j = i + 1; j < n; j++) {
            if (getScore(arr[j]) < getScore(arr[minIdx])) {
                minIdx = j;   // Found a better driver
            }
        }
        // Swap the best found driver into position i
        Driver temp   = arr[i];
        arr[i]        = arr[minIdx];
        arr[minIdx]   = temp;
    }
    // arr[0] is now the best driver
}

// =======================================================
// ALGORITHM 4 — BINARY SEARCH
//
// Find a driver by their ID.
// Works because the pool is always sorted by ID
// (maintained by Insertion Sort).
//
// How it works:
//   Look at the middle of the current range.
//   If it matches, return it.
//   If ID we want is bigger, search right half.
//   If ID we want is smaller, search left half.
//   Repeat until found or range is empty.
// =======================================================
int binarySearch(int targetID) {
    int low  = 0;
    int high = total - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (pool[mid].id == targetID) {
            return mid;         // Found
        } else if (pool[mid].id < targetID) {
            low = mid + 1;      // Search right half
        } else {
            high = mid - 1;     // Search left half
        }
    }
    return -1;  // Not found
}

// =======================================================
// ALGORITHM 5 — LINEAR SEARCH
//
// Find all available drivers in a given area.
// Cannot use Binary Search here because the pool is
// sorted by ID, not by area.
// Must check every single record one by one.
// =======================================================
void linearSearch(string targetArea) {
    cout << "  [Linear Search] Drivers available in " << targetArea << ":" << endl;
    printLine();
    bool found = false;
    for (int i = 0; i < total; i++) {
        if (toLower(pool[i].area) == toLower(targetArea) && pool[i].available) {
            printDriver(pool[i]);
            found = true;
        }
    }
    if (!found) {
        cout << "  No available drivers found in " << targetArea << endl;
    }
    printLine();
}

// =======================================================
// RIDE MATCHING — uses all 5 algorithms together
// =======================================================
void requestRide(string riderName, string riderArea, string destination) {
    printLine();
    cout << "  Ride request by " << riderName
         << " in area: " << riderArea << endl;
    printLine();

    // Collect available drivers in the rider's area
    Driver nearby[MAX];
    int count = 0;
    for (int i = 0; i < total; i++) {
        if (pool[i].available && toLower(pool[i].area) == toLower(riderArea)) {
            nearby[count] = pool[i];
            count++;
        }
    }

    if (count == 0) {
        cout << "  No drivers available in " << riderArea << " right now." << endl;
        printLine();
        return;
    }

    // --- BUBBLE SORT: rank drivers and show leaderboard ----
    // Make a copy for Bubble Sort (so Selection Sort gets
    // its own fresh unsorted copy separately)
    Driver forBubble[MAX];
    Driver forSelection[MAX];
    for (int i = 0; i < count; i++) {
        forBubble[i]    = nearby[i];
        forSelection[i] = nearby[i];
    }

    bubbleSort(forBubble, count);

    cout << "\n  [Bubble Sort] Ranked driver leaderboard:" << endl;
    printLine();
    for (int i = 0; i < count; i++) {
        cout << "  Rank " << i + 1 << ": ";
        printDriver(forBubble[i]);
    }
    printLine();

    // --- SELECTION SORT: pick best driver for dispatch -----
    // Works on its own unsorted copy independently
    cout << "\n  [Selection Sort] Selecting best driver for dispatch..." << endl;
    cout << "  Unsorted input:" << endl;
    for (int i = 0; i < count; i++) printDriver(forSelection[i]);

    selectionSort(forSelection, count);

    cout << "  After Selection Sort:" << endl;
    for (int i = 0; i < count; i++) printDriver(forSelection[i]);

    Driver best = forSelection[0];
    cout << "\n  Best driver selected: " << best.name
         << " (Score: " << getScore(best) << ")" << endl;

    // --- BINARY SEARCH: mark selected driver as Busy -------
    int idx = binarySearch(best.id);
    if (idx != -1) {
        pool[idx].available = false;
        cout << "  [Binary Search] Driver ID " << best.id
             << " found at index " << idx << " — marked Busy." << endl;
    }

    // Save to ride history
    if (totalRides < MAX_RIDES) {
        history[totalRides].rideID      = totalRides + 1;
        history[totalRides].riderName   = riderName;
        history[totalRides].driverName  = best.name;
        history[totalRides].pickup      = best.area;
        history[totalRides].destination = destination;
        totalRides++;
    }

    printLine();
    cout << "  DRIVER BOOKED" << endl;
    printLine();
    cout << "  Driver ID   : " << best.id << endl;
    cout << "  Name        : " << best.name << endl;
    cout << "  Distance    : " << best.distance << " km" << endl;
    cout << "  Rating      : " << best.rating << " / 5.0" << endl;
    cout << "  Surge       : " << best.surge << "x" << endl;
    cout << "  Pickup      : " << best.area << endl;
    cout << "  Destination : " << destination << endl;
    cout << "  Score       : " << getScore(best) << endl;
    cout << "  Status      : Busy (on the way to " << riderName << ")" << endl;
    printLine();
}

// =======================================================
// MENU OPTIONS
// =======================================================
void showAllDrivers() {
    printLine();
    cout << "  All Drivers (sorted by ID):" << endl;
    printLine();
    for (int i = 0; i < total; i++) printDriver(pool[i]);
    printLine();
    cout << "  SYSTEM SUMMARY" << endl;
    printLine();
    cout << "  Total Drivers Loaded  : " << total << endl;
    cout << "  Total Areas Covered   : 5" << endl;
    cout << "  Areas                 : Andheri, Bandra, Dadar, Powai, Thane" << endl;
    printLine();
}

void menuAddDriver() {
    int id;
    string name, area;
    float dist, rating, surge;

    cout << "  Enter Driver ID     : "; cin >> id;
    cin.ignore();
    cout << "  Enter Name          : "; getline(cin, name);
    cout << "  Enter Distance (km) : "; cin >> dist;
    cout << "  Enter Rating (1-5)  : "; cin >> rating;
    while (rating < 1.0 || rating > 5.0) {
        cout << "  Invalid. Enter rating between 1.0 and 5.0: "; cin >> rating;
    }
    cout << "  Enter Surge (1.0+)  : "; cin >> surge;
    while (surge < 1.0) {
        cout << "  Invalid. Surge must be 1.0 or higher: "; cin >> surge;
    }
    cin.ignore();
    cout << "  Enter Area          : "; getline(cin, area);

    addDriver(id, name, dist, rating, surge, area);
    cout << "  [Insertion Sort] Driver " << name << " added. Pool sorted by ID." << endl;
}

void menuLookupDriver() {
    int id;
    cout << "  Enter Driver ID to search: "; cin >> id;
    int idx = binarySearch(id);
    if (idx == -1) {
        cout << "  Driver ID " << id << " not found." << endl;
    } else {
        cout << "  [Binary Search] Found:" << endl;
        printDriver(pool[idx]);
    }
}

void menuAreaSearch() {
    string area;
    cin.ignore();
    cout << "  Enter area name: "; getline(cin, area);
    linearSearch(area);
}

void menuRequestRide() {
    string name, area, destination;
    cin.ignore();
    cout << "  Enter your name        : "; getline(cin, name);
    cout << "  Enter your pickup area : "; getline(cin, area);
    cout << "  Enter your destination : "; getline(cin, destination);
    requestRide(name, area, destination);
}

// =======================================================
// SIMULATE LIVE LOCATION UPDATE
//
// Drivers are always moving, so their distance from the
// rider changes continuously. This function randomly updates
// every driver's distance (between 0.5 and 5.0 km) to
// simulate that movement, then re-sorts using Insertion Sort
// so the pool stays organised after the changes.
// =======================================================
void updateDriverLocations() {
    cout << "  [Live Update] Updating driver distances..." << endl;
    printLine();

    for (int i = 0; i < total; i++) {
        float oldDist = pool[i].distance;
        // Generate random distance between 0.5 and 5.0 km
        // rand() % 46 gives 0-45, divide by 10.0 gives 0.0-4.5, add 0.5
        pool[i].distance = (rand() % 46) / 10.0 + 0.5;
        cout << "  " << pool[i].name
             << "  :  " << oldDist << " km  ->  " << pool[i].distance << " km" << endl;
    }

    printLine();
    cout << "  Driver locations and scores have been updated." << endl;
}

void showRideHistory() {
    printLine();
    cout << "  RIDE HISTORY" << endl;
    printLine();
    if (totalRides == 0) {
        cout << "  No rides booked yet." << endl;
    } else {
        for (int i = 0; i < totalRides; i++) {
            cout << "  Ride ID     : " << history[i].rideID << endl;
            cout << "  Rider       : " << history[i].riderName << endl;
            cout << "  Driver      : " << history[i].driverName << endl;
            cout << "  Pickup      : " << history[i].pickup << endl;
            cout << "  Destination : " << history[i].destination << endl;
            printLine();
        }
    }
}

void menuMarkAvailable() {
    int id;
    cout << "  Enter Driver ID to mark Available: "; cin >> id;
    int idx = binarySearch(id);
    if (idx == -1) {
        cout << "  Driver not found." << endl;
    } else {
        if (pool[idx].available == true) {
            cout << "  " << pool[idx].name << " is already Available." << endl;
        } else {
            pool[idx].available = true;
            cout << "  " << pool[idx].name << " is now Available." << endl;
        }
    }
}

// =======================================================
// SAMPLE DATA — IDs intentionally out of order to
// demonstrate Insertion Sort placing them correctly
// =======================================================
void loadSampleData() {
    // IDs are intentionally out of order to demonstrate Insertion Sort

    // Andheri
    addDriver(104, "Amit Sharma",   2.5, 4.8, 1.0, "Andheri");
    addDriver(101, "Ravi Kumar",    1.2, 4.2, 1.5, "Andheri");
    addDriver(113, "Deepak Singh",  0.8, 3.9, 2.0, "Andheri");

    // Bandra
    addDriver(107, "Suresh Nair",   3.0, 4.9, 1.0, "Bandra");
    addDriver(102, "Priya Menon",   1.5, 4.7, 1.2, "Bandra");
    addDriver(111, "Arjun Desai",   2.0, 4.3, 1.0, "Bandra");

    // Dadar
    addDriver(103, "Kiran Patil",   4.0, 4.5, 1.0, "Dadar");
    addDriver(109, "Neha Joshi",    2.0, 4.6, 1.3, "Dadar");
    addDriver(115, "Sameer Khan",   1.8, 4.4, 1.1, "Dadar");

    // Powai
    addDriver(106, "Rohit Verma",   3.5, 4.7, 1.0, "Powai");
    addDriver(110, "Sneha Iyer",    1.0, 4.9, 1.0, "Powai");
    addDriver(114, "Manish Tiwari", 2.2, 4.1, 1.5, "Powai");

    // Thane
    addDriver(105, "Pooja Rao",     1.3, 4.6, 1.2, "Thane");
    addDriver(108, "Vikram Joshi",  2.8, 4.3, 1.0, "Thane");
    addDriver(112, "Ankita Nair",   0.9, 4.8, 1.3, "Thane");


}

// =======================================================
// MAIN
// =======================================================
int main() {

    // --- Welcome Screen ---
    cout << "\n  ================================================" << endl;
    cout << "    SMART RIDE ALLOCATION SYSTEM" << endl;
    cout << "    ITM Skills University | B.Tech CSE 2025-29" << endl;
    cout << "  ================================================" << endl;
    cout << "\n  1. Start" << endl;
    cout << "  0. Exit" << endl;
    cout << "\n  Enter choice: ";

    int launch;
    cin >> launch;

    if (launch == 0) {
        cout << "\n  Goodbye!" << endl;
        return 0;
    }

    // --- Load data silently ---
    cout << endl;
    cout << "  Loading drivers into the system..." << endl;
    loadSampleData();
    cout << endl;
    printLine();
    cout << "  SYSTEM SUMMARY" << endl;
    printLine();
    cout << "  Total Drivers Loaded  : " << total << endl;
    cout << "  Total Areas Covered   : 5" << endl;
    cout << "  Areas                 : Andheri, Bandra, Dadar, Powai, Thane" << endl;
    printLine();
    cout << endl;

    // --- Main Menu ---
    int choice;
    do {
        cout << "\n  -------- MENU --------" << endl;
        cout << "  1. View all drivers" << endl;
        cout << "  2. Add new driver          [Insertion Sort]" << endl;
        cout << "  3. Request a ride          [Bubble Sort + Selection Sort]" << endl;
        cout << "  4. Lookup driver by ID     [Binary Search]" << endl;
        cout << "  5. Search drivers by area  [Linear Search]" << endl;
        cout << "  6. Mark driver as available" << endl;
        cout << "  7. Refresh live driver locations" << endl;
        cout << "  8. View ride history" << endl;
        cout << "  0. Exit" << endl;
        cout << "  Enter choice: ";
        cin >> choice;
        cout << endl;

        if      (choice == 1) showAllDrivers();
        else if (choice == 2) menuAddDriver();
        else if (choice == 3) menuRequestRide();
        else if (choice == 4) menuLookupDriver();
        else if (choice == 5) menuAreaSearch();
        else if (choice == 6) menuMarkAvailable();
        else if (choice == 7) updateDriverLocations();
        else if (choice == 8) showRideHistory();
        else if (choice == 0) cout << "  Goodbye!" << endl;
        else                  cout << "  Invalid choice." << endl;

    } while (choice != 0);

    return 0;
}
