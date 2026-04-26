#include <string>
#include <vector>
#include <linux/input.h>
#include <unistd.h>
#include <iostream>


using namespace std;

class Physical_Device
{
    private:
        string device_name;
        int fd;
                
    public:
        static vector<Physical_Device> get_connected_devices();
        
        struct input_event read_event()
        {
            struct input_event ev;
            ssize_t read_bytes = read(fd, &ev, sizeof(struct input_event));
            if (read_bytes < 0) {
                cerr << "Failed to read from device " << device_name << endl;
                return {};
            }
            return ev;
        };
};