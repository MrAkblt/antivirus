#include <iostream>
#include <fstream>
using namespace std;

string filePath;

void setArg()
{
    cout << "Taranacak dosyanın yolunu belirtin." << endl;
    cin >> filePath;
}

void getTwoByte()
{
    ifstream file(filePath, ios::binary);
    if (file.is_open())
    {
        char c;
        cout << "Dosyanın ilk iki baytı: ";
        for (int i = 0; i < 2; i++)
        {
            file.get(c);
            cout << c;
        }
        cout << endl;
        file.close();
    }
    else
    {
        cout << "Dosya açılamıyor";
    }
}
int main()
{
    setArg();
    getTwoByte();

    return 0;
}