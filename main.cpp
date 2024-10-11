#include <iostream>
#include <cstring>
#include <cctype>
#include <fstream>
#include <string>
#include<iomanip>
#include <sstream>
#include "sha256.h"
#include <ctime>

using namespace std;

struct date {
    string startDate;
    string endDate;
};

struct Car {
    string plateNumber;
    string brand;
    string model;
    int year;
    string color;
    double pricePerDay;
    date d;
};

struct Client {
    int usertID;
    string firstName;
    string lastName;
    string password;
    string phone;
    string email;
    string role;
    int nbReservation;
    Car* c=NULL;
};

const int maxRows = 100;
const int maxCols = 9;

//prototypes
int getNextUserID();
Client* createClientArray(const string&, int&);
bool authenticateClient(Client*, int, int, const string&);
void AccountInquiries(string&);
Client createAccount();
void write_client_to_csv(Client, const string&);
void read_client_from_csv(Client*, int, const string&);
void welcome(int&);
void adminPage();
void clientPage(Client*, int, Car*, Car*, int);
void read_cars_from_csv(Car*, int, const string&);
int getCarNumber();
void read_rented_cars_from_csv(Client* clients, int size, const string& filename);
int stringToInt(string time);



// Automatically generated ID
int getNextUserID() {
    ifstream file("users.csv");
    string line;
    int count = 0;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        ++count;
    }
    file.close();
    if (count == 0) {
        return 1;
    }
    return (count + 1);
}

//Get the number of cars in the store
int getCarNumber() {
    ifstream file("cars.csv");
    string line;
    int count = 0;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        ++count;
    }
    file.close();
    return count;
}

//Get the number of cars rented to dynamically allocate the array 
int getRentedCarNumber() {
    ifstream file("rentedcars.csv");
    string line;
    int count = 0;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        ++count;
    }
    file.close();
    return count;
}


// Function to create a dynamically allocated array of Client structures
Client* createClientArray(const string& filename, int& size) {
    Client* clients = new Client[size];
    read_client_from_csv(clients, size, filename); //the read function is adding all the information from the file inside the array 
    return clients;
}

//Function to create a dynamically allocated array of cars
Car* createCarsArray(const string& filename, int& size) {
    Car* cars = new Car[size];
    read_cars_from_csv(cars, size, filename);
    return cars;
}

Client* createRentedCarsArray(const string& filename, int size) {
    Client* rentedCars = new Client[size];
    read_rented_cars_from_csv(rentedCars, size, filename); //the read function is adding all the information from the file inside the array 
    return rentedCars;
}

void createPDFarray(Client PDFarray[][maxCols], Client* clients, int sizeClients, Car* cars, int sizeCars, Client* rentedCars, int sizeRentedCars) {
    for (int i = 0; i < sizeRentedCars; i++) {
        int ID = rentedCars[i].usertID;

       
        PDFarray[i][0].usertID = ID;
        for (int j = 0; j < sizeClients; j++) {
            if (ID == clients[j].usertID) {
                PDFarray[i][1].firstName = clients[j].firstName;
                PDFarray[i][2].lastName = clients[j].lastName;
                PDFarray[i][3].phone = clients[j].phone;
                PDFarray[i][4].email = clients[j].email;
            }
        }

        
        PDFarray[i][5].c = new Car;
        PDFarray[i][6].c = new Car;
        PDFarray[i][7].c = new Car;
        PDFarray[i][8].c = new Car;

        
        PDFarray[i][5].c->plateNumber = rentedCars[i].c->plateNumber;
        PDFarray[i][6].c->d.startDate = rentedCars[i].c->d.startDate;
        PDFarray[i][7].c->d.endDate = rentedCars[i].c->d.endDate;

       
        string plateNumber = PDFarray[i][5].c->plateNumber;
        for (int j = 0; j < sizeCars; j++) {
            if (cars[j].plateNumber == plateNumber) {
                PDFarray[i][8].c->pricePerDay = cars[j].pricePerDay;
                break;
            }
        }
    }
}

// Sorting function for a static array
void sortPDFarray(Client PDFarray[][maxCols], int sizeRentedCars) {
    for (int i = 0; i < sizeRentedCars - 1; ++i) {
        for (int j = 0; j < sizeRentedCars - i - 1; ++j) {

            // Compare pricePerDay of two consecutive rows
            double pricePerDay1 = PDFarray[j][8].c->pricePerDay;
            double pricePerDay2 = PDFarray[j + 1][8].c->pricePerDay;

            // Swap rows if necessary (ascending order)
            if (pricePerDay1 > pricePerDay2) {
                // Swap client details
                Client tempClient = PDFarray[j][0];
                PDFarray[j][0] = PDFarray[j + 1][0];
                PDFarray[j + 1][0] = tempClient;

                // Swap car details
                Car* tempCar = PDFarray[j][5].c;
                PDFarray[j][5].c = PDFarray[j + 1][5].c;
                PDFarray[j + 1][5].c = tempCar;

                // Swap pricePerDay
                double tempPrice = pricePerDay1;
                PDFarray[j][8].c->pricePerDay = pricePerDay2;
                PDFarray[j + 1][8].c->pricePerDay = tempPrice;
            }
        }
    }
}


