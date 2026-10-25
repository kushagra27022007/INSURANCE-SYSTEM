#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;

struct Nominee {
    string name;
    string relation;
};

inline double addGst(double amount) {
    return amount * 1.18;
}

template <class T>
T bigger(T a, T b) {
    return (a > b) ? a : b;
}

class Customer {
private:
    int id;
    string name;
    int age;
    Nominee nominee;
    static int count;

public:
    Customer() {
        id = 0;
        name = "";
        age = 0;
    }

    void setData(string n, int a, string nomName, string rel);
    void show() const;

    int getId() const { return id; }
    int getAge() const { return age; }
    static int getCount() { return count; }
};

int Customer::count = 0;

void Customer::setData(string n, int a, string nomName, string rel) {
    if (a < 18 || a > 65)
        throw runtime_error("Age must be between 18 and 65");
    this->name = n;
    this->age = a;
    nominee.name = nomName;
    nominee.relation = rel;
    id = 100 + (++count);
}

void Customer::show() const {
    cout << id << "  " << name << "  Age: " << age
         << "  Nominee: " << nominee.name << " (" << nominee.relation << ")" << endl;
}

// base class, cant make object of it because of the pure virtual functions
class Policy {
protected:
    const int policyNo;
    int custId;
    double sumAssured;
    int years;
    double claimed;
    static int total;

public:
    Policy(int c, double s, int y = 5);
    virtual ~Policy() {}

    virtual double premium() const = 0;
    virtual string type() const = 0;

    void show() const;
    double discount(int claimFreeYears) const;
    double discount(int claimFreeYears, double rate) const;
    void claim(double amount);
    bool operator>(const Policy &other) const;

    int getNo() const { return policyNo; }
    int getCustId() const { return custId; }
    double getSum() const { return sumAssured; }
    int getYears() const { return years; }
    double getClaimed() const { return claimed; }

    friend void printStatement(const Policy &p);
    static int getTotal() { return total; }
};

int Policy::total = 0;

Policy::Policy(int c, double s, int y) : policyNo(1000 + (++total)) {
    if (s <= 0)
        throw runtime_error("Sum assured must be more than 0");
    if (y < 1 || y > 30)
        throw runtime_error("Years must be between 1 and 30");
    custId = c;
    sumAssured = s;
    years = y;
    claimed = 0;
}

void Policy::show() const {
    cout << "Policy No: " << policyNo << endl;
    cout << "Type: " << type() << endl;
    cout << "Customer ID: " << custId << endl;
    cout << "Sum assured: " << sumAssured << endl;
    cout << "Years: " << years << endl;
    cout << "Premium per year: " << premium() << " (with GST " << addGst(premium()) << ")" << endl;
}

double Policy::discount(int claimFreeYears) const {
    return discount(claimFreeYears, 0.02);
}

double Policy::discount(int claimFreeYears, double rate) const {
    if (claimFreeYears > 5)
        claimFreeYears = 5;
    return premium() * rate * claimFreeYears;
}

void Policy::claim(double amount) {
    if (amount <= 0)
        throw runtime_error("Claim amount must be more than 0");
    if (claimed + amount > sumAssured)
        throw runtime_error("Claim is more than the remaining cover");
    claimed += amount;
}

bool Policy::operator>(const Policy &other) const {
    return premium() > other.premium();
}

void printStatement(const Policy &p) {
    cout << "---- Statement ----" << endl;
    cout << "Policy " << p.policyNo << " (" << p.type() << ")" << endl;
    cout << "Cover: " << p.sumAssured << endl;
    cout << "Claimed: " << p.claimed << endl;
    cout << "Left: " << p.sumAssured - p.claimed << endl;
}

class LifePolicy : public Policy {
    int holderAge;

public:
    LifePolicy(int c, double s, int y, int a) : Policy(c, s, y) {
        holderAge = a;
    }
    double premium() const { return sumAssured * 0.01 + holderAge * 40; }
    string type() const { return "Life"; }
};

class HealthPolicy : public Policy {
public:
    HealthPolicy(int c, double s, int y) : Policy(c, s, y) {}
    double premium() const { return sumAssured * 0.02 + 1500; }
    string type() const { return "Health"; }
};

class VehiclePolicy : public Policy {
    string regNo;

public:
    VehiclePolicy(int c, double s, string reg, int y = 1) : Policy(c, s, y) {
        regNo = reg;
    }
    double premium() const { return sumAssured * 0.03; }
    string type() const { return "Vehicle"; }
};

int findCustomer(Customer list[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (list[i].getId() == id)
            return i;
    }
    throw runtime_error("Customer not found");
}

Policy *findPolicy(Policy *list[], int n, int no) {
    for (int i = 0; i < n; i++) {
        if (list[i]->getNo() == no)
            return list[i];
    }
    throw runtime_error("Policy not found");
}

void addCustomer(Customer list[], int &n) {
    string name, nomName, rel;
    int age;

    cin.ignore();
    cout << "Name: ";
    getline(cin, name);
    cout << "Age: ";
    cin >> age;
    cin.ignore();
    cout << "Nominee name: ";
    getline(cin, nomName);
    cout << "Nominee relation: ";
    getline(cin, rel);

    list[n].setData(name, age, nomName, rel);
    cout << "Customer added, ID = " << list[n].getId() << endl;
    n++;
}

