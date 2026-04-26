#include <linux/uinput.h>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <cstring>

using namespace std;

#define BITS_PER_LONG (sizeof(long) * 8)
#define NLONGS(x) ((x + BITS_PER_LONG - 1) / BITS_PER_LONG)

class Virtual_Device {
    private:
        std::string device_name;
        int fd;

        struct uinput_setup setup_mouse(string name, int physical_mouse_fd)
        {
            struct uinput_setup usetup;
            memset(&usetup, 0, sizeof(usetup));
            unsigned long key_bits[NLONGS(KEY_CNT)] = {0};
            unsigned long rel_bits[NLONGS(REL_CNT)] = {0};

            if (ioctl(physical_mouse_fd, EVIOCGBIT(EV_REL, sizeof(rel_bits)), rel_bits) < 0 ||
                ioctl(physical_mouse_fd, EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits) < 0)
            {
                cerr << "Error getting mouse bits" << endl;
                return {0};
            }

            ioctl(fd, UI_SET_EVBIT, EV_SYN);
            ioctl(fd, UI_SET_RELBIT, EV_REL);
            ioctl(fd, UI_SET_EVBIT, EV_KEY);

            //* set relative bits
            for (int i = 0; i < REL_CNT; i++) 
            {
                if (rel_bits[i / BITS_PER_LONG] & 1UL << (i % BITS_PER_LONG))
                {
                    ioctl(fd, UI_SET_RELBIT, i);
                }
            }

            //* set key bits
            for (int i = 0; i < KEY_CNT; i++)
            {
                if (key_bits[i / BITS_PER_LONG] & 1UL << (i % BITS_PER_LONG))
                {
                    ioctl(fd, UI_SET_KEYBIT, i);
                }
            }            
            
            usetup.id.bustype = BUS_USB;
            usetup.id.vendor = 0xDEAD;
            usetup.id.product = 0xBEEF;
            strcpy(usetup.name, name.c_str());

            ioctl(fd, UI_DEV_SETUP, &usetup);
            ioctl(fd, UI_DEV_CREATE);

            return usetup;
        }


    public:
        struct uinput_setup usetup;
        Virtual_Device(string name, int physical_mouse_fd) : device_name(name)
        {
            fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
            if (fd < 0) 
            {
                cerr << "Failed to open uinput device" << endl;
                return;
            }

            setup_mouse(name, physical_mouse_fd);
        }
};