void displayPDFarray(Client PDFarray[][maxCols], int sizeRentedCars) {

    cout << "UserID,First Name, Last Name, Phone, Email, Plate Number, Start Date, End Date, Price Per Day" << endl;

    for (int i = 0; i < sizeRentedCars; ++i) {
        cout << PDFarray[i][0].usertID << "   ";
        cout << PDFarray[i][1].firstName << "   ";
        cout << PDFarray[i][2].lastName << "   ";
        cout << PDFarray[i][3].phone << "   ";
        cout << PDFarray[i][4].email << "   ";
        cout << PDFarray[i][5].c->plateNumber << "   ";
        cout << PDFarray[i][6].c->d.startDate << "   ";
        cout << PDFarray[i][7].c->d.endDate << "   ";
        cout << PDFarray[i][8].c->pricePerDay << endl;
    }

}

void writeCSV(string PDFarray[][maxCols], int size, const string& filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    // Write header
    outputFile << "UserID,FirstName,LastName,Phone,Email,PlateNumber,StartDate,EndDate,PricePerDay($)" << endl;

    // Write data
    for (int i = 0; i < size; ++i) {
        outputFile << PDFarray[i][0]<< ",";
        outputFile << PDFarray[i][1] << ",";
        outputFile << PDFarray[i][2] << ",";
        outputFile << PDFarray[i][3]<< ",";
        outputFile << PDFarray[i][4] << ",";
        outputFile << PDFarray[i][5]<< ",";
        outputFile << PDFarray[i][6]<< ",";
        outputFile << PDFarray[i][7] << ",";
        outputFile << PDFarray[i][8] << endl;
    }

    outputFile.close();
}

int stringToInt1(string time) {



    stringstream ss(time);



    int date = stoi(time);


    return date;
}

void transformArray(string PDFarray[][maxCols], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 9; j++) {
            if (j == 1 || j == 8) {
                stringToInt1(PDFarray[i][j]);
            }
        }
         }
}

// Authentication of an account (checking)
bool authenticateClient(Client* clients, int size, int userID, const string& password) {
    for (int i = 0; i < size; ++i) {
        if (clients[i].usertID == userID && sha256(password) == clients[i].password) {
            return true;
        }
    }
    return false;
}

// Function to check if the car already exists in the file 
bool authenticatePlateNumber(Car* cars, int size, string plateNumber) {
    for (int i = 0; i < size; i++)
        if (cars[i].plateNumber == plateNumber)
            return true;

    return false;
}

bool checkCarAvailability(Client* rentedCars, int rentedSize, std::string plateNumber, int startDate, int endDate) {
    bool is_available = true;
    for (int i = 0; i < rentedSize; i++) {
        if (rentedCars[i].c->plateNumber == plateNumber) {
            int rented_start_date = stringToInt(rentedCars[i].c->d.startDate);
            int rented_end_date = stringToInt(rentedCars[i].c->d.endDate);

            if ((startDate >= rented_start_date && startDate <= rented_end_date) ||
                (endDate >= rented_start_date && endDate <= rented_end_date) ||
                (startDate <= rented_start_date && endDate >= rented_end_date)) {
                is_available = false; // Car is not available for rental
            }
        }
    }
    return is_available;
}
// Display array
void displayClients(Client* clients, int size) {
    cout << "Clients Information:" << endl;
    cout << "---------------------" << endl;
    for (int i = 0; i < size; ++i) {
        cout << "User ID: " << clients[i].usertID << endl;
        cout << "First Name: " << clients[i].firstName << endl;
        cout << "Last Name: " << clients[i].lastName << endl;
        cout << "Phone: " << clients[i].phone << endl;
        cout << "Email: " << clients[i].email << endl;
        cout << "Number of Reservations: " << clients[i].nbReservation << endl;
        cout << "Role: " << clients[i].role << endl;
        cout << "---------------------" << endl;
    }
}

void displayCars(Car* cars, int size) {
    cout << "Available cars:" << endl;
    cout << "---------------------" << endl;
    for (int i = 0; i < size; i++) {
        cout << "Plate Number: " << cars[i].plateNumber << endl;
        cout << "Brand: " << cars[i].brand << endl;
        cout << "Model: " << cars[i].model << endl;
        cout << "Year: " << cars[i].year << endl;
        cout << "Color: " << cars[i].color << endl;
        cout << "Price per day: " << cars[i].pricePerDay << endl;
        cout << "---------------------" << endl;
    }
}


// Create a csv file for clients
void write_client_to_csv(Client clients, const string& filename) {

    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    //file << "UserID,FirstName,LastName,Password,Phone,Email,NbReservation,Role\n";

    file << clients.usertID << ","
        << clients.firstName << ","
        << clients.lastName << ","
        << sha256(clients.password) << ","
        << clients.phone << ","
        << clients.email << ","
        << clients.nbReservation << ","
        << clients.role << "\n";
    file.close();
}

// Create a csv file for cars
void write_car_to_csv(Car cars, const string& filename) {
    ofstream file("cars.csv", ios::app);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }
    //file << "PlateNumber,Brand,Model,Year,Color,PricePerDay($)\n";

    file << cars.plateNumber << ","
        << cars.brand << ","
        << cars.model << ","
        << cars.year << ","
        << cars.color << ","
        << cars.pricePerDay << "\n";

    file.close();
}


