#include <iostream>
#include <string>

using namespace std;

#define MAX_SLOTS 3     
#define MAX_QUEUE 5     

// Unit 2: Doubly Linked List Node Upgrade
struct Vehicle {
    int slotId;
    string licensePlate;
    string vehicleType;
    Vehicle* prev;  // NEW: Pointer to previous node
    Vehicle* next;
};

// Unit 2: Circular Array-Based Queue
class WaitingQueue {
private:
    string queueArray[MAX_QUEUE];
    int front;
    int rear;
    int count;

public:
    WaitingQueue() {
        front = -1;
        rear = -1;
        count = 0;
    }

    bool isFull() { return count == MAX_QUEUE; }
    bool isEmpty() { return count == 0; }

    void enqueue(string plate) {
        if (isFull()) {
            cout << ">> Waiting queue is FULL! Vehicle " << plate << " cannot wait.\n";
            return;
        }
        if (front == -1) front = 0;
        rear = (rear + 1) % MAX_QUEUE;
        queueArray[rear] = plate;
        count++;
        cout << ">> Parking FULL! Vehicle " << plate << " added to Waiting Queue.\n";
    }

    string dequeue() {
        if (isEmpty()) return "";
        string plate = queueArray[front];
        if (front == rear) { 
            front = -1;
            rear = -1;
        } else {
            front = (front + 1) % MAX_QUEUE;
        }
        count--;
        return plate;
    }

    void displayQueue() {
        if (isEmpty()) {
            cout << ">> Waiting Queue: [ EMPTY ]\n";
            return;
        }
        cout << ">> Waiting Queue: ";
        int i = front;
        for (int c = 0; c < count; c++) {
            cout << "[" << queueArray[i] << "] ";
            i = (i + 1) % MAX_QUEUE;
        }
        cout << "\n";
    }
};

class ParkingSystem {
private:
    Vehicle* head;        
    WaitingQueue wQueue;  
    int occupiedSlots;

public:
    ParkingSystem() {
        head = NULL;
        occupiedSlots = 0;
    }

    int getFreeSlotId() {
        for (int slot = 1; slot <= MAX_SLOTS; slot++) { 
            bool found = false;
            Vehicle* temp = head;
            while (temp != NULL) {
                if (temp->slotId == slot) {
                    found = true;
                    break;
                }
                temp = temp->next;
            }
            if (!found) return slot;
        }
        return -1;
    }

    // Insert into Doubly Linked List
    void parkVehicle(string plate, string type) {
        if (occupiedSlots >= MAX_SLOTS) {
            wQueue.enqueue(plate);
            return;
        }

        int slot = getFreeSlotId();
        Vehicle* newVehicle = new Vehicle();
        newVehicle->slotId = slot;
        newVehicle->licensePlate = plate;
        newVehicle->vehicleType = type;
        newVehicle->next = NULL;
        newVehicle->prev = NULL; 

        if (head == NULL) {
            head = newVehicle;
        } else {
            Vehicle* temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newVehicle;
            newVehicle->prev = temp; // Set backward link
        }

        occupiedSlots++;
        cout << ">> SUCCESS: Vehicle " << plate << " parked at Slot " << slot << ".\n";
    }

    // O(1) Deletion logic using Doubly Linked List pointers
    void removeVehicle(string plate) {
        if (head == NULL) {
            cout << ">> ERROR: Parking lot is empty!\n";
            return;
        }

        Vehicle* temp = head;

        // Sequential search to find the specific plate
        while (temp != NULL && temp->licensePlate != plate) {
            temp = temp->next;
        }

        if (temp == NULL) {
            cout << ">> ERROR: Vehicle " << plate << " not found.\n";
            return;
        }

        // DLL Node Deletion (No 'prev' traversal tracking required)
        if (temp == head) {
            head = head->next;
            if (head != NULL) head->prev = NULL;
        } else {
            temp->prev->next = temp->next;
            if (temp->next != NULL) {
                temp->next->prev = temp->prev;
            }
        }

        cout << ">> SUCCESS: Vehicle " << plate << " vacated Slot " << temp->slotId << ".\n";
        delete temp;
        occupiedSlots--;

        if (!wQueue.isEmpty()) {
            string waitingPlate = wQueue.dequeue();
            cout << ">> Moving vehicle " << waitingPlate << " from Waiting Queue to Parking...\n";
            parkVehicle(waitingPlate, "Car/Bike");
        }
    }

