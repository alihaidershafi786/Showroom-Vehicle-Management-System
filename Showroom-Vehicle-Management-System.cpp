#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

// ========================================================
//   SMART SHOWROOM + VEHICLE MAINTENANCE MANAGEMENT SYSTEM
//   v3.0 — Stable, Full-Featured, All Bugs Fixed
// ========================================================

const int MAX_VEHICLES  = 100;
const int MAX_CUSTOMERS = 200;
const int MAX_RECORDS   = 200;
const int MAX_RENTALS   = 100;
const int MAX_INSURANCE = 100;

// ============================================================
//  INPUT VALIDATION
// ============================================================

int readInt(const string &prompt)
{
    int val;
    while (true)
    {
        if (!prompt.empty()) cout << prompt;
        cin >> val;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Please enter a whole number.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

float readFloat(const string &prompt)
{
    float val;
    while (true)
    {
        if (!prompt.empty()) cout << prompt;
        cin >> val;
        if (cin.fail() || val < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Please enter a valid positive number.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

// Always call after cin stream is clean (no pending \n)
string readLine(const string &prompt)
{
    string val;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        getline(cin, val);
        if (!val.empty()) return val;
        cout << "  [!] This field cannot be empty.\n";
    }
}

string readCNIC(const string &prompt)
{
    string val;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        cin >> val;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bool ok = (val.size() == 13);
        if (ok) for (char c : val) if (!isdigit(c)) { ok = false; break; }
        if (ok) return val;
        cout << "  [!] CNIC must be exactly 13 digits (no dashes). Try again.\n";
    }
}

string readDate(const string &prompt)
{
    string val;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        cin >> val;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (val.size() == 10 && val[2] == '/' && val[5] == '/') return val;
        cout << "  [!] Date must be DD/MM/YYYY format. Try again.\n";
    }
}

// ============================================================
//  FORMAT PKR
// ============================================================

string formatPKR(float amount)
{
    char buf[80];
    long long amt = (long long)amount;
    if      (amt >= 10000000) sprintf(buf, "%.2f Crore  (PKR %.0f)", amount/10000000.0f, amount);
    else if (amt >= 100000)   sprintf(buf, "%.2f Lakh   (PKR %.0f)", amount/100000.0f,  amount);
    else                      sprintf(buf, "PKR %.2f", amount);
    return string(buf);
}

// ============================================================
//  FILE I/O HELPERS  (static, used by all classes)
// ============================================================

static void wf(ofstream &f, const string &s)
{
    f << s.size() << "\n";
    if (!s.empty()) f.write(s.c_str(), s.size());
    f << "\n";
}

static void rf(ifstream &f, string &s)
{
    size_t len = 0;
    f >> len; f.ignore();
    s.assign(len, ' ');
    if (len > 0) f.read(&s[0], len);
    f.ignore();
}

// ============================================================
//  VEHICLE BASE CLASS
// ============================================================

class Vehicle
{
protected:
    int    vehicleID;
    string brand, model, color, year;
    float  price;
    string status;       // Available | Sold | Rented
    string ownership;    // OUR_SALE | OUR_RENT | COMMISSION
    string ownerName;
    float  commissionPct;

public:
    Vehicle() : vehicleID(0), price(0), status("Available"),
                ownership("OUR_SALE"), commissionPct(0) {}

    void addVehicleManual()
    {
        vehicleID     = readInt  ("  Enter Vehicle ID        : ");
        brand         = readLine ("  Enter Brand             : ");
        model         = readLine ("  Enter Model             : ");
        color         = readLine ("  Enter Color             : ");
        year          = readLine ("  Enter Year              : ");
        price         = readFloat("  Enter Price (PKR)       : ");

        cout << "\n  Vehicle Purpose:\n"
             << "  1. Our Vehicle - For Sale\n"
             << "  2. Our Vehicle - For Rent\n"
             << "  3. Third-Party (Commission) - For Sale\n"
             << "  4. Third-Party (Commission) - For Rent\n";
        int otype = readInt("  Select: ");
        status = "Available";
        if      (otype == 1) ownership = "OUR_SALE";
        else if (otype == 2) ownership = "OUR_RENT";
        else if (otype == 3 || otype == 4) {
            ownership     = "COMMISSION";
            ownerName     = readLine ("  Enter Owner Full Name      : ");
            commissionPct = readFloat("  Enter Commission % (e.g 5) : ");
        } else ownership = "OUR_SALE";
    }

    void setVehicle(int id, string br, string mo, string co, string yr,
                    float pr, string own, string stat = "Available",
                    string oName = "", float comm = 0)
    {
        vehicleID=id; brand=br; model=mo; color=co; year=yr;
        price=pr; ownership=own; status=stat; ownerName=oName; commissionPct=comm;
    }

    // FIX: editVehicle now uses readLine/readFloat consistently — no raw cin mix
    void editVehicle()
    {
        cout << "\n  Edit Vehicle — press Enter to keep current value.\n";

        cout << "  Brand  [" << brand << "]: ";
        string t; getline(cin, t); if (!t.empty()) brand = t;

        cout << "  Model  [" << model << "]: ";
        getline(cin, t); if (!t.empty()) model = t;

        cout << "  Color  [" << color << "]: ";
        getline(cin, t); if (!t.empty()) color = t;

        cout << "  Year   [" << year << "]: ";
        getline(cin, t); if (!t.empty()) year = t;

        cout << "  Price  [" << price << "] (0 = keep): ";
        getline(cin, t);
        if (!t.empty() && t != "0") {
            try { float fp = stof(t); if (fp > 0) price = fp; } catch(...) {}
        }

        cout << "  Status [" << status << "] (Available / Sold / Rented, Enter = keep): ";
        getline(cin, t);
        if (t=="Available" || t=="Sold" || t=="Rented") status = t;
    }

    void displayVehicle() const
    {
        string ownLabel;
        if      (ownership == "OUR_SALE") ownLabel = "Ours - For Sale";
        else if (ownership == "OUR_RENT") ownLabel = "Ours - For Rent";
        else ownLabel = "Commission (" + ownerName + " | " + to_string((int)commissionPct) + "%)";

        cout << "\n  +------------------------------------------------------+";
        cout << "\n  | Vehicle ID  : " << left << setw(37) << vehicleID        << "|";
        cout << "\n  | Brand       : " << left << setw(37) << brand            << "|";
        cout << "\n  | Model       : " << left << setw(37) << model            << "|";
        cout << "\n  | Color       : " << left << setw(37) << color            << "|";
        cout << "\n  | Year        : " << left << setw(37) << year             << "|";
        cout << "\n  | Price       : " << left << setw(37) << formatPKR(price) << "|";
        cout << "\n  | Status      : " << left << setw(37) << status           << "|";
        cout << "\n  | Ownership   : " << left << setw(37) << ownLabel         << "|";
        cout << "\n  +------------------------------------------------------+\n";
    }

    int    getID()         const { return vehicleID; }
    float  getPrice()      const { return price; }
    string getBrand()      const { return brand; }
    string getModel()      const { return model; }
    string getStatus()     const { return status; }
    string getOwnership()  const { return ownership; }
    float  getCommission() const { return commissionPct; }
    void   setStatus(string s)   { status = s; }
    bool   isEmpty()       const { return vehicleID == 0; }

    void saveToFile(ofstream &f) const
    {
        f << vehicleID    << "\n";
        f << price        << "\n";
        f << commissionPct<< "\n";
        wf(f, brand);  wf(f, model); wf(f, color);
        wf(f, year);   wf(f, status); wf(f, ownership); wf(f, ownerName);
    }

    void loadFromFile(ifstream &f)
    {
        f >> vehicleID;     f.ignore();
        f >> price;         f.ignore();
        f >> commissionPct; f.ignore();
        rf(f, brand);  rf(f, model); rf(f, color);
        rf(f, year);   rf(f, status); rf(f, ownership); rf(f, ownerName);
    }
};

// ============================================================
//  CAR
// ============================================================

class Car : public Vehicle
{
private:
    string transmission;
    int    doors;
    string fuelType;

public:
    Car() : doors(0) {}

    void addCarManual()
    {
        addVehicleManual();
        transmission = readLine ("  Transmission (Manual/Automatic)   : ");
        doors        = readInt  ("  Number of Doors                   : ");
        fuelType     = readLine ("  Fuel Type (Petrol/Diesel/Hybrid)  : ");
    }

    void setCar(int id,string br,string mo,string co,string yr,float pr,
                string own,string trans,int dr,string fuel,
                string stat="Available",string oName="",float comm=0)
    {
        setVehicle(id,br,mo,co,yr,pr,own,stat,oName,comm);
        transmission=trans; doors=dr; fuelType=fuel;
    }

    void editCar()
    {
        editVehicle();
        cout << "  Transmission [" << transmission << "]: ";
        string t; getline(cin, t); if (!t.empty()) transmission = t;

        cout << "  Doors [" << doors << "] (0 = keep): ";
        getline(cin, t);
        if (!t.empty() && t != "0") { try { doors = stoi(t); } catch(...) {} }

        cout << "  Fuel Type [" << fuelType << "]: ";
        getline(cin, t); if (!t.empty()) fuelType = t;

        cout << "\n  Car updated successfully.\n";
    }

    void displayCar() const
    {
        displayVehicle();
        cout << "  | Type         : Car\n";
        cout << "  | Transmission : " << transmission << "\n";
        cout << "  | Doors        : " << doors        << "\n";
        cout << "  | Fuel Type    : " << fuelType     << "\n";
    }

    void saveToFile(ofstream &f) const
    {
        f << "CAR\n";
        Vehicle::saveToFile(f);
        wf(f, transmission);
        f << doors << "\n";
        wf(f, fuelType);
    }

    void loadFromFile(ifstream &f)
    {
        Vehicle::loadFromFile(f);
        rf(f, transmission);
        f >> doors; f.ignore();
        rf(f, fuelType);
    }
};

// ============================================================
//  BIKE
// ============================================================

class Bike : public Vehicle
{
private:
    int    engineCC;
    string bikeType;

public:
    Bike() : engineCC(0) {}

    void addBikeManual()
    {
        addVehicleManual();
        engineCC = readInt ("  Engine CC                            : ");
        bikeType = readLine("  Bike Type (Sports/Standard/Cruiser)  : ");
    }

    void setBike(int id,string br,string mo,string co,string yr,float pr,
                 string own,int cc,string btype,
                 string stat="Available",string oName="",float comm=0)
    {
        setVehicle(id,br,mo,co,yr,pr,own,stat,oName,comm);
        engineCC=cc; bikeType=btype;
    }

    void editBike()
    {
        editVehicle();
        cout << "  Engine CC [" << engineCC << "] (0 = keep): ";
        string t; getline(cin, t);
        if (!t.empty() && t != "0") { try { engineCC = stoi(t); } catch(...) {} }

        cout << "  Bike Type [" << bikeType << "]: ";
        getline(cin, t); if (!t.empty()) bikeType = t;

        cout << "\n  Bike updated successfully.\n";
    }

    void displayBike() const
    {
        displayVehicle();
        cout << "  | Type      : Bike\n";
        cout << "  | Engine CC : " << engineCC << "\n";
        cout << "  | Bike Type : " << bikeType  << "\n";
    }

    void saveToFile(ofstream &f) const
    {
        f << "BIKE\n";
        Vehicle::saveToFile(f);
        f << engineCC << "\n";
        wf(f, bikeType);
    }

    void loadFromFile(ifstream &f)
    {
        Vehicle::loadFromFile(f);
        f >> engineCC; f.ignore();
        rf(f, bikeType);
    }
};

// ============================================================
//  CUSTOMER RECORD  (auto-created on every transaction)
// ============================================================

class CustomerRecord
{
public:
    int    customerID;
    string name, phone, cnic;
    string activity;
    string vehicleInfo, date;
    float  amountPaid;

    CustomerRecord() : customerID(0), amountPaid(0) {}

    void display() const
    {
        cout << "\n  +------------------------------------------------------+";
        cout << "\n  | Customer ID  : " << left << setw(37) << customerID   << "|";
        cout << "\n  | Name         : " << left << setw(37) << name         << "|";
        cout << "\n  | Phone        : " << left << setw(37) << phone        << "|";
        cout << "\n  | CNIC         : " << left << setw(37) << cnic         << "|";
        cout << "\n  | Activity     : " << left << setw(37) << activity     << "|";
        cout << "\n  | Vehicle      : " << left << setw(37) << vehicleInfo  << "|";
        cout << "\n  | Date         : " << left << setw(37) << date         << "|";
        cout << "\n  | Amount       : " << left << setw(37) << formatPKR(amountPaid) << "|";
        cout << "\n  +------------------------------------------------------+\n";
    }

    void saveToFile(ofstream &f) const
    {
        f << customerID  << "\n";
        f << amountPaid  << "\n";
        wf(f,name); wf(f,phone); wf(f,cnic);
        wf(f,activity); wf(f,vehicleInfo); wf(f,date);
    }

    void loadFromFile(ifstream &f)
    {
        f >> customerID; f.ignore();
        f >> amountPaid; f.ignore();
        rf(f,name); rf(f,phone); rf(f,cnic);
        rf(f,activity); rf(f,vehicleInfo); rf(f,date);
    }
};

// ============================================================
//  RENTAL RECORD
// ============================================================

class RentalRecord
{
public:
    int    rentalID, vehicleID;
    string vehicleInfo;
    string customerName, customerPhone, customerCNIC;
    string startDate;
    int    agreedDays;
    float  rentPerDay, driverPerDay, securityDeposit;
    bool   withDriver, returned;
    string returnDate;
    float  lateFee, totalCharged, refundGiven;

    RentalRecord() : rentalID(0), vehicleID(0), agreedDays(0),
                     rentPerDay(0), driverPerDay(0), securityDeposit(0),
                     withDriver(false), returned(false),
                     lateFee(0), totalCharged(0), refundGiven(0) {}

    void printBill() const
    {
        cout << "\n  ============================================================";
        cout << "\n               SMART SHOWROOM - RENTAL BILL                   ";
        cout << "\n  ============================================================";
        cout << "\n  Rental ID         : " << rentalID;
        cout << "\n  Customer Name     : " << customerName;
        cout << "\n  Phone             : " << customerPhone;
        cout << "\n  CNIC              : " << customerCNIC;
        cout << "\n  Vehicle           : " << vehicleInfo;
        cout << "\n  Driver            : " << (withDriver ? "Yes (With Driver)" : "No (Self-Drive)");
        cout << "\n  Start Date        : " << startDate;
        cout << "\n  Agreed Days       : " << agreedDays;
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  Rent / Day        : " << formatPKR(rentPerDay);
        cout << "\n  Rent Total        : " << formatPKR(rentPerDay * agreedDays);
        if (withDriver)
            cout << "\n  Driver Total      : " << formatPKR(driverPerDay * agreedDays);
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  GRAND TOTAL       : " << formatPKR(totalCharged);
        cout << "\n  Security Deposit  : " << formatPKR(securityDeposit) << "  (Refundable on return)";
        cout << "\n  ============================================================\n";
    }

    void printReturnSettlement() const
    {
        cout << "\n  ============================================================";
        cout << "\n               SMART SHOWROOM - RETURN SETTLEMENT             ";
        cout << "\n  ============================================================";
        cout << "\n  Rental ID         : " << rentalID;
        cout << "\n  Customer          : " << customerName;
        cout << "\n  CNIC              : " << customerCNIC;
        cout << "\n  Vehicle           : " << vehicleInfo;
        cout << "\n  Start Date        : " << startDate;
        cout << "\n  Return Date       : " << returnDate;
        cout << "\n  Agreed Days       : " << agreedDays;
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  Total Charged     : " << formatPKR(totalCharged);
        if (lateFee > 0)
            cout << "\n  Late Fee          : " << formatPKR(lateFee);
        cout << "\n  Security Deposit  : " << formatPKR(securityDeposit);
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  REFUND TO CUSTOMER: " << formatPKR(refundGiven)
             << (lateFee == 0 ? "  (Full — returned on time)" : "  (Partial — late fee deducted)");
        cout << "\n  ============================================================\n";
    }

    void saveToFile(ofstream &f) const
    {
        f << rentalID        << "\n" << vehicleID      << "\n"
          << agreedDays      << "\n" << rentPerDay     << "\n"
          << driverPerDay    << "\n" << securityDeposit<< "\n"
          << withDriver      << "\n" << returned       << "\n"
          << lateFee         << "\n" << totalCharged   << "\n"
          << refundGiven     << "\n";
        wf(f,vehicleInfo); wf(f,customerName); wf(f,customerPhone);
        wf(f,customerCNIC); wf(f,startDate);   wf(f,returnDate);
    }

    void loadFromFile(ifstream &f)
    {
        f >> rentalID;        f.ignore();
        f >> vehicleID;       f.ignore();
        f >> agreedDays;      f.ignore();
        f >> rentPerDay;      f.ignore();
        f >> driverPerDay;    f.ignore();
        f >> securityDeposit; f.ignore();
        f >> withDriver;      f.ignore();
        f >> returned;        f.ignore();
        f >> lateFee;         f.ignore();
        f >> totalCharged;    f.ignore();
        f >> refundGiven;     f.ignore();
        rf(f,vehicleInfo); rf(f,customerName); rf(f,customerPhone);
        rf(f,customerCNIC); rf(f,startDate);   rf(f,returnDate);
    }
};

// ============================================================
//  MAINTENANCE RECORD
// ============================================================

class MaintenanceRecord
{
private:
    int    recordID, vehicleID;
    string customerName, customerPhone, customerCNIC;
    string vehicleInfo, serviceType, serviceDate;
    float  serviceCost;
    string technicianName, notes, nextServiceDate;
    string month;

public:
    MaintenanceRecord() : recordID(0), vehicleID(0), serviceCost(0) {}

    void addRecord(string &oName, string &oPhone, string &oCNIC,
                   string &oVehicle, string &oDate, float &oCost, string &oMonth)
    {
        recordID  = readInt  ("  Enter Record ID                  : ");
        vehicleID = readInt  ("  Enter Vehicle ID                 : ");
        customerName  = readLine("  Enter Customer Full Name         : ");
        customerPhone = readLine("  Enter Customer Phone             : ");
        customerCNIC  = readCNIC("  Enter Customer CNIC (13 digits)  : ");
        vehicleInfo   = readLine("  Enter Vehicle Info               : ");

        cout << "\n  --- Service Types ---\n"
             << "   1. Oil Change          2. Engine Service\n"
             << "   3. Brake Service       4. Tire Change/Alignment\n"
             << "   5. Battery Check       6. AC Service\n"
             << "   7. Car Wash & Polish   8. Transmission Service\n"
             << "   9. Suspension Repair  10. Electrical Work\n"
             << "  11. Custom\n";
        int sc = readInt("  Select: ");
        switch(sc) {
            case 1:  serviceType="Oil Change"; break;
            case 2:  serviceType="Engine Service"; break;
            case 3:  serviceType="Brake Service"; break;
            case 4:  serviceType="Tire Change/Alignment"; break;
            case 5:  serviceType="Battery Check/Replace"; break;
            case 6:  serviceType="AC Service"; break;
            case 7:  serviceType="Full Car Wash & Polish"; break;
            case 8:  serviceType="Transmission Service"; break;
            case 9:  serviceType="Suspension Repair"; break;
            case 10: serviceType="Electrical Work"; break;
            default: serviceType = readLine("  Enter Service Type : ");
        }
        serviceDate    = readDate ("  Enter Service Date (DD/MM/YYYY)  : ");
        serviceCost    = readFloat("  Enter Service Cost (PKR)         : ");
        technicianName = readLine ("  Enter Technician Name            : ");
        notes          = readLine ("  Enter Notes / Parts Used         : ");
        nextServiceDate= readDate ("  Enter Next Service Date          : ");
        month = serviceDate.substr(3, 7);

        oName=customerName; oPhone=customerPhone; oCNIC=customerCNIC;
        oVehicle=vehicleInfo; oDate=serviceDate; oCost=serviceCost; oMonth=month;
    }

    void printReceipt() const
    {
        cout << "\n  ============================================================";
        cout << "\n               SMART SHOWROOM - MAINTENANCE RECEIPT           ";
        cout << "\n  ============================================================";
        cout << "\n  Record ID     : " << recordID;
        cout << "\n  Vehicle ID    : " << vehicleID;
        cout << "\n  Customer      : " << customerName;
        cout << "\n  Phone         : " << customerPhone;
        cout << "\n  CNIC          : " << customerCNIC;
        cout << "\n  Vehicle       : " << vehicleInfo;
        cout << "\n  Service       : " << serviceType;
        cout << "\n  Date          : " << serviceDate;
        cout << "\n  Cost          : " << formatPKR(serviceCost);
        cout << "\n  Technician    : " << technicianName;
        cout << "\n  Notes         : " << notes;
        cout << "\n  Next Service  : " << nextServiceDate;
        cout << "\n  ============================================================\n";
    }

    void displayRecord() const { printReceipt(); }

    int    getRecordID()  const { return recordID; }
    int    getVehicleID() const { return vehicleID; }
    string getCNIC()      const { return customerCNIC; }
    bool   isEmpty()      const { return recordID == 0; }
    float  getCost()      const { return serviceCost; }
    string getMonth()     const { return month; }
    string getDate()      const { return serviceDate; }

    void saveToFile(ofstream &f) const
    {
        f << recordID    << "\n" << vehicleID   << "\n" << serviceCost << "\n";
        wf(f,customerName); wf(f,customerPhone); wf(f,customerCNIC);
        wf(f,vehicleInfo);  wf(f,serviceType);   wf(f,serviceDate);
        wf(f,technicianName); wf(f,notes); wf(f,nextServiceDate); wf(f,month);
    }

    void loadFromFile(ifstream &f)
    {
        f >> recordID; f.ignore();
        f >> vehicleID; f.ignore();
        f >> serviceCost; f.ignore();
        rf(f,customerName); rf(f,customerPhone); rf(f,customerCNIC);
        rf(f,vehicleInfo);  rf(f,serviceType);   rf(f,serviceDate);
        rf(f,technicianName); rf(f,notes); rf(f,nextServiceDate); rf(f,month);
    }
};

// ============================================================
//  SALE RECORD
// ============================================================

class SaleRecord
{
private:
    int    saleID, vehicleID;
    string vehicleInfo, buyerName, buyerPhone, buyerCNIC;
    string saleDate;
    float  salePrice, taxAmount, discount, insuranceAmt, totalBill;
    float  commissionEarned;
    string paymentMethod, paymentRef;
    string month;

public:
    SaleRecord() : saleID(0), vehicleID(0), salePrice(0),
                   taxAmount(0), discount(0), insuranceAmt(0),
                   totalBill(0), commissionEarned(0) {}

    void addSale(int vid, string vinfo, float commPct,
                 string &oName, string &oPhone, string &oCNIC,
                 string &oDate, float &oPrice, string &oMonth)
    {
        vehicleID   = vid;
        vehicleInfo = vinfo;

        saleID       = readInt  ("  Enter Sale ID                  : ");
        buyerName    = readLine ("  Enter Buyer Full Name          : ");
        buyerPhone   = readLine ("  Enter Buyer Phone              : ");
        cout << "\n  [IMPORTANT] Collect Original CNIC from buyer.\n";
        buyerCNIC    = readCNIC ("  Enter Buyer CNIC (13 digits)   : ");
        saleDate     = readDate ("  Enter Sale Date (DD/MM/YYYY)   : ");
        salePrice    = readFloat("  Enter Vehicle Sale Price (PKR) : ");
        taxAmount    = readFloat("  Enter Tax Amount (PKR)         : ");
        discount     = readFloat("  Enter Discount (PKR)           : ");
        insuranceAmt = readFloat("  Enter Insurance Amount (PKR)   : ");

        totalBill = salePrice + taxAmount + insuranceAmt - discount;

        cout << "\n  Payment: 1.Cash  2.Bank Transfer  3.Easypaisa/JazzCash  4.Cheque\n";
        int pc = readInt("  Select: ");
        if      (pc==1) { paymentMethod="Cash"; }
        else if (pc==2) { paymentMethod="Bank Transfer";      paymentRef=readLine("  Transaction Ref  : "); }
        else if (pc==3) { paymentMethod="Easypaisa/JazzCash"; paymentRef=readLine("  Account Number   : "); }
        else            { paymentMethod="Cheque";             paymentRef=readLine("  Cheque Number    : "); }

        commissionEarned = (commPct > 0) ? salePrice * commPct / 100.0f : 0;
        month = saleDate.substr(3, 7);

        printInvoice();

        oName=buyerName; oPhone=buyerPhone; oCNIC=buyerCNIC;
        oDate=saleDate;  oPrice=totalBill;  oMonth=month;
    }

    void printInvoice() const
    {
        cout << "\n  ============================================================";
        cout << "\n          SMART SHOWROOM - OFFICIAL SALE INVOICE              ";
        cout << "\n  ============================================================";
        cout << "\n  Sale ID          : " << saleID;
        cout << "\n  Vehicle ID       : " << vehicleID;
        cout << "\n  Sale Date        : " << saleDate;
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  Buyer Name       : " << buyerName;
        cout << "\n  Buyer Phone      : " << buyerPhone;
        cout << "\n  Buyer CNIC       : " << buyerCNIC;
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  Vehicle          : " << vehicleInfo;
        cout << "\n  Vehicle Price    : " << formatPKR(salePrice);
        cout << "\n  Tax              : " << formatPKR(taxAmount);
        cout << "\n  Insurance        : " << formatPKR(insuranceAmt);
        cout << "\n  Discount (-)     : " << formatPKR(discount);
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  TOTAL BILL       : " << formatPKR(totalBill);
        cout << "\n  ------------------------------------------------------------";
        cout << "\n  Payment Method   : " << paymentMethod;
        if (!paymentRef.empty())
            cout << "\n  Reference No.    : " << paymentRef;
        if (commissionEarned > 0) {
            cout << "\n  ------------------------------------------------------------";
            cout << "\n  Commission Earned: " << formatPKR(commissionEarned);
        }
        cout << "\n  ============================================================\n";
    }

    void displaySale()  const { printInvoice(); }

    int    getSaleID()  const { return saleID; }
    string getCNIC()    const { return buyerCNIC; }
    bool   isEmpty()    const { return saleID == 0; }
    float  getPrice()   const { return totalBill; }
    float  getComm()    const { return commissionEarned; }
    string getMonth()   const { return month; }
    string getDate()    const { return saleDate; }

    void saveToFile(ofstream &f) const
    {
        f << saleID           << "\n" << vehicleID        << "\n"
          << salePrice        << "\n" << taxAmount        << "\n"
          << discount         << "\n" << insuranceAmt     << "\n"
          << totalBill        << "\n" << commissionEarned << "\n";
        wf(f,vehicleInfo); wf(f,buyerName);  wf(f,buyerPhone);
        wf(f,buyerCNIC);   wf(f,saleDate);   wf(f,paymentMethod);
        wf(f,paymentRef);  wf(f,month);
    }

    void loadFromFile(ifstream &f)
    {
        f >> saleID;           f.ignore();
        f >> vehicleID;        f.ignore();
        f >> salePrice;        f.ignore();
        f >> taxAmount;        f.ignore();
        f >> discount;         f.ignore();
        f >> insuranceAmt;     f.ignore();
        f >> totalBill;        f.ignore();
        f >> commissionEarned; f.ignore();
        rf(f,vehicleInfo); rf(f,buyerName);  rf(f,buyerPhone);
        rf(f,buyerCNIC);   rf(f,saleDate);   rf(f,paymentMethod);
        rf(f,paymentRef);  rf(f,month);
    }
};

// ============================================================
//  INSURANCE RECORD  (now saved properly)
// ============================================================

class InsuranceRecord
{
public:
    int    insID;
    string customerName, customerPhone, customerCNIC;
    string insType, company, vehicleInfo, policyNo;
    float  insAmount, premium;
    string startDate, endDate;

    InsuranceRecord() : insID(0), insAmount(0), premium(0) {}

    void addInsurance()
    {
        insID = readInt("  Enter Insurance Record ID             : ");
        cout << "\n  Type:  1.Comprehensive  2.Third Party  3.Fire & Theft\n";
        int it = readInt("  Select: ");
        if      (it==1) insType="Comprehensive";
        else if (it==2) insType="Third Party";
        else            insType="Fire & Theft";

        customerName  = readLine ("  Enter Customer Name               : ");
        customerPhone = readLine ("  Enter Customer Phone              : ");
        cout << "\n  [IMPORTANT] Collect Original CNIC.\n";
        customerCNIC  = readCNIC ("  Enter Customer CNIC (13 digits)   : ");
        company       = readLine ("  Enter Insurance Company           : ");
        vehicleInfo   = readLine ("  Enter Vehicle Info                : ");
        policyNo      = readLine ("  Enter Policy Number               : ");
        insAmount     = readFloat("  Enter Insurance Amount (PKR)      : ");
        premium       = readFloat("  Enter Monthly Premium (PKR)       : ");
        startDate     = readDate ("  Enter Start Date (DD/MM/YYYY)     : ");
        endDate       = readDate ("  Enter End Date   (DD/MM/YYYY)     : ");

        printReceipt();
    }

    void printReceipt() const
    {
        cout << "\n  ============================================================";
        cout << "\n               SMART SHOWROOM - INSURANCE DETAILS             ";
        cout << "\n  ============================================================";
        cout << "\n  Record ID      : " << insID;
        cout << "\n  Customer       : " << customerName;
        cout << "\n  Phone          : " << customerPhone;
        cout << "\n  CNIC           : " << customerCNIC;
        cout << "\n  Insurance Type : " << insType;
        cout << "\n  Company        : " << company;
        cout << "\n  Vehicle        : " << vehicleInfo;
        cout << "\n  Policy No.     : " << policyNo;
        cout << "\n  Insured Amount : " << formatPKR(insAmount);
        cout << "\n  Monthly Premium: " << formatPKR(premium);
        cout << "\n  Start Date     : " << startDate;
        cout << "\n  End Date       : " << endDate;
        cout << "\n  ============================================================\n";
    }

    int    getInsID() const { return insID; }
    string getCNIC()  const { return customerCNIC; }
    bool   isEmpty()  const { return insID == 0; }
    float  getPremium() const { return premium; }
    string getDate()  const { return startDate; }
    string getMonth() const { return startDate.size()>=10 ? startDate.substr(3,7) : ""; }

    void saveToFile(ofstream &f) const
    {
        f << insID << "\n" << insAmount << "\n" << premium << "\n";
        wf(f,customerName); wf(f,customerPhone); wf(f,customerCNIC);
        wf(f,insType);      wf(f,company);       wf(f,vehicleInfo);
        wf(f,policyNo);     wf(f,startDate);     wf(f,endDate);
    }

    void loadFromFile(ifstream &f)
    {
        f >> insID; f.ignore();
        f >> insAmount; f.ignore();
        f >> premium; f.ignore();
        rf(f,customerName); rf(f,customerPhone); rf(f,customerCNIC);
        rf(f,insType);      rf(f,company);       rf(f,vehicleInfo);
        rf(f,policyNo);     rf(f,startDate);     rf(f,endDate);
    }
};

// ============================================================
//  INSTALLMENT
// ============================================================

class Installment
{
public:
    void installmentPlan()
    {
        float totalPrice   = readFloat("  Enter Vehicle Price (PKR)  : ");
        float downPayment  = readFloat("  Enter Down Payment (PKR)   : ");
        int   months       = readInt  ("  Enter Installment Months   : ");
        float interestRate = readFloat("  Enter Interest Rate (%)    : ");

        float remaining    = totalPrice - downPayment;
        float interest     = remaining * interestRate / 100.0f;
        float totalPayable = remaining + interest;
        float monthly      = totalPayable / months;

        cout << "\n  ============================================================";
        cout << "\n             INSTALLMENT PLAN CALCULATOR                      ";
        cout << "\n  ============================================================";
        cout << "\n  Vehicle Price       : " << formatPKR(totalPrice);
        cout << "\n  Down Payment        : " << formatPKR(downPayment);
        cout << "\n  Remaining Amount    : " << formatPKR(remaining);
        cout << "\n  Interest (" << interestRate << "%)      : " << formatPKR(interest);
        cout << "\n  Total Payable       : " << formatPKR(totalPayable);
        cout << "\n  Monthly Payment     : " << formatPKR(monthly);
        cout << "\n  Duration            : " << months << " months";
        cout << "\n  ============================================================\n";
    }
};

// ============================================================
//  GLOBAL ARRAYS
// ============================================================

Car              cars[MAX_VEHICLES];
Bike             bikes[MAX_VEHICLES];
CustomerRecord   customers[MAX_CUSTOMERS];
MaintenanceRecord mainRecords[MAX_RECORDS];
SaleRecord       saleRecords[MAX_RECORDS];
RentalRecord     rentals[MAX_RENTALS];
InsuranceRecord  insRecords[MAX_INSURANCE];

int totalCars=0, totalBikes=0, totalCustomers=0;
int totalMaintRecs=0, totalSaleRecs=0, totalRentals=0, totalInsRecs=0;

// ============================================================
//  AUTO REGISTER CUSTOMER
// ============================================================

void registerCustomer(string name, string phone, string cnic,
                      string activity, string vehicleInfo,
                      string date, float amount)
{
    if (totalCustomers >= MAX_CUSTOMERS) {
        cout << "  [!] Customer record limit reached!\n"; return;
    }
    CustomerRecord &c = customers[totalCustomers++];
    c.customerID  = totalCustomers;
    c.name        = name;
    c.phone       = phone;
    c.cnic        = cnic;
    c.activity    = activity;
    c.vehicleInfo = vehicleInfo;
    c.date        = date;
    c.amountPaid  = amount;
    cout << "  [System] Customer auto-registered. ID: " << c.customerID << "\n";
}

// ============================================================
//  HARDCODED DEFAULT VEHICLES
// ============================================================

void loadDefaultVehicles()
{
    cars[0].setCar(101,"Toyota","Corolla","White","2022",4500000,"OUR_SALE","Automatic",4,"Petrol");
    cars[1].setCar(102,"Honda","Civic","Black","2023",5800000,"OUR_SALE","Automatic",4,"Petrol");
    cars[2].setCar(103,"Suzuki","Alto","Silver","2023",2200000,"OUR_SALE","Manual",4,"Petrol");
    cars[3].setCar(104,"Toyota","Fortuner","Grey","2022",9500000,"OUR_SALE","Automatic",4,"Diesel");
    cars[4].setCar(105,"Kia","Sportage","Red","2023",7200000,"OUR_SALE","Automatic",4,"Petrol");
    totalCars = 5;
    cars[5].setCar(201,"Toyota","Corolla","White","2021",3800000,"OUR_RENT","Automatic",4,"Petrol");
    cars[6].setCar(202,"Honda","BRV","Blue","2022",5500000,"OUR_RENT","Automatic",4,"Petrol");
    cars[7].setCar(203,"Suzuki","Cultus","Silver","2020",2000000,"OUR_RENT","Manual",4,"Petrol");
    totalCars = 8;
    cars[8].setCar(501,"Hyundai","Tucson","White","2022",8500000,"COMMISSION","Automatic",4,"Petrol","Available","Ali Raza",5.0f);
    cars[9].setCar(502,"MG","HS","Grey","2023",7900000,"COMMISSION","Automatic",4,"Petrol","Available","Ahmed Khan",4.0f);
    totalCars = 10;

    bikes[0].setBike(301,"Honda","CB150F","Red","2023",380000,"OUR_SALE",150,"Standard");
    bikes[1].setBike(302,"Yamaha","YBR125","Black","2023",290000,"OUR_SALE",125,"Standard");
    bikes[2].setBike(303,"Honda","CD70","Green","2022",165000,"OUR_SALE",70,"Standard");
    bikes[3].setBike(304,"Suzuki","GS150","Blue","2023",320000,"OUR_SALE",150,"Standard");
    totalBikes = 4;
    bikes[4].setBike(401,"Honda","CB150F","Black","2022",370000,"OUR_RENT",150,"Standard");
    bikes[5].setBike(402,"Yamaha","YBR125","Blue","2021",270000,"OUR_RENT",125,"Standard");
    bikes[6].setBike(601,"Honda","CB500F","Red","2022",950000,"COMMISSION",500,"Sports","Available","Bilal Sheikh",6.0f);
    totalBikes = 7;

    cout << "\n  [System] Default vehicles loaded: " << totalCars << " Cars, " << totalBikes << " Bikes.\n";
}

// ============================================================
//  SAVE ALL DATA
// ============================================================

void saveAllData()
{
    ofstream f("ShowroomData.txt");
    if (!f) { cout << "\n  [!] Error saving file!\n"; return; }

    f << totalCars << "\n";
    for (int i=0;i<totalCars;i++)  cars[i].saveToFile(f);

    f << totalBikes << "\n";
    for (int i=0;i<totalBikes;i++) bikes[i].saveToFile(f);

    f << totalCustomers << "\n";
    for (int i=0;i<totalCustomers;i++) customers[i].saveToFile(f);

    f << totalMaintRecs << "\n";
    for (int i=0;i<totalMaintRecs;i++) mainRecords[i].saveToFile(f);

    f << totalSaleRecs << "\n";
    for (int i=0;i<totalSaleRecs;i++) saleRecords[i].saveToFile(f);

    f << totalRentals << "\n";
    for (int i=0;i<totalRentals;i++) rentals[i].saveToFile(f);

    f << totalInsRecs << "\n";
    for (int i=0;i<totalInsRecs;i++) insRecords[i].saveToFile(f);

    f.close();
    cout << "\n  [System] All data saved to ShowroomData.txt\n";
}

// ============================================================
//  LOAD ALL DATA
// ============================================================

void loadAllData()
{
    ifstream f("ShowroomData.txt");
    if (!f) { cout << "\n  [Info] No saved data found. Starting fresh.\n"; return; }

    string tag;

    f >> totalCars; f.ignore();
    for (int i=0;i<totalCars;i++) {
        f >> tag; f.ignore();
        if (tag=="CAR")  cars[i].loadFromFile(f);
    }

    f >> totalBikes; f.ignore();
    for (int i=0;i<totalBikes;i++) {
        f >> tag; f.ignore();
        if (tag=="BIKE") bikes[i].loadFromFile(f);
    }

    f >> totalCustomers; f.ignore();
    for (int i=0;i<totalCustomers;i++) customers[i].loadFromFile(f);

    f >> totalMaintRecs; f.ignore();
    for (int i=0;i<totalMaintRecs;i++) mainRecords[i].loadFromFile(f);

    f >> totalSaleRecs; f.ignore();
    for (int i=0;i<totalSaleRecs;i++) saleRecords[i].loadFromFile(f);

    f >> totalRentals; f.ignore();
    for (int i=0;i<totalRentals;i++) rentals[i].loadFromFile(f);

    f >> totalInsRecs; f.ignore();
    for (int i=0;i<totalInsRecs;i++) insRecords[i].loadFromFile(f);

    f.close();
    cout << "\n  [System] Data loaded: "
         << totalCars << " Cars, " << totalBikes << " Bikes, "
         << totalCustomers << " Customers, " << totalMaintRecs << " Maintenance, "
         << totalSaleRecs << " Sales, " << totalRentals << " Rentals, "
         << totalInsRecs << " Insurance.\n";
}

// ============================================================
//  VIEW ALL VEHICLES
// ============================================================

void viewAllVehicles()
{
    auto section = [](const string &t){ cout << "\n  ===== " << t << " ====="; };

    section("OUR CARS FOR SALE");
    bool any=false;
    for(int i=0;i<totalCars;i++) if(cars[i].getOwnership()=="OUR_SALE"){cars[i].displayCar();any=true;}
    if(!any) cout<<"\n  None.\n";

    section("OUR CARS FOR RENT");
    any=false;
    for(int i=0;i<totalCars;i++) if(cars[i].getOwnership()=="OUR_RENT"){cars[i].displayCar();any=true;}
    if(!any) cout<<"\n  None.\n";

    section("COMMISSION CARS");
    any=false;
    for(int i=0;i<totalCars;i++) if(cars[i].getOwnership()=="COMMISSION"){cars[i].displayCar();any=true;}
    if(!any) cout<<"\n  None.\n";

    section("OUR BIKES FOR SALE");
    any=false;
    for(int i=0;i<totalBikes;i++) if(bikes[i].getOwnership()=="OUR_SALE"){bikes[i].displayBike();any=true;}
    if(!any) cout<<"\n  None.\n";

    section("OUR BIKES FOR RENT");
    any=false;
    for(int i=0;i<totalBikes;i++) if(bikes[i].getOwnership()=="OUR_RENT"){bikes[i].displayBike();any=true;}
    if(!any) cout<<"\n  None.\n";

    section("COMMISSION BIKES");
    any=false;
    for(int i=0;i<totalBikes;i++) if(bikes[i].getOwnership()=="COMMISSION"){bikes[i].displayBike();any=true;}
    if(!any) cout<<"\n  None.\n";
}

// ============================================================
//  SEARCH VEHICLE
// ============================================================

void searchVehicle()
{
    cout << "\n  Search By:  1. Vehicle ID   2. Brand Name\n";
    int choice = readInt("  Select: ");
    if (choice==1) {
        int id = readInt("  Enter Vehicle ID: ");
        bool found=false;
        for(int i=0;i<totalCars;i++)  if(cars[i].getID()==id) {cars[i].displayCar();found=true;}
        for(int i=0;i<totalBikes;i++) if(bikes[i].getID()==id){bikes[i].displayBike();found=true;}
        if(!found) cout<<"\n  Vehicle Not Found!\n";
    } else {
        string brand = readLine("  Enter Brand: ");
        bool found=false;
        for(int i=0;i<totalCars;i++)  if(cars[i].getBrand()==brand) {cars[i].displayCar();found=true;}
        for(int i=0;i<totalBikes;i++) if(bikes[i].getBrand()==brand){bikes[i].displayBike();found=true;}
        if(!found) cout<<"\n  No Vehicle Found!\n";
    }
}

// ============================================================
//  EDIT VEHICLE
// ============================================================

void editVehicleMenu()
{
    int vid = readInt("\n  Enter Vehicle ID to Edit: ");
    for(int i=0;i<totalCars;i++)
        if(cars[i].getID()==vid) { cout<<"\n  Editing Car:\n"; cars[i].editCar(); return; }
    for(int i=0;i<totalBikes;i++)
        if(bikes[i].getID()==vid) { cout<<"\n  Editing Bike:\n"; bikes[i].editBike(); return; }
    cout<<"\n  Vehicle ID not found!\n";
}

// ============================================================
//  SELL A VEHICLE
// ============================================================

void sellVehicle()
{
    cout << "\n  ============================================================";
    cout << "\n              VEHICLES AVAILABLE FOR SALE                     ";
    cout << "\n  ============================================================";
    int count=0;
    cout << "\n  --- CARS ---";
    for(int i=0;i<totalCars;i++)
        if((cars[i].getOwnership()=="OUR_SALE"||cars[i].getOwnership()=="COMMISSION")
            && cars[i].getStatus()=="Available") { cars[i].displayCar(); count++; }
    cout << "\n  --- BIKES ---";
    for(int i=0;i<totalBikes;i++)
        if((bikes[i].getOwnership()=="OUR_SALE"||bikes[i].getOwnership()=="COMMISSION")
            && bikes[i].getStatus()=="Available") { bikes[i].displayBike(); count++; }

    if (count==0) { cout<<"\n  No vehicles available for sale.\n"; return; }
    if (totalSaleRecs>=MAX_RECORDS) { cout<<"\n  Sale record limit reached!\n"; return; }

    cout << "\n  ============================================================";
    cout << "\n                    VEHICLE SALE FORM                         ";
    cout << "\n  ============================================================\n";

    int vid = readInt("  Enter Vehicle ID to Purchase    : ");

    float  vPrice=0, vComm=0;
    string vInfo="";

    for(int i=0;i<totalCars;i++)
        if(cars[i].getID()==vid) {
            if(cars[i].getStatus()!="Available") { cout<<"\n  [!] Not available.\n"; return; }
            if(cars[i].getOwnership()=="OUR_RENT") { cout<<"\n  [!] Rent-only vehicle.\n"; return; }
            vPrice=cars[i].getPrice(); vComm=cars[i].getCommission();
            vInfo=cars[i].getBrand()+" "+cars[i].getModel(); break;
        }
    for(int i=0;i<totalBikes;i++)
        if(bikes[i].getID()==vid) {
            if(bikes[i].getStatus()!="Available") { cout<<"\n  [!] Not available.\n"; return; }
            if(bikes[i].getOwnership()=="OUR_RENT") { cout<<"\n  [!] Rent-only vehicle.\n"; return; }
            vPrice=bikes[i].getPrice(); vComm=bikes[i].getCommission();
            vInfo=bikes[i].getBrand()+" "+bikes[i].getModel(); break;
        }

    if (vInfo.empty()) { cout<<"\n  [!] Vehicle ID not found.\n"; return; }

    cout << "\n  Vehicle Selected : " << vInfo;
    cout << "\n  Listed Price     : " << formatPKR(vPrice) << "\n\n";

    string oName,oPhone,oCNIC,oDate,oMonth; float oPrice;
    saleRecords[totalSaleRecs].addSale(vid,vInfo,vComm,oName,oPhone,oCNIC,oDate,oPrice,oMonth);

    for(int i=0;i<totalCars;i++)  if(cars[i].getID()==vid)  { cars[i].setStatus("Sold");  break; }
    for(int i=0;i<totalBikes;i++) if(bikes[i].getID()==vid) { bikes[i].setStatus("Sold"); break; }

    totalSaleRecs++;
    registerCustomer(oName,oPhone,oCNIC,"SALE",vInfo,oDate,oPrice);
    cout << "\n  [System] " << vInfo << " marked as Sold.\n";
}

// ============================================================
//  RENT A VEHICLE
// ============================================================

void rentVehicle()
{
    cout << "\n  ============================================================";
    cout << "\n              VEHICLES AVAILABLE FOR RENT                     ";
    cout << "\n  ============================================================";
    int count=0;
    cout << "\n  --- CARS ---";
    for(int i=0;i<totalCars;i++)
        if((cars[i].getOwnership()=="OUR_RENT"||cars[i].getOwnership()=="COMMISSION")
            && cars[i].getStatus()=="Available") { cars[i].displayCar(); count++; }
    cout << "\n  --- BIKES ---";
    for(int i=0;i<totalBikes;i++)
        if((bikes[i].getOwnership()=="OUR_RENT"||bikes[i].getOwnership()=="COMMISSION")
            && bikes[i].getStatus()=="Available") { bikes[i].displayBike(); count++; }

    if (count==0) { cout<<"\n  No vehicles available for rent.\n"; return; }
    if (totalRentals>=MAX_RENTALS) { cout<<"\n  Rental limit reached!\n"; return; }

    cout << "\n  ============================================================";
    cout << "\n                       RENTAL FORM                            ";
    cout << "\n  ============================================================\n";

    string custName  = readLine ("  Enter Customer Full Name        : ");
    string custPhone = readLine ("  Enter Customer Phone            : ");
    cout << "\n  [IMPORTANT] Collect Original CNIC before handing keys.\n";
    string custCNIC  = readCNIC ("  Enter Customer CNIC (13 digits) : ");

    int vid = readInt("  Enter Vehicle ID to Rent        : ");

    float vPrice=0; string vInfo="";
    for(int i=0;i<totalCars;i++)
        if(cars[i].getID()==vid) {
            if(cars[i].getStatus()!="Available") { cout<<"\n  [!] Not available!\n"; return; }
            vPrice=cars[i].getPrice();
            vInfo=cars[i].getBrand()+" "+cars[i].getModel();
            cars[i].setStatus("Rented"); break;
        }
    for(int i=0;i<totalBikes;i++)
        if(bikes[i].getID()==vid) {
            if(bikes[i].getStatus()!="Available") { cout<<"\n  [!] Not available!\n"; return; }
            vPrice=bikes[i].getPrice();
            vInfo=bikes[i].getBrand()+" "+bikes[i].getModel();
            bikes[i].setStatus("Rented"); break;
        }
    if (vInfo.empty()) { cout<<"\n  [!] Vehicle ID not found!\n"; return; }

    cout << "\n  Driver Option:  1. Without Driver   2. With Driver\n";
    int dc = readInt("  Select: ");
    float driverPerDay = 0;
    if (dc==2) driverPerDay = readFloat("  Driver Charge Per Day (PKR): ");

    int   days       = readInt  ("  Number of Days              : ");
    float rentPerDay = readFloat("  Rent Per Day (PKR)          : ");

    // Security deposit = 1/8 of vehicle price
    float secDeposit = vPrice / 8.0f;
    cout << "\n  ----------------------------------------------------------";
    cout << "\n  Vehicle Price          : " << formatPKR(vPrice);
    cout << "\n  Security Deposit (1/8) : " << formatPKR(secDeposit);
    cout << "\n  [FRONT DESK] Collect   : " << formatPKR(secDeposit) << " before releasing vehicle.";
    cout << "\n  ----------------------------------------------------------\n";

    string startDate = readDate("  Start Date (DD/MM/YYYY)     : ");

    float rentTotal   = days * rentPerDay;
    float driverTotal = days * driverPerDay;
    float grandTotal  = rentTotal + driverTotal;

    RentalRecord &rr   = rentals[totalRentals++];
    rr.rentalID        = totalRentals;
    rr.vehicleID       = vid;
    rr.vehicleInfo     = vInfo;
    rr.customerName    = custName;
    rr.customerPhone   = custPhone;
    rr.customerCNIC    = custCNIC;
    rr.startDate       = startDate;
    rr.agreedDays      = days;
    rr.rentPerDay      = rentPerDay;
    rr.driverPerDay    = driverPerDay;
    rr.securityDeposit = secDeposit;
    rr.withDriver      = (dc==2);
    rr.returned        = false;
    rr.totalCharged    = grandTotal;

    rr.printBill();
    registerCustomer(custName,custPhone,custCNIC,"RENT",vInfo,startDate,grandTotal);
}

// ============================================================
//  RETURN VEHICLE
// ============================================================

void returnVehicle()
{
    cout << "\n  ===== ACTIVE RENTALS =====\n";
    bool any=false;
    for(int i=0;i<totalRentals;i++)
        if(!rentals[i].returned) { rentals[i].printBill(); any=true; }
    if(!any) { cout<<"\n  No active rentals.\n"; return; }

    int rid = readInt("\n  Enter Rental ID to Return: ");
    RentalRecord *rr=nullptr;
    for(int i=0;i<totalRentals;i++)
        if(rentals[i].rentalID==rid && !rentals[i].returned) { rr=&rentals[i]; break; }
    if(!rr) { cout<<"\n  [!] Active rental not found.\n"; return; }

    string returnDate = readDate("  Return Date (DD/MM/YYYY)       : ");
    int    actualDays = readInt ("  Actual Days Used               : ");

    float rentCost   = actualDays * rr->rentPerDay;
    float driverCost = actualDays * rr->driverPerDay;
    float totalDue   = rentCost + driverCost;
    float lateFee=0, refund=0;

    if (actualDays > rr->agreedDays)
    {
        int extra = actualDays - rr->agreedDays;
        float feePerDay = readFloat("  Late Fee Per Extra Day (PKR)   : ");
        lateFee  = feePerDay * extra;
        totalDue += lateFee;
        float remaining = rr->securityDeposit - lateFee;
        refund = (remaining > 0) ? remaining : 0;
        if (remaining < 0)
            cout << "\n  [!] Late fee exceeds deposit. Extra to collect: " << formatPKR(-remaining) << "\n";
    }
    else
    {
        refund  = rr->securityDeposit; // Full refund — on time
        lateFee = 0;
    }

    // Mark vehicle available again
    for(int i=0;i<totalCars;i++)  if(cars[i].getID()==rr->vehicleID)  { cars[i].setStatus("Available");  break; }
    for(int i=0;i<totalBikes;i++) if(bikes[i].getID()==rr->vehicleID) { bikes[i].setStatus("Available"); break; }

    rr->returned     = true;
    rr->returnDate   = returnDate;
    rr->lateFee      = lateFee;
    rr->totalCharged = totalDue;
    rr->refundGiven  = refund;

    rr->printReturnSettlement();
    registerCustomer(rr->customerName,rr->customerPhone,rr->customerCNIC,
                     "RENT_RETURN",rr->vehicleInfo,returnDate,refund);
}

// ============================================================
//  MAINTENANCE
// ============================================================

void viewMaintenanceHistory()
{
    cout << "\n  1. View All   2. Search by Vehicle ID\n";
    int c = readInt("  Select: ");
    if (c==1) {
        cout << "\n  ===== MAINTENANCE RECORDS (" << totalMaintRecs << ") =====\n";
        if(!totalMaintRecs) { cout<<"  No records yet.\n"; return; }
        for(int i=0;i<totalMaintRecs;i++) mainRecords[i].printReceipt();
    } else {
        int vid = readInt("  Enter Vehicle ID: ");
        bool found=false; float total=0;
        for(int i=0;i<totalMaintRecs;i++)
            if(mainRecords[i].getVehicleID()==vid) {
                mainRecords[i].printReceipt(); total+=mainRecords[i].getCost(); found=true;
            }
        if(!found) cout<<"\n  No records for this vehicle.\n";
        else cout<<"\n  Total Maintenance Cost: " << formatPKR(total) << "\n";
    }
}

// ============================================================
//  REVENUE REPORT  — FIX: Rentals use returnDate for monthly
// ============================================================

void revenueReport()
{
    cout << "\n  Revenue Report By:  1. Monthly   2. Daily\n";
    int rtype = readInt("  Select: ");

    string filter;
    if (rtype==1) filter = readLine("  Enter Month (MM/YYYY, e.g. 06/2025): ");
    else          filter = readDate("  Enter Date  (DD/MM/YYYY)            : ");

    float saleRev=0, commRev=0, maintRev=0, rentRev=0, insRev=0;
    int   saleCnt=0, maintCnt=0, rentCnt=0, insCnt=0;

    for(int i=0;i<totalSaleRecs;i++) {
        bool m = (rtype==1) ? (saleRecords[i].getMonth()==filter)
                            : (saleRecords[i].getDate()==filter);
        if(m) { saleRev+=saleRecords[i].getPrice(); commRev+=saleRecords[i].getComm(); saleCnt++; }
    }
    for(int i=0;i<totalMaintRecs;i++) {
        bool m = (rtype==1) ? (mainRecords[i].getMonth()==filter)
                            : (mainRecords[i].getDate()==filter);
        if(m) { maintRev+=mainRecords[i].getCost(); maintCnt++; }
    }
    // FIX: Rentals count by RETURN date (when money is actually settled)
    //      Active (unreturned) rentals are counted by startDate
    for(int i=0;i<totalRentals;i++) {
        string useDate = rentals[i].returned ? rentals[i].returnDate : rentals[i].startDate;
        string useMonth = useDate.size()>=10 ? useDate.substr(3,7) : "";
        bool m = (rtype==1) ? (useMonth==filter) : (useDate==filter);
        if(m) { rentRev+=rentals[i].totalCharged; rentCnt++; }
    }
    for(int i=0;i<totalInsRecs;i++) {
        bool m = (rtype==1) ? (insRecords[i].getMonth()==filter)
                            : (insRecords[i].getDate()==filter);
        if(m) { insRev+=insRecords[i].getPremium(); insCnt++; }
    }

    float grandTotal = saleRev + commRev + maintRev + rentRev + insRev;
    string label = (rtype==1) ? "MONTHLY" : "DAILY";

    cout << "\n  ============================================================";
    cout << "\n        " << label << " REVENUE REPORT  [" << filter << "]";
    cout << "\n  ============================================================";
    cout << "\n  SALES (" << saleCnt << " transactions)";
    cout << "\n    Sale Revenue         : " << formatPKR(saleRev);
    cout << "\n    Commission Earned    : " << formatPKR(commRev);
    cout << "\n  MAINTENANCE (" << maintCnt << " jobs)";
    cout << "\n    Service Revenue      : " << formatPKR(maintRev);
    cout << "\n  RENTALS (" << rentCnt << " bookings)";
    cout << "\n    Rental Revenue       : " << formatPKR(rentRev);
    cout << "\n  INSURANCE (" << insCnt << " policies)";
    cout << "\n    Premium Revenue      : " << formatPKR(insRev);
    cout << "\n  ------------------------------------------------------------";
    cout << "\n  TOTAL REVENUE          : " << formatPKR(grandTotal);
    cout << "\n  ============================================================\n";
    if (grandTotal==0) cout << "  No transactions found for [" << filter << "]\n";
}

// ============================================================
//  SHOWROOM STATS
// ============================================================

void showroomStats()
{
    int osc=0,orc=0,cc=0,sc=0,rc=0;
    int osb=0,orb=0,cb=0,sb=0;
    for(int i=0;i<totalCars;i++) {
        if(cars[i].getOwnership()=="OUR_SALE") osc++;
        else if(cars[i].getOwnership()=="OUR_RENT") orc++;
        else cc++;
        if(cars[i].getStatus()=="Sold")   sc++;
        if(cars[i].getStatus()=="Rented") rc++;
    }
    for(int i=0;i<totalBikes;i++) {
        if(bikes[i].getOwnership()=="OUR_SALE") osb++;
        else if(bikes[i].getOwnership()=="OUR_RENT") orb++;
        else cb++;
        if(bikes[i].getStatus()=="Sold") sb++;
    }
    float mc=0; for(int i=0;i<totalMaintRecs;i++) mc+=mainRecords[i].getCost();
    int ar=0;   for(int i=0;i<totalRentals;i++) if(!rentals[i].returned) ar++;

    cout << "\n  ============================================================";
    cout << "\n                  SHOWROOM STATISTICS                         ";
    cout << "\n  ============================================================";
    cout << "\n  CARS  (Total: " << totalCars << ")";
    cout << "\n    Our Sale Cars     : " << osc;
    cout << "\n    Our Rent Cars     : " << orc;
    cout << "\n    Commission Cars   : " << cc;
    cout << "\n    Sold              : " << sc;
    cout << "\n    Currently Rented  : " << rc;
    cout << "\n  BIKES (Total: " << totalBikes << ")";
    cout << "\n    Our Sale Bikes    : " << osb;
    cout << "\n    Our Rent Bikes    : " << orb;
    cout << "\n    Commission Bikes  : " << cb;
    cout << "\n    Sold              : " << sb;
    cout << "\n  CUSTOMERS (Auto)    : " << totalCustomers;
    cout << "\n  MAINTENANCE Records : " << totalMaintRecs;
    cout << "\n  Total Maint. Cost   : " << formatPKR(mc);
    cout << "\n  SALE Records        : " << totalSaleRecs;
    cout << "\n  RENTAL Records      : " << totalRentals;
    cout << "\n  Active Rentals Now  : " << ar;
    cout << "\n  INSURANCE Records   : " << totalInsRecs;
    cout << "\n  ============================================================\n";
}

// ============================================================
//  REPRINT RECEIPT  — covers all transaction types
// ============================================================

void reprintReceipt()
{
    cout << "\n  ============================================================";
    cout << "\n                  REPRINT / VIEW RECEIPT                      ";
    cout << "\n  ============================================================";
    cout << "\n  1. Sale Invoice";
    cout << "\n  2. Rental Bill";
    cout << "\n  3. Rental Return Settlement";
    cout << "\n  4. Maintenance Receipt";
    cout << "\n  5. Insurance Details";
    cout << "\n  ============================================================\n";
    int rtype = readInt("  Select Type: ");

    cout << "\n  Search By:  1. Record / Transaction ID   2. Customer CNIC\n";
    int stype = readInt("  Select: ");

    if (rtype==1) // SALE INVOICE
    {
        if (stype==1) {
            int id = readInt("  Enter Sale ID: ");
            bool f=false;
            for(int i=0;i<totalSaleRecs;i++) if(saleRecords[i].getSaleID()==id){saleRecords[i].printInvoice();f=true;}
            if(!f) cout<<"\n  Sale ID not found.\n";
        } else {
            string cnic = readCNIC("  Enter Customer CNIC: ");
            bool f=false;
            for(int i=0;i<totalSaleRecs;i++) if(saleRecords[i].getCNIC()==cnic){saleRecords[i].printInvoice();f=true;}
            if(!f) cout<<"\n  No sale found for this CNIC.\n";
        }
    }
    else if (rtype==2) // RENTAL BILL
    {
        if (stype==1) {
            int id = readInt("  Enter Rental ID: ");
            bool f=false;
            for(int i=0;i<totalRentals;i++) if(rentals[i].rentalID==id){rentals[i].printBill();f=true;}
            if(!f) cout<<"\n  Rental ID not found.\n";
        } else {
            string cnic = readCNIC("  Enter Customer CNIC: ");
            bool f=false;
            for(int i=0;i<totalRentals;i++) if(rentals[i].customerCNIC==cnic){rentals[i].printBill();f=true;}
            if(!f) cout<<"\n  No rental found for this CNIC.\n";
        }
    }
    else if (rtype==3) // RENTAL RETURN SETTLEMENT
    {
        if (stype==1) {
            int id = readInt("  Enter Rental ID: ");
            bool f=false;
            for(int i=0;i<totalRentals;i++)
                if(rentals[i].rentalID==id && rentals[i].returned){rentals[i].printReturnSettlement();f=true;}
            if(!f) cout<<"\n  Returned rental not found with that ID.\n";
        } else {
            string cnic = readCNIC("  Enter Customer CNIC: ");
            bool f=false;
            for(int i=0;i<totalRentals;i++)
                if(rentals[i].customerCNIC==cnic && rentals[i].returned){rentals[i].printReturnSettlement();f=true;}
            if(!f) cout<<"\n  No return record found for this CNIC.\n";
        }
    }
    else if (rtype==4) // MAINTENANCE RECEIPT
    {
        if (stype==1) {
            int id = readInt("  Enter Record ID: ");
            bool f=false;
            for(int i=0;i<totalMaintRecs;i++) if(mainRecords[i].getRecordID()==id){mainRecords[i].printReceipt();f=true;}
            if(!f) cout<<"\n  Record ID not found.\n";
        } else {
            string cnic = readCNIC("  Enter Customer CNIC: ");
            bool f=false;
            for(int i=0;i<totalMaintRecs;i++) if(mainRecords[i].getCNIC()==cnic){mainRecords[i].printReceipt();f=true;}
            if(!f) cout<<"\n  No maintenance record for this CNIC.\n";
        }
    }
    else if (rtype==5) // INSURANCE
    {
        if (stype==1) {
            int id = readInt("  Enter Insurance Record ID: ");
            bool f=false;
            for(int i=0;i<totalInsRecs;i++) if(insRecords[i].getInsID()==id){insRecords[i].printReceipt();f=true;}
            if(!f) cout<<"\n  Insurance record not found.\n";
        } else {
            string cnic = readCNIC("  Enter Customer CNIC: ");
            bool f=false;
            for(int i=0;i<totalInsRecs;i++) if(insRecords[i].getCNIC()==cnic){insRecords[i].printReceipt();f=true;}
            if(!f) cout<<"\n  No insurance record for this CNIC.\n";
        }
    }
    else { cout<<"\n  Invalid type.\n"; }
}

// ============================================================
//  VIEW CUSTOMERS
// ============================================================

void viewAllCustomers()
{
    cout << "\n  ===== AUTO-REGISTERED CUSTOMERS (" << totalCustomers << ") =====\n";
    if(!totalCustomers) { cout<<"  No customers yet.\n"; return; }
    for(int i=0;i<totalCustomers;i++) customers[i].display();
}

// ============================================================
//  MAIN
// ============================================================

int main()
{
    Installment installment;

    loadAllData();
    if (totalCars==0 && totalBikes==0)
        loadDefaultVehicles();

    int choice;
    do
    {
        cout << "\n\n  =====================================================";
        cout << "\n     SMART SHOWROOM + MAINTENANCE MANAGEMENT SYSTEM   ";
        cout << "\n  =====================================================";
        cout << "\n   VEHICLES";
        cout << "\n    1.  Add Car    (Manual / Commission)";
        cout << "\n    2.  Add Bike   (Manual / Commission)";
        cout << "\n    3.  View All Vehicles";
        cout << "\n    4.  Search Vehicle";
        cout << "\n    5.  Edit / Update Vehicle";
        cout << "\n   SALES & RENTAL";
        cout << "\n    6.  Sell a Vehicle";
        cout << "\n    7.  Rent a Vehicle";
        cout << "\n    8.  Return Vehicle (Settlement)";
        cout << "\n   CUSTOMERS  (Auto-Registered)";
        cout << "\n    9.  View All Customers";
        cout << "\n   MAINTENANCE";
        cout << "\n    10. Add Maintenance Record";
        cout << "\n    11. View Maintenance History";
        cout << "\n   FINANCE & SERVICES";
        cout << "\n    12. Installment Plan Calculator";
        cout << "\n    13. Insurance Details";
        cout << "\n   REPORTS & DATA";
        cout << "\n    14. Showroom Statistics";
        cout << "\n    15. Daily / Monthly Revenue Report";
        cout << "\n    16. Reprint / View Receipt";
        cout << "\n    17. Save Data to File";
        cout << "\n    18. Exit";
        cout << "\n  =====================================================";
        cout << "\n   Enter Choice: ";

        choice = readInt("");

        switch(choice)
        {
        case 1:
            if(totalCars>=MAX_VEHICLES){cout<<"\n  Car limit reached!\n";break;}
            cars[totalCars].addCarManual();
            cars[totalCars].displayCar();
            totalCars++;
            cout<<"\n  Car added! Total: "<<totalCars<<"\n"; break;

        case 2:
            if(totalBikes>=MAX_VEHICLES){cout<<"\n  Bike limit reached!\n";break;}
            bikes[totalBikes].addBikeManual();
            bikes[totalBikes].displayBike();
            totalBikes++;
            cout<<"\n  Bike added! Total: "<<totalBikes<<"\n"; break;

        case 3:  viewAllVehicles();     break;
        case 4:  searchVehicle();       break;
        case 5:  editVehicleMenu();     break;
        case 6:  sellVehicle();         break;
        case 7:  rentVehicle();         break;
        case 8:  returnVehicle();       break;
        case 9:  viewAllCustomers();    break;

        case 10:
        {
            if(totalMaintRecs>=MAX_RECORDS){cout<<"\n  Record limit reached!\n";break;}
            string n,ph,cn,vi,dt,mo; float cost;
            mainRecords[totalMaintRecs].addRecord(n,ph,cn,vi,dt,cost,mo);
            mainRecords[totalMaintRecs].printReceipt();
            totalMaintRecs++;
            registerCustomer(n,ph,cn,"MAINTENANCE",vi,dt,cost);
            cout<<"\n  Maintenance record added!\n"; break;
        }

        case 11: viewMaintenanceHistory(); break;
        case 12: installment.installmentPlan(); break;

        case 13:
        {
            if(totalInsRecs>=MAX_INSURANCE){cout<<"\n  Insurance limit reached!\n";break;}
            insRecords[totalInsRecs].addInsurance();
            registerCustomer(
                insRecords[totalInsRecs].customerName,
                insRecords[totalInsRecs].customerPhone,
                insRecords[totalInsRecs].customerCNIC,
                "INSURANCE",
                insRecords[totalInsRecs].vehicleInfo,
                insRecords[totalInsRecs].startDate,
                insRecords[totalInsRecs].getPremium()
            );
            totalInsRecs++;
            break;
        }

        case 14: showroomStats();       break;
        case 15: revenueReport();       break;
        case 16: reprintReceipt();      break;
        case 17: saveAllData();         break;
        case 18: cout<<"\n  Exiting... Goodbye!\n"; break;
        default: cout<<"\n  [!] Invalid choice. Enter 1-18.\n";
        }

    } while (choice != 18);

    return 0;
}