void write_rented_car_to_csv(Client& clients, const string& filename) {

    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    //file << "UserID,PlateNumber,StartDate,EndDate\n";
   

    file << clients.usertID << ","
        << clients.c->plateNumber << ","
        << clients.c->d.startDate << ","
        << clients.c->d.endDate << "\n";
    file.close();
}

 

// Reading data from a CSV file for client
void read_client_from_csv(Client* clients, int size, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    int i = 0;
    while (getline(file, line) && i < size) {
        stringstream ss(line);
        ss >> clients[i].usertID;
        ss.ignore(); // Ignore the comma
        getline(ss, clients[i].firstName, ',');
        getline(ss, clients[i].lastName, ',');
        getline(ss, clients[i].password, ',');
        getline(ss, clients[i].phone, ',');
        getline(ss, clients[i].email, ',');
        ss >> clients[i].nbReservation;
        ss.ignore(); // Ignore the comma
        getline(ss, clients[i].role, ',');
        i++;
    }
    file.close();
}

// Reading data from a CSV file for cars
void read_cars_from_csv(Car* cars, int size, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    int i = 0;
    while (getline(file, line) && i < size) {
        stringstream ss(line);
        getline(ss, cars[i].plateNumber, ',');
        getline(ss, cars[i].brand, ',');
        getline(ss, cars[i].model, ',');
        ss >> cars[i].year;
        ss.ignore(); // Ignore the comma
        getline(ss, cars[i].color, ',');
        ss >> cars[i].pricePerDay;
        ss.ignore(); // Ignore the comma
        i++;
    }
    file.close();
}

void read_rented_cars_from_csv(Client* clients, int size, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    int i = 0; 
    while (getline(file, line) && i < size) {
        stringstream ss(line);
        ss >> clients[i].usertID;
        ss.ignore(); // Ignore the comma
        clients[i].c = new Car;
        getline(ss, clients[i].c->plateNumber, ',');
        getline(ss, clients[i].c->d.startDate, ',');
        getline(ss, clients[i].c->d.endDate, ',');
        i++;
    }
    file.close();

}


void addCar() {
    Car newCar;
    string plateNumber;
    int choice = 0;
    int size = getCarNumber();

    Car* cars = createCarsArray("cars.csv", size);

    ofstream file("cars.csv", ios::app);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    cout << "\nEnter the following car details:" << endl;

    // Plate Number
    cout << "Plate Number: ";
    getline(cin, plateNumber);

    if (authenticatePlateNumber(cars, size, plateNumber) == true) {
        cout << "The car already exists. Try again or exit\n";
        return;
    }

    newCar.plateNumber = plateNumber;

    // Brand
    cout << "Brand: ";
    getline(cin, newCar.brand);

    // Model
    cout << "Model: ";
    getline(cin, newCar.model);

    // Year
    cout << "Year: ";
    cin >> newCar.year;
    cin.ignore();

    // Color
    cout << "Color: ";
    getline(cin, newCar.color);

    // Price Per Day
    cout << "Price Per Day (in $): ";
    cin >> newCar.pricePerDay;


    write_car_to_csv(newCar, "cars.csv");

    cout << "Car added successfully!" << endl;
}

void removeCar() {
    string plateNumber;
    int sizeOfArrayCars = getCarNumber();
    Car* cars = createCarsArray("cars.csv", sizeOfArrayCars);

    cout << endl;
    displayCars(cars, sizeOfArrayCars);
    cout << endl;

  
        ofstream file("temp.csv");
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }
        cout << "Enter the plate number of the car: ";
        getline(cin, plateNumber);

        if (authenticatePlateNumber(cars, sizeOfArrayCars, plateNumber) == false) {
            cout << "Plate number not available. " << endl;
            return;
        }

        file << "PlateNumber,Brand,Model,Year,Color,PricePerDay\n";

        for (int j = 0; j < sizeOfArrayCars; ++j) {
            if (cars[j].plateNumber != plateNumber) {
                file << cars[j].plateNumber << ","
                    << cars[j].brand << ","
                    << cars[j].model << ","
                    << cars[j].year << ","
                    << cars[j].color << ","
                    << cars[j].pricePerDay << "\n";
            }
        }

        file.close();

        // Rename temp.csv to cars.csv to overwrite the original file
        remove("cars.csv");
        rename("temp.csv", "cars.csv");

        cout << "Car removed successfully!" << endl;
    
}

void modifyCar() {

    int sizeOfArrayCars = getCarNumber();
    Car* cars = createCarsArray("cars.csv", sizeOfArrayCars);

    string plate;
    displayCars(cars, sizeOfArrayCars);


    cout << "Enter plate number of the car to modify: ";
    cin >> plate;

    bool found = false;
    for (int i = 0; i < sizeOfArrayCars; i++) {
        if (cars[i].plateNumber == plate) {
            cout << "Enter new brand: ";
            cin >> cars[i].brand;
            cout << "Enter new model: ";
            cin >> cars[i].model;
            cout << "Enter new year: ";
            cin >> cars[i].year;
            cout << "Enter new color: ";
            cin >> cars[i].color;
            cout << "Enter new price per day: ";
            cin >> cars[i].pricePerDay;


            // Write the updated client information to the CSV file
            ofstream file("cars.csv", ios::trunc); // Opening the file in truncation mode
            if (!file.is_open()) {
                cerr << "Error: Unable to open file for writing." << endl;
                return;
            }

            file << "PlateNumber,Brand,Model,Year,Color,PricePerDay\n"; // Writing header

            for (int j = 0; j < sizeOfArrayCars; ++j) {
                file << cars[j].plateNumber << ", " 
                    << cars[j].brand << ", " 
                    << cars[j].model << ", "
                    << cars[j].year << " ," 
                    << cars[j].color << ", " 
                    << cars[j].pricePerDay << ", "
                    << cars[j].d.startDate << ", "
                    << cars[j].d.endDate << endl;
            }

            file.close(); // Closing the file

            found = true;
            cout << "Car modified successfully." << endl;
            break;
        }
    }

    if (!found) {
        cout << "Car not found." << endl;
    }
}

