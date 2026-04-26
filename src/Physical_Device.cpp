#include <string>
#include <vector>
#include <linux/input.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <fcntl.h>

using namespace std;
namespace fs = std::filesystem;

class Physical_Device
{
    private:
        string device_name;
        string device_path;
        int fd;
                
    public:
        Physical_Device(string name, string path, int file_descriptor) 
            : device_name(name), device_path(path), fd(file_descriptor) {}

        string get_name() const { return device_name; }
        string get_path() const { return device_path; }
        int get_fd() const { return fd; }

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

vector<Physical_Device> Physical_Device::get_connected_devices()
{
    vector<Physical_Device> devices;
    string input_dir = "/dev/input/by-id";

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        string path = entry.path().string();

        // Only process character devices named "event"
        if (path.find("event") != string::npos) {
            
            // Attempt to open the device file
            int temp_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
            if (temp_fd < 0) {
                continue; // Skip if we lack permissions (run as root)
            }

            char name[256] = "Unknown";
            // Grab the human-readable device name from the kernel
            if (ioctl(temp_fd, EVIOCGNAME(sizeof(name)), name) < 0) {
                cerr << "Failed to get name for " << path << endl;
            }

            devices.push_back(Physical_Device(name, path, temp_fd));
        }
    }
    return devices;
}