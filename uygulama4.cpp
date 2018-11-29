#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
using namespace std;

typedef uint64_t QWORD;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

#define READ_BYTE(p) (((unsigned char *)(p))[0])
#define READ_WORD(p) ((((unsigned char *)(p))[0]) | ((((unsigned char *)(p))[1]) << 8))
#define READ_DWORD(p) ((((unsigned char *)(p))[0]) | ((((unsigned char *)(p))[1]) << 8) | \
                       ((((unsigned char *)(p))[2]) << 16) | ((((unsigned char *)(p))[3]) << 24))
#define READ_QWORD(p) *((unsigned long long*)(p))
#define PAD(x) (((x) + 3) & 0xFFFFFFFC)

string searchWord;
vector<string> folderElements;

bool is_file(string path)
{
    const char *f_path = path.c_str();
    struct stat buf;
    stat(f_path, &buf);
    return S_ISREG(buf.st_mode);
}

//Path(yol) alır, içerisindeki klasör veya dosyaları pathlerini folderElements vektörüne ekler.
void read_directory(string folderPath)
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

int searchMalware(string buf)
{
  size_t found = buf.find(searchWord);
  if (found != string::npos)
  {
    return found;
  }
  return -1;
}


//Gelen dosyada istenilen kelimeyi arar.Bulursa yolu ve adıyla birlikte yazdırır.
int searchSections(char *buf,string filePath)
{ 
  //buf is a IMAGE_DOS_HEADER
  if (READ_WORD(buf) != 0x5A4D) { 
    cout<<filePath<<": Geçerli PE dosyası değil\n";
    return 0; } //MZ imzası
  ///pe is a IMAGE_NT_HEADERS32
  const char *pe = buf + READ_DWORD(buf + 0x3C);
  if (READ_WORD(pe) != 0x4550) { return 0; }  //Soruda gerek duyulmayan satır
    //coff is a IMAGE_FILE_HEADER
  const char *coff = pe + 4;
  WORD numSections = READ_WORD(coff + 2);
  WORD optHeaderSize = READ_WORD(coff + 16);
  if (numSections == 0 || optHeaderSize == 0) return 0;  //Soruda gerek duyulmayan satır
  //Machine alanın kontrol edildiği kısım
  WORD machineType = READ_WORD(coff);
  if(machineType != 0x8664 && machineType != 0x14c) return 0; //Makine tip kontrolü
  //optHeader is a IMAGE_OPTIONAL_HEADER32
  const char *optHeader = coff + 20;
  //secTable is an array of IMAGE_SECTION_HEADER
  const char *secTable = optHeader + optHeaderSize;
  bool findVirus=false;
  for(int i=0; i<numSections; i++){
    const char *secHeader = secTable + 40*i;
    char secName[9];
    memcpy(secName, secHeader, 8);
    secName[8]=0;
    //exec flag 20(int=32) veya 20000000 , data flag 40(int 64)
    // Section executable kontrolü.  Bu kontrol kaldırılırsa aranan kelimenin hangi dosyada, sectionda ve adreste olduğu bilgileri dökülür.
    DWORD secCharacteristic = READ_DWORD(secHeader + 36);
    if (!(secCharacteristic & 0x00000020) && !(secCharacteristic & 0x20000000)) continue;
    DWORD pointerToRawData=READ_DWORD(secHeader + 20);
    DWORD sizeOfRawData=READ_DWORD(secHeader + 16);
    const char *sec = buf + pointerToRawData;
    string secData(sec,sizeOfRawData);
    int found=searchMalware(secData);
    if(found!=-1){
      //Hafizadaki hex adress için  (void *)&sec[found] kullanılabilir. Aşagıdaki adress dosya içerisindeki adrestir.
      cout<<filePath<<": BULUNDU ("<<pointerToRawData+found<<" adresinde ve \""<<secName<<"\" section'ı içerisinde)\n";
      findVirus=true; 
    }
  }
  if(!findVirus) cout<<filePath<<": BULUNAMADI\n";
 return 0;
}

char* getFileStrings(string filePath)
{
    const char *fPath = filePath.c_str();
    struct stat st;
    if (stat(fPath, &st) < 0)
    {
        perror(fPath);
        return 0;
    }
    FILE *file = fopen(fPath, "rb");
    if (!file)
    {
        perror(fPath);
        return 0;
    }
    char *fileStrings=new char[st.st_size];
    fread(fileStrings, 1, st.st_size, file);
    fclose(file);
    return fileStrings;
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
      char *fileStrings = getFileStrings(folderElements.front());
      searchSections(fileStrings,folderElements.front());
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
}

int main()
{
  setArg();
  controlandRoute();
  return 0;
}

//   /home/murat/Templates/Antivirus/Viruses/