int stringToInt(string time) {

    string strDay, strMon, strYear;

    stringstream ss(time);

    getline(ss, strDay, '/');
    getline(ss, strMon, '/');
    getline(ss, strYear, '/');

    int day = stoi(strDay);
    int month = stoi(strMon);
    int year = stoi(strYear);

    int Date = year * 10000 + month * 100 + day; //it will look like 20240502 (YYYYMMDD)
    return Date;
}

/*int stringToInt1(string time) {

   

    stringstream ss(time);

   

    int date = stoi(time);

    
    return date;
}*/


void read_from_csv(string PDF[200][9], const string& filename, int size = 100) {
   
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    int i = 0;
    while (getline(file, line) && i < size) {
        stringstream ss(line);
        getline(ss,PDF[i][0], ',');
        getline(ss, PDF[i][1], ',');
        getline(ss, PDF[i][2], ',');
        getline(ss, PDF[i][3], ',');
        getline(ss, PDF[i][4], ',');
        getline(ss, PDF[i][5], ',');
        getline(ss, PDF[i][6], ',');
        getline(ss, PDF[i][7], ',');
        getline(ss, PDF[i][8], ',');
        
        
        i++;
    }
    file.close();
    
}

void adminPage() {

    int choice;
    cout << "\nAdmins' page:\nPlease select your choice:\n";
    cout << "1. Add a car" << endl;
    cout << "2. Remove a car" << endl;
    cout << "3. Modify a car " << endl;
    cout << "4. Display all the rented cars information with their clients'" << endl;

    do {

        cout << "\nEnter your choice:  ";

        cin >> choice;

        if (choice < 1 || choice > 4)

            cout << "Invalid response. Please try again." << endl;

    } while (choice < 1 || choice > 4);
    cin.ignore();

    switch (choice) {

    case 1:
        addCar();
        break;

    case 2:
        removeCar();
        break;

    case 3:
        modifyCar();
        break;
    case 4:{
        cout << endl;
        int sizeOfArrayClients = getNextUserID() - 1;
        int userID;


        Client* clients = NULL;
        clients = createClientArray("users.csv", sizeOfArrayClients);



        int rentedsize = getRentedCarNumber();
        Client* c = createRentedCarsArray("rentedcars.csv", rentedsize);

        int size = getCarNumber();
        Car* cars = createCarsArray("cars.csv", size);


        Client PDFarray[maxRows][maxCols];

        createPDFarray(PDFarray, clients, sizeOfArrayClients, cars, size, c, rentedsize);

        sortPDFarray(PDFarray, rentedsize);

        displayPDFarray(PDFarray, rentedsize);
        cout << endl<<endl;
        string newarr[maxRows][9];

        for (int i = 0; i < rentedsize; i++) {
            for (int j = 0; j < 9; j++) {
                newarr[i][0] = to_string(PDFarray[i][0].usertID);
                newarr[i][1] = PDFarray[i][1].firstName;
                newarr[i][2] = PDFarray[i][2].lastName;
                newarr[i][3] = (PDFarray[i][3].phone);
                newarr[i][4] = (PDFarray[i][4].email);
                newarr[i][5] = (PDFarray[i][5].c->plateNumber);
                newarr[i][6] = (PDFarray[i][6].c->d.startDate);
                newarr[i][7] = PDFarray[i][7].c->d.endDate;
                newarr[i][8] = to_string(PDFarray[i][8].c->pricePerDay);
            }
        }

        

        writeCSV(newarr,rentedsize, "PDF.csv");

        read_from_csv(newarr, "PDF.csv", rentedsize);

        string calc="UserID, FirstName, LastName, Phone, Email, PlateNumber, StartDate, EndDate, PricePerDay($)\n";

     
        for (int i = 0; i < rentedsize; i++) {
           
            calc += newarr[i][0] + " " + newarr[i][1] + " " + newarr[i][2] + " " + newarr[i][3] + " " + newarr[i][4] + " " + newarr[i][5] + " " + newarr[i][6] + " " + newarr[i][7] + " " + newarr[i][8] + " ";
        
            calc += "\n";
        }

        string information =calc;
        ofstream pdfFile("rentcarcutumerpdf.pdf", ios::binary); // Changed PDF name

        // PDF header
        pdfFile << "%PDF-1.7\n";

        // PDF body
        pdfFile << "1 0 obj\n";
        pdfFile << "<< /Type /Catalog /Pages 2 0 R >>\n";
        pdfFile << "endobj\n";
        pdfFile << "2 0 obj\n";
        pdfFile << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n";
        pdfFile << "endobj\n";
        pdfFile << "3 0 obj\n";
        pdfFile << "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] /Contents 4 0 R >>\n";
        pdfFile << "endobj\n";
        pdfFile << "4 0 obj\n";
        pdfFile << "<< /Length 200 >>\n"; // Adjusted length to accommodate additional content
        pdfFile << "stream\n";
        pdfFile << "BT\n";
        pdfFile << "/F1 8 Tf\n"; // Changed font size to 8
        pdfFile << "100 720 Td\n"; // Start from higher position
        size_t startPos = 0;
        size_t foundPos = information.find("\n", startPos);
        while (foundPos != string::npos) {
            pdfFile << "(" << information.substr(startPos, foundPos - startPos) << ") Tj\n"; // Insert text here
            pdfFile << "0 -10 Td\n"; // Move down 10 points
            startPos = foundPos + 1;
            foundPos = information.find("\n", startPos);
        }
        if (startPos < information.size()) {
            pdfFile << "(" << information.substr(startPos) << ") Tj\n"; // Insert remaining text
        }
        pdfFile << "ET\n";
        pdfFile << "endstream\n";
        pdfFile << "endobj\n";

        // PDF xref table
        pdfFile << "xref\n";
        pdfFile << "0 5\n";
        pdfFile << "0000000000 65535 f \n";
        pdfFile << "0000000009 00000 n \n";
        pdfFile << "0000000061 00000 n \n"; // Adjusted offset to accommodate the change in stream length
        pdfFile << "0000000125 00000 n \n";
        pdfFile << "0000000194 00000 n \n";

        // PDF trailer
        pdfFile << "trailer\n";
        pdfFile << "<< /Size 5 /Root 1 0 R >>\n";
        pdfFile << "startxref\n";
        pdfFile << "237\n";
        pdfFile << "%%EOF\n";

        pdfFile.close();
    }
          break;
    default:
        cout << "Error";
        break;
    }
}


