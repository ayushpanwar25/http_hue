#define CURL_STATICLIB
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<curl/curl.h>
using namespace std;
void loadUsertoMem();
void loadDevtoMem();
void initiate();
void signup();
void signin();
int validateUsername(string);
void fetchDevices(string);
void addDevice(string);
void displayDevice();
void control(int, int);
void decor();
size_t writeFunction(void*, size_t, size_t, void*);
struct device
{
    string owner;
    string Url;
    string name;
    string type;
    string key="null";
    int id=0;
    friend istream& operator>>(istream& input, device& d);
};
struct user
{
    string username;
    string password;
    friend istream& operator>>(istream& input, user& u);
};
istream& operator>>(istream& input, device& d)
{
    input >> d.owner;
    input >> d.Url;
    input >> d.name;
    input >> d.type;
    input >> d.id;
    input >> d.key;
    return input;
}
istream& operator>>(istream& input, user& u)
{
    input >> u.username;
    input >> u.password;
    return input;
}
int LOGIN_STATE = 0;
string activeUser;
vector<user> users;
vector<device> devs, sub;
int main()
{   
    int j, k = 1; 
    loadUsertoMem();
    loadDevtoMem();
    initiate();
    while (k)
    {
        displayDevice();
        cout << "Select device to control:   ";
        cin >> k;
        if (k == -2)
        {
            LOGIN_STATE = 0;
            initiate();
            continue;
        }
        else if (k == -1)
        {
            addDevice(activeUser);
            continue;
        }
        else if (k == 0)
            return 0;
        cout << "(1) Turn on\t(0) Turn off:  ";
        cin >> j;
        control(k, j);
        decor();
    }
    return 0;
}
void initiate()
{
    int i;
    cout << "====================================================" << endl;
    cout << "               IoT Device Controller" << endl;
    cout << "====================================================" << endl;
    do
    {
        cout << "(1) Sign in\t(2) Register:  ";
        cin >> i;
        if (i == 1)
            signin();
        else if (i == 2)
            signup();
        else
            cout << "Invalid Input\n";
    } while (LOGIN_STATE == 0);
    decor();
    fetchDevices(activeUser);
    if (sub.size() == 0)
        addDevice(activeUser);
}
void decor()
{
    cout << "\n====================================================" << endl;
    cout << "\nWelcome, " << activeUser << "\nCONTROLS:\tExit: 0\n\t\tAdd new device: -1\n\t\tSign out: -2\n\n";
}
void signin()
{
    string username, password;
    cout << "\nEnter username:   ";
    cin >> username;
    cout << "Enter password:   ";
    cin >> password;
    if (username.compare("admin") == 0 && password.compare("pass") == 0)
    {
        LOGIN_STATE = 1;
        activeUser = "admin";
        return;
    }
    for (auto& i : users)
    {
        if (username.compare(i.username) == 0 && password.compare(i.password) == 0)
        {
            LOGIN_STATE = 1;
            activeUser = i.username;
            return;
        }
    }
    cout << "Wrong Credentials. Try again!\n";
}
void signup()
{
    string username, password;
    cout << "\nEnter username:   ";
    cin >> username;
    if (!validateUsername(username))
    {
        cout << "Account already exists with the username!\n";
        return;
    }
    cout << "Enter password:   ";
    cin >> password;
    ofstream eUser("accounts.dat", ios::app);
    eUser << username << '\t' << password << endl;
    eUser.close();
    cout << endl;
    loadUsertoMem();
}
void loadUsertoMem()
{
    users.clear();
    user temp;
    ifstream f("accounts.dat");
    while (f >> temp)
        users.push_back(temp);
    f.close();
}
int validateUsername(string a)
{
    if(a.compare("admin")==0)
        return 0;
    for (auto& i : users)
    {
        if (a.compare(i.username) == 0)
            return 0;
    }
    return 1;
}
void loadDevtoMem()
{
    devs.clear();
    device temp;
    ifstream f("devices.dat");
    while (f >> temp)
        devs.push_back(temp);
    f.close();
}
void fetchDevices(string a)
{
    sub.clear();
    for (auto& j : devs)
    {
        if (a.compare("admin") == 0)
           sub.push_back(j);
        else if (a.compare(j.owner) == 0)
            sub.push_back(j);
    }
}
void addDevice(string a)
{
    string Url;
    string name;
    string type;
    string apikey;
    int id;
    cout << "\nAdd new device:\n";
    cout << "Enter device name:   ";
    cin >> name;
    cout << "Enter device type (wled/hue):   ";
    cin >> type;
    cout << "Enter device address:   ";
    cin >> Url;
    if (type.compare("hue") == 0)
    {
        cout << "Enter light ID:   ";
        cin >> id;
        cout << "Enter Hue API key:   ";
        cin >> apikey;
    }
    else
    {
        id = 0;
        apikey = "null";
    }
    if (a.compare("admin") == 0)
    {
        cout << "Enter owner:   ";
        cin >> a;
    }
    ofstream nDev("devices.dat", ios::app);
    nDev << a << '\t' << Url << '\t' << name << '\t' << type << '\t' << id << '\t' << apikey << endl;
    nDev.close();
    decor();
    loadDevtoMem();
    fetchDevices(a);
}
void displayDevice()
{
    cout << "Available Devices:\n\n";
    for (size_t j = 0; j < sub.size(); j++)
    {
        cout << "\t" << j + 1 << ". " << sub[j].name << endl << endl;
    }
}
void control(int a, int b)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    string url;
    if (sub[a-1].type.compare("wled")==0)
    {
        if (curl)
        {
            url = "http://";
            url.append(sub[a - 1].Url);
            url.append("/win&T=2");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
            curl_easy_setopt(curl, CURLOPT_HEADER, 0);
            curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, true);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
            curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 10);
            curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, true);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            curl = NULL;
        }
    }
    else if (sub[a - 1].type.compare("hue") == 0)
    {
        static const char* json_struct;
        if (b == 1)
            json_struct = "{\"on\":true}";
        else if (b == 0)
            json_struct = "{\"on\":false}";
        else
        {
            cout << "Invalid Input" << endl;
            return;
        }
        if (curl)
        {
            url = "http://";
            url.append(sub[a - 1].Url);
            url.append("/api/");
            url.append(sub[a - 1].key);
            url.append("/lights/");
            url.append(to_string(sub[a - 1].id));
            url.append("/state");
            curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_struct);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_perform(curl);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
    }
    else
        cout << "Invalid Input" << endl;
    cout << endl;
}
size_t writeFunction(void* buffer, size_t size, size_t nmemb, void* userp)
{
    return size * nmemb;
}