    // Unit 3: In-Place Bubble Sort by Slot ID (Required for Binary Search)
    void sortBySlotNumber(bool silent = false) {
        if (head == NULL || head->next == NULL) {
            if (!silent) cout << ">> Not enough vehicles to sort.\n";
            return;
        }

        bool swapped;
        Vehicle* ptr1;
        Vehicle* lptr = NULL; 

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                if (ptr1->slotId > ptr1->next->slotId) {
                    swap(ptr1->slotId, ptr1->next->slotId);
                    swap(ptr1->licensePlate, ptr1->next->licensePlate);
                    swap(ptr1->vehicleType, ptr1->next->vehicleType);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1; 
        } while (swapped);

        if (!silent) {
            cout << "\n--- Vehicles Sorted by Slot Number (In-Place Bubble Sort) ---\n";
            displayParkedVehicles();
        }
    }

    // Unit 3: Binary Search by Slot ID
    void binarySearchBySlotId(int targetSlot) {
        if (occupiedSlots == 0) {
            cout << ">> ERROR: Parking lot is empty!\n";
            return;
        }

        // 1. Enforce sorting first (Prerequisite for Binary Search)
        sortBySlotNumber(true); 

        // 2. Map DLL to array for O(1) index access
        Vehicle* arr[MAX_SLOTS];
        Vehicle* temp = head;
        int index = 0;
        while (temp != NULL) {
            arr[index++] = temp;
            temp = temp->next;
        }

        // 3. Execute Binary Search algorithm
        int low = 0;
        int high = occupiedSlots - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (arr[mid]->slotId == targetSlot) {
                cout << ">> FOUND via BINARY SEARCH: Slot " << targetSlot 
                     << " is occupied by Vehicle " << arr[mid]->licensePlate 
                     << " (Type: " << arr[mid]->vehicleType << ").\n";
                return;
            }
            
            if (arr[mid]->slotId < targetSlot) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        
        cout << ">> NOT FOUND: Slot " << targetSlot << " is currently empty.\n";
    }

    void displayParkedVehicles() {
        if (head == NULL) {
            cout << ">> Parking Lot: [ EMPTY ]\n";
            return;
        }

        cout << "\n--- Currently Parked Vehicles (" << occupiedSlots << "/" << MAX_SLOTS << ") ---\n";
        Vehicle* temp = head;
        while (temp != NULL) {
            cout << "Slot " << temp->slotId << " | License: " << temp->licensePlate 
                 << " | Type: " << temp->vehicleType << "\n";
            temp = temp->next;
        }
        cout << "-------------------------------------------\n";
    }

    void displayWaitingQueue() {
        wQueue.displayQueue();
    }
};

int main() {
    ParkingSystem parking;
    int choice;
    string plate, type;
    int targetSlot;

    while (true) {
        cout << "\n========== PARKING MANAGEMENT SYSTEM ==========\n";
        cout << "1. Park Vehicle (Insert)\n";
        cout << "2. Remove Vehicle (O(1) DLL Delete)\n";
        cout << "3. Binary Search by Slot ID (O(log N))\n";
        cout << "4. Display All Parked Vehicles\n";
        cout << "5. Display Waiting Queue\n";
        cout << "6. Sort Vehicles by Slot Number (Bubble Sort)\n";
        cout << "7. Exit\n";
        cout << "Enter your choice (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Vehicle Number Plate: ";
                cin >> plate;
                cout << "Enter Vehicle Type (Car/Bike): ";
                cin >> type;
                parking.parkVehicle(plate, type);
                break;
            case 2:
                cout << "Enter Vehicle Number Plate to exit: ";
                cin >> plate;
                parking.removeVehicle(plate);
                break;
            case 3:
                cout << "Enter Slot ID to search (1-" << MAX_SLOTS << "): ";
                cin >> targetSlot;
                parking.binarySearchBySlotId(targetSlot);
                break;
            case 4:
                parking.displayParkedVehicles();
                break;
            case 5:
                parking.displayWaitingQueue();
                break;
            case 6:
                parking.sortBySlotNumber();
                break;
            case 7:
                cout << "Exiting Parking Management System. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice! Please choose between 1 and 7.\n";
        }
    }
    return 0;
}