string currentTime() {
    time_t t = time(nullptr);
    tm now;
    localtime_s(&now, &t);
    string orderDate = to_string(now.tm_mday) + "/" + to_string(now.tm_mon + 1) + "/" + to_string(now.tm_year + 1900);

    return orderDate;
}

int stringToIntCurrentTime() {

    string str0 = currentTime();

    string strDay, strMon, strYear;

    // Create a stringstream to parse the date string
    stringstream ss(str0);

    // Extract day, month, year from the date string using getline and '-'
    getline(ss, strDay, '/');
    getline(ss, strMon, '/');
    getline(ss, strYear, '/');

    // Convert extracted strings to integers
    int day = stoi(strDay);
    int month = stoi(strMon);
    int year = stoi(strYear);

    // Combine day, month, year into a single integer (representing date)
    int orderDate = year * 10000 + month * 100 + day; //it will look like 20240502 (YYYYMMDD)
    return orderDate;
}

void rentCar(int userID, Client* clients, int clientSize) {

    int carSize = getCarNumber();
    Car* cars = createCarsArray("cars.csv", carSize);

    int rentedSize = getRentedCarNumber();
    Client* rentedCars = createRentedCarsArray("rentedcars.csv", rentedSize);

    int now = stringToIntCurrentTime();
    int intStartDate, intEndDate;

    cout << endl;
    displayCars(cars, carSize);
    cout << endl;

    clients[userID - 1].nbReservation++;

    Client rental;
    Car* rentalCar = new Car[clients[userID - 1].nbReservation];

    rental.usertID = userID;

    string plateNumber;
    string startDate;
    string endDate;

    do {
        cout << "Enter the plate number of the car you want to rent: ";
        getline(cin, plateNumber);

        if (!authenticatePlateNumber(cars, carSize, plateNumber)) {
            cout << "Plate number unavailable. Please try again  ";
        }

    } while (!authenticatePlateNumber(cars, carSize, plateNumber));

    cout << "Enter the rental date (DD-MM-YYYY) for the car: ";

    do {
        do {
            cout << "Start Date: ";
            getline(cin, startDate);

            intStartDate = stringToInt(startDate);

            if (now > intStartDate) {
                cout << "The entered value is invalid. Please try again. " << endl;
            }

        } while (now > intStartDate);

        do {
            cout << "End Date: ";
            getline(cin, endDate);

            intEndDate = stringToInt(endDate);

            if (intEndDate < intStartDate) {
                cout << "The entered value is invalid. Please try again. " << endl;
            }

        } while (intEndDate < intStartDate);


        if (!checkCarAvailability(rentedCars, rentedSize, plateNumber, intStartDate, intEndDate)) {
            cout << "The car is not available on these dates. Please try again. ";
        }

    } while (!checkCarAvailability(rentedCars, rentedSize, plateNumber, intStartDate, intEndDate));


    rentalCar->plateNumber = plateNumber;
    rentalCar->d.startDate = startDate;
    rentalCar->d.endDate = endDate;

    rental.c = rentalCar;

    write_rented_car_to_csv(rental, "rentedcars.csv");



    if (clients[userID - 1].c == NULL) {
        clients[userID - 1].c = new Car[1];
    }

    else {

        Client newArray;
        newArray.c = new Car[clients[userID - 1].nbReservation];

        for (int i = 0; i < clients[userID - 1].nbReservation; i++) {
            if (i < clients[userID - 1].nbReservation-1) {
                newArray.c = clients[userID - 1].c;
            }
            if (i == clients[userID - 1].nbReservation-1) {
                newArray.c = rental.c;
            }
        }
        delete[]clients[userID - 1].c;
        clients[userID - 1].c = newArray.c;
    }


      // Write the updated client information to the CSV file
    ofstream file("users.csv", ios::trunc); // Opening the file in truncation mode
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    file << "UserID,FirstName,LastName,Password,Phone,Email,NbReservation,Role\n"; // Writing header

    for (int j = 0; j < clientSize; ++j) {
        file << clients[j].usertID << ","
            << clients[j].firstName << ","
            << clients[j].lastName << ","
            << clients[j].password << ","
            << clients[j].phone << ","
            << clients[j].email << ","
            << clients[j].nbReservation << ","
            << clients[j].role << "\n";
    }

    file.close(); // Closing the file
    cout << "\nCar rented successfully!\n\nPress enter to continue. ";
}


