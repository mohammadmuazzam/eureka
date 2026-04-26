//* one remapper per device
#include <map>
#include <string>

using namespace std;

class Remapper {
    public:
        Remapper();
    private:
        //* from original device to virtual device
        string device_name;
        map <unsigned short, unsigned short> mapping;
};