void buyPolicy(Customer cust[], int nc, Policy *pol[], int &np) {
    int id, choice, years;
    double sum;

    cout << "Customer ID: ";
    cin >> id;
    int idx = findCustomer(cust, nc, id);

    cout << "1. Life  2. Health  3. Vehicle" << endl;
    cout << "Choose: ";
    cin >> choice;
    cout << "Sum assured: ";
    cin >> sum;

    Policy *p = NULL;
    if (choice == 1) {
        cout << "Years: ";
        cin >> years;
        p = new LifePolicy(id, sum, years, cust[idx].getAge());
    } else if (choice == 2) {
        cout << "Years: ";
        cin >> years;
        p = new HealthPolicy(id, sum, years);
    } else if (choice == 3) {
        string reg;
        cout << "Vehicle number: ";
        cin >> reg;
        p = new VehiclePolicy(id, sum, reg);
    } else {
        throw runtime_error("Wrong choice");
    }

    pol[np++] = p;
    cout << "Policy created" << endl;
    p->show();
}

void saveToFile(Policy *pol[], int np) {
    ofstream out("policies.txt");
    if (!out)
        throw runtime_error("Could not open file");
    out << fixed << setprecision(2);
    for (int i = 0; i < np; i++) {
        out << pol[i]->getNo() << " " << pol[i]->type() << " " << pol[i]->getCustId() << " "
            << pol[i]->getSum() << " " << pol[i]->getYears() << " " << pol[i]->getClaimed() << endl;
    }
    out.close();
    cout << "Saved " << np << " policies" << endl;
}

void readFromFile() {
    ifstream in("policies.txt");
    if (!in)
        throw runtime_error("No saved file yet, save first");
    string line;
    cout << "PolicyNo Type CustID Sum Years Claimed" << endl;
    while (getline(in, line)) {
        cout << line << endl;
    }
    in.close();
}

int main() {
    Customer customers[50];
    Policy *policies[100];
    int nc = 0, np = 0;
    int choice;

    cout << fixed << setprecision(2);

    do {
        cout << "\n--- Insurance System ---" << endl;
        cout << "1. Add customer" << endl;
        cout << "2. Buy policy" << endl;
        cout << "3. Show customers" << endl;
        cout << "4. Show policies" << endl;
        cout << "5. Make a claim" << endl;
        cout << "6. Compare two policies" << endl;
        cout << "7. Policy statement" << endl;
        cout << "8. Save to file" << endl;
        cout << "9. Read from file" << endl;
        cout << "10. Summary" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            if (cin.eof())
                break;
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please enter a number" << endl;
            choice = -1;
            continue;
        }

        try {
            if (choice == 1) {
                if (nc >= 50)
                    throw runtime_error("Customer list is full");
                addCustomer(customers, nc);
            } else if (choice == 2) {
                if (np >= 100)
                    throw runtime_error("Policy list is full");
                buyPolicy(customers, nc, policies, np);
            } else if (choice == 3) {
                for (int i = 0; i < nc; i++)
                    customers[i].show();
                if (nc == 0)
                    cout << "No customers yet" << endl;
            } else if (choice == 4) {
                for (int i = 0; i < np; i++) {
                    policies[i]->show();
                    cout << endl;
                }
                if (np == 0)
                    cout << "No policies yet" << endl;
            } else if (choice == 5) {
                int no;
                double amt;
                cout << "Policy no: ";
                cin >> no;
                Policy *p = findPolicy(policies, np, no);
                cout << "Claim amount: ";
                cin >> amt;
                p->claim(amt);
                cout << "Claim approved" << endl;
            } else if (choice == 6) {
                int a, b;
                cout << "First policy no: ";
                cin >> a;
                cout << "Second policy no: ";
                cin >> b;
                Policy *p1 = findPolicy(policies, np, a);
                Policy *p2 = findPolicy(policies, np, b);
                if (*p1 > *p2)
                    cout << "Policy " << a << " has higher premium" << endl;
                else
                    cout << "Policy " << b << " has higher premium (or both same)" << endl;
                cout << "Highest premium: " << bigger(p1->premium(), p2->premium()) << endl;
            } else if (choice == 7) {
                int no;
                cout << "Policy no: ";
                cin >> no;
                printStatement(*findPolicy(policies, np, no));
            } else if (choice == 8) {
                saveToFile(policies, np);
            } else if (choice == 9) {
                readFromFile();
            } else if (choice == 10) {
                cout << "Total customers: " << Customer::getCount() << endl;
                cout << "Total policies: " << Policy::getTotal() << endl;
            } else if (choice != 0) {
                cout << "Invalid choice" << endl;
            }
        } catch (exception &e) {
            cout << "Error: " << e.what() << endl;
        }
    } while (choice != 0);

    // free the memory
    for (int i = 0; i < np; i++)
        delete policies[i];

    cout << "Bye!" << endl;
    return 0;
}
