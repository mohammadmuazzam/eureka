#include <string>
#include <vector>

using namespace std;

class Physical_Device
{
    private:
        string device_name;
                
    public:
        static vector<Physical_Device> get_connected_devices();
};