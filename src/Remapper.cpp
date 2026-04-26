//* one remapper per device
#include <map>

using namespace std;

class Remapper {
    public:
        //* from original device to virtual device 
        map <unsigned short, unsigned short> mapping;
    private:
        
};