bool authenticateCancelReservation(Client* rentedCars, int size, string plateNumber, string startDate) {
    for (int i = 0; i < size; i++)
        if (rentedCars[i].c->plateNumber == plateNumber && rentedCars[i].c->d.startDate == startDate)
            return true;

    return false;

}

void writeUpdatedUsersFile(Client* clients, int size) {
    // Write the updated client information to the CSV file
    ofstream file("users.csv", ios::trunc); // Opening the file in truncation mode
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    file << "UserID,FirstName,LastName,Password,Phone,Email,NbReservation,Role\n"; // Writing header

    for (int j = 0; j < size; ++j) {
        file << clients[j].usertID << ","
            << clients[j].firstName << ","
            << clients[j].lastName << ","
            << clients[j].password << ","
            << clients[j].phone << ","
            << clients[j].email << ","
            << clients[j].nbReservation << ","
            << clients[j].role << "\n";
    }

    file.close(); // Closing the file
}
void CancelReservation(int userID, int rentedsize, Client* rentedCars, Client* clients) {

    int sizeClientArray = getNextUserID() - 1;
    // Display rented cars of the client
    //cout << "Rented cars for user " << clients[userID - 1].firstName << " " << clients[userID - 1].lastName << ":" << endl;

    cout << "Rented Cars: " << endl;
    cout << "Plate Number\tStart Date\tEnd Date" << endl;

 
    for (int i = 0; i < rentedsize; ++i) {
        if (rentedCars[i].usertID == userID && rentedCars[i].c->plateNumber != "-") {
            cout << rentedCars[i].c->plateNumber << "\t\t" << rentedCars[i].c->d.startDate << "\t" << rentedCars[i].c->d.endDate << endl;
        }
    }
    cout << endl;

    // ask which one to cancel
    string plateNumber;
    string startDate;



  
        ofstream file("temp.csv");
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        cout << "Enter the plate number of the car that you want to cancel: ";
        getline(cin, plateNumber);
        cout << "Enter the start date of the car that you want to cancel: ";
        getline(cin, startDate);

        if (authenticateCancelReservation(rentedCars, rentedsize, plateNumber, startDate) == false) {
            cout << "Plate number and date are not available. " << endl;
            return;
        }

        int index = 0;
        for (int k = 0; k < rentedsize; k++) {
            if ((rentedCars[k].c->plateNumber == plateNumber) && (rentedCars[k].c->d.startDate == startDate)) {
                index = k;
                break;
            }
        }

        file << "UserID,PlateNumber,StartDate,EndDate\n";

        for (int j = 0; j < rentedsize; ++j) {
            if (j!=index) {

                file << rentedCars[j].usertID << ","
                    << rentedCars[j].c->plateNumber << ","
                    << rentedCars[j].c->d.startDate << ","
                    << rentedCars[j].c->d.endDate << "\n";

            }
        }

        file.close();

        // Rename temp.csv to cars.csv to overwrite the original file
        remove("rentedcars.csv");
        rename("temp.csv", "rentedcars.csv");

        clients[userID - 1].nbReservation--;

        Client newArray;
        newArray.c = new Car[clients[userID - 1].nbReservation];

        for (int i = 0; i < clients[userID - 1].nbReservation + 1; i++) {
            if (i != index) {
                newArray.c = clients[userID - 1].c;
            }
        }
        delete[]clients[userID - 1].c;
        clients[userID - 1].c = newArray.c;


        writeUpdatedUsersFile(clients, sizeClientArray);

        cout << "Car removed successfully!" << endl;

    
}


bool isDateGreaterThanCurrent(string date) {
    // Get current date
    string currentDateString = currentTime();

    // Convert current date and given date to integers for comparison
    int currentDateInt = stringToInt(currentDateString);
    int givenDateInt = stringToInt(date);

    // Compare dates
    if (givenDateInt > currentDateInt)
        return true;
    else
        return false;
}

bool areDatesAvailable(string startDate, string endDate) {

    // Convert start date and end date to integers for comparison
    int startDateInt = stringToInt(startDate);
    int endDateInt = stringToInt(endDate);

    // Check if end date is greater than start date
    if (endDateInt > startDateInt)
        return true;
    else
        return false;
}

