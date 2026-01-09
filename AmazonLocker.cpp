/*
"Design a locker system like Amazon Locker where delivery drivers can deposit packages and 
customers can pick them up using a code."

Requirements:
1. Carrier deposits a package by specifying size (small, medium, large)
   - System assigns an available compartment of matching size
   - Returns compartment number and access token, or error if no space
2. Upon successful deposit, an access token is generated and returned
   - One access token per package
3. User retrieves package by entering access token
   - System validates code and returns compartment number
   - Throws specific error if code is invalid or expired
4. Access tokens expire after 7 days
   - Expired codes are rejected if used for pickup
   - Package remains in compartment until staff removes it
5. Staff can remove expired packages to free up compartments
   - Returns the package to sender and makes compartment available
6. Invalid access tokens are rejected with clear error messages
   - Wrong code, already used, or expired - user gets specific feedback

*/

#include <chrono>
#include <vector>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;

enum Size {
    SMALL,
    MEDIUM,
    LARGE
};

class Compartment{
private:
    string id;
    Size size;
    bool occupied;


public:
    Compartment(string id, Size size) : id(id), size(size), occupied(false) {}

    bool isOccupied(){
        return occupied;
    }

    void setOccupied(){
        occupied = true;
    }

    void setEmpty(){
        occupied = false;
    }

    string getId(){
        return id;
    }

    Size getSize(){
        return size;
    }


};

class AccessToken{
private:
    string code;
    system_clock::time_point expiryTime;
    Compartment* c;

public:
    AccessToken(string code, system_clock::time_point expiry, Compartment* c) : code(code), expiryTime(expiry), c(c) {}

    system_clock::time_point getExpiry(){
        return expiryTime;
    }

    string getCode(){
        return code;
    }

    Compartment* getCompartment(){
        return c;
    }

};


class LockerSystem{
private:
    vector<Compartment*> compartments;
    unordered_map<string, AccessToken*> tokensMap;
    mt19937 random;

    AccessToken* getAccessToken(Compartment* c){
        uniform_int_distribution<int> dist(1000000, 1999999);
        int num = 100000 + (rand() % 900000); 
        string code = to_string(dist(random)).substr(1);
        system_clock::time_point expiry = system_clock::now() + hours(7 * 24);
        return new AccessToken(code, expiry, c);
    }
      
public:
    LockerSystem() : random(random_device{}()) {} 

    ~LockerSystem() {
        for(auto c : compartments) delete c;
        for(auto pair : tokensMap) delete pair.second;
    }

    void addCompartment(string id, Size s){
        Compartment* c = new Compartment(id, s);
        compartments.push_back(c);
    }

    string depositPackage(Size size){
        for(auto&c : compartments){
            if(c->getSize() == size && !c->isOccupied()){
                AccessToken* token = getAccessToken(c);
                c->setOccupied();
                tokensMap[token->getCode()] = token;
                return token->getCode();
            }
        }
        throw runtime_error("No compartment available for this size");
    }

    string getPackage(string code) {
        if(tokensMap.find(code)==tokensMap.end()){
            throw invalid_argument("Invalid Code provided");
        }
        AccessToken* t = tokensMap[code];
        auto now = system_clock::now();
        if(t->getExpiry() < now){
            throw invalid_argument("Code Expired!");
        }
        Compartment* c = t->getCompartment();
        c->setEmpty();
        string compId = c->getId();
        delete t; 
        tokensMap.erase(code);
        
        return compId;
    }

    string removeExpiredPackage(string code){
        if(tokensMap.find(code)==tokensMap.end()){
            throw invalid_argument("Invalid Code provided");
        }
        AccessToken* t = tokensMap[code];
        auto now = system_clock::now();
        if(t->getExpiry() > now){
            throw invalid_argument("Expiry time not passed");
        }
        Compartment* c = t->getCompartment();
        c->setEmpty();
        tokensMap.erase(code);
        return c->getId();
    }

};

int main() {
    try {
        LockerSystem locker;
        locker.addCompartment("A1", SMALL);

        // Deposit
        string code = locker.depositPackage(SMALL);
        cout << "Package deposited. Code: " << code << endl;

        // Retrieve
        string lockerId = locker.getPackage(code);
        cout << "Package retrieved from: " << lockerId << endl;

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}

