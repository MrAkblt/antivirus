#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

string searchWord;
bool findVirus = false;
vector<string> folderElements;

bool is_file(string path)
{
  const char *f_path = path.c_str();
  struct stat buf;
  stat(f_path, &buf);
  return S_ISREG(buf.st_mode);
}

//Path(yol) alır, içerisindeki klasör veya dosyaları pathlerini folderElements vektörüne ekler.
void read_directory(const string folderPath)
{
  DIR *dirp = opendir(folderPath.c_str());
  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL)
  {
    string elementPath = folderPath + dp->d_name;
    if (!is_file(elementPath))
    {
      elementPath += "/";
    };
    if (elementPath.compare(folderPath + "./") != 0 && elementPath.compare(folderPath + "../") != 0)
    {
      folderElements.push_back(elementPath);
    }
  }
  closedir(dirp);
}

//Gelen dosyada istenilen kelimeyi arar.Bulursa yolu ve adıyla birlikte yazdırır.
void searchMalware(string filePath)
{
  string fileString;

  ifstream f_in(filePath, ios::binary);
  if (!f_in.is_open()) {
    cout << "Dosya açılamıyor";
    return;
  }
  string str;
  while (getline(f_in, str))
  {
    fileString += str + "\n";
  }
  size_t found = fileString.find(searchWord);
  if (found != string::npos)
  {
    cout << filePath << ": BULUNDU (" << found << " adresinde)\n";
    findVirus = true;
  }
  else{
    cout<<filePath<< ": BULUNAMADI\n";
  }
  f_in.close();
}

/*folderElement Dizisini dosya kontrolünü sağlar eğer dosya ise taraması için searchMalware, 
  klasör ise içindekileri getirmesi için read_directory fonksyonuna gönderir.Dizide kullandığı elemanları siler
  ve boş ise döndügen çıkar.*/
void controlandRoute()
{
  while (1)
  {
    if (is_file(folderElements.front()))
    {
      searchMalware(folderElements.front());
    }
    else
    {
      read_directory(folderElements.front());
    }
    folderElements.erase(folderElements.begin());
    if (folderElements.empty())
      break;
  }
}

void setArg()
{
  string folderPath;
  cout << "Taranacak klasörün yolunu belirtin." << endl;
  cin >> folderPath;
  cout << "Aranacak kelimeyi girin:";
  cin >> searchWord;
  read_directory(folderPath);
  /* read_directory yerine commentli bu kısım file veya folder ayırt etmeksizin işlem yaptırır.
  if(!is_file(folderPath)){
     if(!dosya sonunda "/" var mı?){folderPath+=folderPath} 
  }
  folderElements.push_back*/
}

int main()
{
  setArg();
  controlandRoute();
  return 0;
}

//   /home/murat/Templates/Antivirus/Viruses/