void modifyReservation(int userID, int rentedsize, Client* rentedCars, Client* clients) {

    int sizeClientArray = getNextUserID() - 1;

    // Display rented cars of the client
   // cout << "Rented cars for user " << clients[userID - 1].firstName << " " << clients[userID - 1].lastName << ":" << endl;

    cout << "Rented Cars: " << endl;
    cout << "Plate Number\tStart Date\tEnd Date" << endl;

  
    for (int i = 0; i < rentedsize; ++i) {
        if (rentedCars[i].usertID == userID && rentedCars[i].c->plateNumber != "-") {
            cout << rentedCars[i].c->plateNumber << "\t\t" << rentedCars[i].c->d.startDate << "\t" << rentedCars[i].c->d.endDate << endl;
        }
    }
    cout << endl;

   
    // Ask the client which reservation to modify
    string plateNumber;
    string OldstartDate;
    cout << "Enter the plate number of the car you want to modify the reservation for: ";
    getline(cin, plateNumber);

    cout << "Enter the old start date of the rented car: ";
    getline(cin, OldstartDate);

    string newStartDate, newEndDate;
    
    bool found1 = false;
    bool found = false;
    for (int i = 0; i < rentedsize; ++i) {
        if (rentedCars[i].usertID == userID && rentedCars[i].c->plateNumber == plateNumber && rentedCars[i].c->d.startDate == OldstartDate) {
            found = true;

            bool validDates = false;
            while (!validDates) {

                // Enter new reservation date
                cout << "\nEnter the new rental dates (Start Date and End Date):" << endl;
               
                cout << "Start Date (YYYY-MM-DD): ";
                getline(cin, newStartDate);
                cout << "End Date (YYYY-MM-DD): ";
                getline(cin, newEndDate);

                // Check if the entered dates are valid and available
                if (isDateGreaterThanCurrent(newStartDate) && isDateGreaterThanCurrent(newEndDate) && areDatesAvailable(newStartDate, newEndDate)) {

                    // Update reservation with new dates
                    rentedCars[i].c->d.startDate = newStartDate;
                    rentedCars[i].c->d.endDate = newEndDate;

                    if (found1) {
                        int index = 0;
                        for (int k = 0; k < rentedsize; k++) {
                            if ((rentedCars[k].c->plateNumber == plateNumber) && (rentedCars[k].c->d.startDate == OldstartDate)) {
                                index = k;
                                break;
                            }
                        }
                        cout << index;
                        for (int i = 0; i < clients[userID - 1].nbReservation + 1; i++) {
                            if (i == index) {
                                clients[userID - 1].c->d.startDate = newStartDate;
                                clients[userID - 1].c->d.endDate = newEndDate;
                            }
                        }
                    }

                    cout << "Reservation modified successfully!" << endl;
                    validDates = true; // Exit the loop as valid dates are entered
                }
                else {
                    cout << "Invalid dates or car not available for the selected period. Please enter valid dates." << endl;
                }
            }
         
            // Write the updated client information to the CSV file
            ofstream file("rentedcars.csv", ios::trunc); // Opening the file in truncation mode
            if (!file.is_open()) {
                cerr << "Error: Unable to open file for writing." << endl;
                return;
            }
            file << "UserID,PlateNumber,StartDate,EndDate\n";
            
            for (int j = 0; j < rentedsize; ++j) {
                file << rentedCars[j].usertID << ","
                    << rentedCars[j].c->plateNumber << ","
                    << rentedCars[j].c->d.startDate << ","
                    << rentedCars[j].c->d.endDate << "\n";
            }

            file.close(); // Closing the file
            break;
        }
    }
    if (!found )
        cout << "Invalid dates or car not available for the selected period. Please enter valid dates." << endl;
}


void clientPage(int userID, Client *clients, int clientSize) {
    int clientPageCheck = 0;
  do{
    int choice;

    cout << "\nClients' page:\nPlease select your choice:\n";
    cout << "1. Rent a car" << endl;
    cout << "2. Cancel a car reservation" << endl;
    cout << "3. Modify a car reservation " << endl;
    cout << "4. Exit " << endl;

    do {

        cout << "Enter your choice:  ";

        cin >> choice;

        if (choice < 1 || choice > 4)

            cout << "Invalid response. Please try again." << endl;

    } while (choice < 1 || choice > 4);
    cin.ignore();

    switch (choice) {

    case 1: 
        rentCar(userID, clients,clientSize );
        cin.ignore();
    
        break;

    case 2: {
        int size = getRentedCarNumber();
        Client* rentedCars = createRentedCarsArray("rentedcars.csv", size);
        CancelReservation(userID, size,rentedCars , clients);
    }
        break;

    case 3: {
        int size = getRentedCarNumber();
        Client* rentedCars = createRentedCarsArray("rentedcars.csv", size);
        modifyReservation(userID, size, rentedCars, clients);
    }
        break;

    case 4: {
        clientPageCheck = 1;
        cout << "Goodbye!";
    }
        break;

    default:
        cout << "Error";
        break;
    }
   }while (clientPageCheck==0);
}

