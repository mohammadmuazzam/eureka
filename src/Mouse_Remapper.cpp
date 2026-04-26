//* one remapper per device
#include <map>
#include <string>
#include <linux/input.h>

using namespace std;

class Mouse_Remapper {
    public:
        Mouse_Remapper();

        int get_remap_code(struct input_event ev)
        {
            if (code_mapping.find(ev.code) != code_mapping.end())
            {
                return code_mapping[ev.code];
            }
            return -1;
        }

    private:
        //* original device -> virtual device
        string device_name;
        map <unsigned short, unsigned short> code_mapping;
        int cthreshold_x, cthreshold_y,                         //* for cursor sensitivity
            sthreshold_x, sthreshold_y;                         //* for scroll sensitivity

};