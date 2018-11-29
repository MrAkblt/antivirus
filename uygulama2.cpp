#include <iostream>
#include <fstream>
using namespace std;

string filePath;
string searchWord;
string fileString;

void setArg(){
  cout << "Taranacak dosyanın yolunu belirtin." << endl;
  cin >> filePath;
  cout << "Aranacak kelimeyi girin:";
  cin >> searchWord;
}

void searchMalware(){
  ifstream file(filePath/*, ios::binary*/);
  if (file.is_open())
  {
    string str; 
    while (getline(file, str))
    {
        fileString+=str+"\n";
    }
    
    size_t found = fileString.find(searchWord);
    if (found == string::npos)
    {
      cout << searchWord << " ifadesi herhangi bir konumda bulunamadı."<<endl;
    }
    else
    {
      cout << searchWord << " ifadesi " << found << " adresinde bulundu."<<endl;
    }
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
  searchMalware();

  return 0;
}

//   /home/murat/Templates/Antivirus/Viruses/1A1AD71CA246E37E1D4EC419C9E78F8191F08F2BC7F5D6F1570772F630B666C7