Client createAccount() {
    Client newClient;
    newClient.usertID = getNextUserID();

    cout << "\nRegistration Page: \nPlease enter the following information:" << endl << endl;
    cout << "First name: ";
    getline(cin >> ws, newClient.firstName);
    cout << "Last name: ";
    getline(cin >> ws, newClient.lastName);

    bool hasNumber = false, hasLetter = false, hasSpecialCharacter = false;

    do {

        cout << "Password: " << endl;
        cout << "P.S. the password's length must be at least 8 character, contain numbers, letters and special characters. ";
        getline(cin, newClient.password);
        if (newClient.password.length() < 8) {
            cout << "Password is too short. Please try again." << endl;
            continue;
        }

        for (int i = 0; newClient.password[i] != '\0'; i++) {
            if (isdigit(newClient.password[i]))
                hasNumber = true;

            else if (isalpha(newClient.password[i]))
                hasLetter = true;

            else if (!isspace(newClient.password[i]))
                hasSpecialCharacter = true;
        }
        if (hasNumber == false || hasLetter == false || hasSpecialCharacter == false) {

            cout << "Password must contain numbers, letters, and special characters. Please try again." << endl;
        }

    } while (hasNumber == false || hasLetter == false || hasSpecialCharacter == false || newClient.password.length() < 8);

    bool case1 = true, case2 = true, case3 = true;
    do {
        cout << "Phone Number: (format: ** ******) ";
        getline(cin, newClient.phone);

        if (newClient.phone.length() != 9) {
            cout << "Invalid number. Please try again." << endl;
            continue;
        }

        for (int i = 0; i <= 1; i++) {
            if (!isdigit(newClient.phone[i]))
                case1 = false;
        }

        if (newClient.phone[2] != ' ')
            case2 = false;

        for (int i = 3; i <= 8; i++)
            if (!isdigit(newClient.phone[i]))
                case3 = false;

        if (case1 == false || case2 == false || case3 == false)
            cout << "Invalid number. Please try again." << endl;

    } while (case1 == false || case2 == false || case3 == false || newClient.phone.length() != 9);


    do {
        cout << "Email: (format: ***@**.**) ";
        getline(cin, newClient.email);
        if (newClient.email.find('@') == string::npos || newClient.email.find('.') == string::npos || newClient.email.find('@') > newClient.email.find('.'))
            cout << "Invalid email. Please try again." << endl;
    } while (newClient.email.find('@') == string::npos || newClient.email.find('.') == string::npos || newClient.email.find('@') > newClient.email.find('.'));


    do {
        cout << "Are you an Admin or a Client? (A for Admin and C for Client) ";
        cin >> newClient.role;
        if (newClient.role != "A" && newClient.role != "a" && newClient.role != "C" && newClient.role != "c")
            cout << "Invalid response. Please try again." << endl;
    } while (newClient.role != "A" && newClient.role != "a" && newClient.role != "C" && newClient.role != "c");


    if (newClient.role == "A" || newClient.role == "a")
        newClient.role = "Admin";
    else
        newClient.role = "Client";


    //Reservation number is initially 0 always 
    newClient.nbReservation = 0;

    newClient.c = new Car;

    newClient.c->plateNumber = "-";
    newClient.c->d.startDate = "-";
    newClient.c->d.endDate = "-";

    //to write in csv
    write_client_to_csv(newClient, "users.csv");

    return newClient;

}

void welcome(int& AccountCheck) {

    int choice;
    string password;

    int sizeOfArrayClients = getNextUserID() - 1;
    int userID;
    

    Client* clients = NULL;
    clients = createClientArray("users.csv", sizeOfArrayClients);

    

    int rentedsize = getRentedCarNumber();
    Client* c = createRentedCarsArray("rentedcars.csv", rentedsize);

    int size = getCarNumber();
    Car* cars = createCarsArray("cars.csv", size);


    cout << "Thank you for choosing our services!!" << endl;

    cout << endl;

    cout << "1. Register\n";

    cout << "2. Login\n";

    cout << "3. Exit\n";

    do {

        cout << "\nEnter your choice:  ";

        cin >> choice;

        if (choice < 1 || choice > 3)

            cout << "Invalid response. Please try again." << endl;

    } while (choice < 1 || choice > 3);

    switch (choice) {
    case 1: {

        Client registration = createAccount();

        if (clients != NULL) {
            AccountCheck = 0;
            cout << "\nNew account created successfully! " << endl;
            cout << "Your ID is " << registration.usertID << endl<< endl;

            cout << "Exit or Login to your new account to continue. ";
        }
        else {
            AccountCheck = 0;
            cout << "Error in creating your account. Please try again.";
        }
        break;
    }

    case 2: {

        AccountCheck = 1;
        cout << "\n\nLogin page:\nEnter your ID: ";

        cin >> userID;

        cout << "Enter your password: ";

        getline(cin >> ws, password);

        if (authenticateClient(clients, sizeOfArrayClients, userID, password) == true) {
            AccountCheck = 1;
            cout << "Login successful!" << endl;

            if (clients[userID - 1].role == "Admin")
                adminPage();
            else
                clientPage(userID, clients, sizeOfArrayClients);
        }
        else {
            cout << "\nLogin failed. Incorrect userID or password. \nPlease try again to log into your account or register." << endl << endl;
            AccountCheck = 0;
        }

        break;
    }


    case 3: {
        AccountCheck = 1;
        cout << "Goodbye!";

        break;
    }
    default: {
        AccountCheck = 1;
        cout << "Error";

        break;

    }

    }
}

int main() {

    int AccountCheck = 0;

    while (AccountCheck == 0)
        welcome(AccountCheck);

    return 0;

}

