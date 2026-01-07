#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;


enum VehicleType { MOTORCYCLE, CAR, BUS };
enum SpotType { SMALL, MEDIUM, LARGE };


class PricingStrategy {
public:
    virtual double calculate(int hours) = 0;
};

class HourlyStrategy : public PricingStrategy {
    double rate;
public:
    HourlyStrategy(double r) : rate(r) {}
    double calculate(int hours) override { 
        if(hours == 0) hours = 1;
        return hours * rate; 
    }
};


class Vehicle {
private:
    string licensePlate;
    VehicleType type;
    PricingStrategy* strategy;

public:
    Vehicle(string licensePlate, VehicleType type, PricingStrategy* strategy) 
        : licensePlate(licensePlate), type(type), strategy(strategy) {}

    VehicleType getType() { return type; }
    PricingStrategy* getStrategy() { return strategy; }
    string getPlate() { return licensePlate; }
};


class ParkingSpot {
private:
    SpotType type;
    bool isFree;

public:
    ParkingSpot(SpotType type) : type(type), isFree(true) {}

    bool isAvailable() { return isFree; }

    bool canFit(Vehicle* v) {
        if (!isFree) return false;
        VehicleType vType = v->getType();
        
        if (vType == BUS) return (type == LARGE);
        if (vType == CAR) return (type == MEDIUM || type == LARGE);
        return true; 
    }

    void park() { isFree = false; }
    void removeVehicle() { isFree = true; }
};

class Ticket {
private:
    system_clock::time_point entryTime;
    int spotIndex;

public:
    Ticket(int index) : spotIndex(index) {
        entryTime = system_clock::now();
    }

    int getSpotIndex() { return spotIndex; }

    int getDurationHours() {
        auto now = system_clock::now();
        auto duration = duration_cast<seconds>(now - entryTime).count();
        return (int)duration; 
    }
};


class ParkingLot {
private:
    vector<ParkingSpot> spots; 

public:
    ParkingLot() {
        for(int i=0; i<5; i++) spots.push_back(ParkingSpot(MEDIUM));
    }

    Ticket* parkVehicle(Vehicle* v) {
        for (int i = 0; i < spots.size(); i++) {
            if (spots[i].isAvailable() && spots[i].canFit(v)) {
                
                spots[i].park(); // Mark occupied
                cout << "Parked " << v->getPlate() << " at spot " << i << endl;
                
                return new Ticket(i); // Save spot ID in ticket
            }
        }
        cout << "No spot found for " << v->getPlate() << endl;
        return nullptr;
    }

    void exitVehicle(Ticket* ticket, Vehicle* v) {
        int spotIdx = ticket->getSpotIndex();
        int hours = ticket->getDurationHours();
        
        double price = v->getStrategy()->calculate(hours);
        cout << "Vehicle " << v->getPlate() << " exiting. Duration: " << hours << " hrs. Bill: $" << price << endl;

        spots[spotIdx].removeVehicle();
        
        delete ticket;
    }
};


int main() {
    ParkingLot lot;
    HourlyStrategy normalRate(2.0); // $2/hr

    Vehicle car1("ABC-123", CAR, &normalRate);
    Vehicle car2("XYZ-999", CAR, &normalRate);

    // Park
    Ticket* t1 = lot.parkVehicle(&car1);
    
    // Simulate time passing
    this_thread::sleep_for(seconds(2)); 

    // Exit
    if (t1) lot.exitVehicle(t1, &car1);

    return 0;
}