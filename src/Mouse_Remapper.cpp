//* one remapper per device
#include <map>
#include <string>

using namespace std;

class Mouse_Remapper {
    public:
        Mouse_Remapper();
    private:
        //* from original device to virtual device
        string device_name;
        map <unsigned short, unsigned short> mapping;
        int threshold_x, threshold_y; //* for scroll sensitivity
};