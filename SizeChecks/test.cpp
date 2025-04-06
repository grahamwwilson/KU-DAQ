#include <iostream>
using namespace std;

int main() 
{    
    cout << "Size of char: " << sizeof(char) << " byte" << endl;
    cout << "Size of short: " << sizeof(short) << " bytes" << endl;
    cout << "Size of int: " << sizeof(int) << " bytes" << endl;
    cout << "Size of long: " << sizeof(long) << " bytes" << endl;
    cout << "Size of long long: " << sizeof(long long) << " bytes" << endl;
 
    cout << " " << endl;

    cout << "Size of unsigned short: " << sizeof(unsigned short) << " bytes" << endl;
    cout << "Size of unsigned int: " << sizeof(unsigned int) << " bytes" << endl;
    cout << "Size of unsigned long: " << sizeof(unsigned long) << " bytes" << endl;
    cout << "Size of unsigned long long: " << sizeof(unsigned long long) << " bytes" << endl;

    cout << " " << endl;

    cout << "Size of float: " << sizeof(float) << " bytes" << endl;
    cout << "Size of double: " << sizeof(double) << " bytes" << endl;
    cout << "Size of long double: " << sizeof(long double) << " bytes" << endl;

    return 0;
}
