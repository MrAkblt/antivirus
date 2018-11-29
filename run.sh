echo Çalıştırmak istediğiniz uygulamanın adını girin:
read varname
g++ -std=c++11 $varname.cpp -o $varname
./$varname
