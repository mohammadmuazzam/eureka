#include <linux/input.h>
#include <linux/uinput.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <asm/ioctl.h>
#include <cstring>


using namespace std;

int virtual_fd;
int mouse_fd;
int threshold_x = 100;
int threshold_y = 50;

struct uinput_setup setup_uinput_mouse(int v_mouse_fd);

int main()
{
    mouse_fd = open("/dev/input/event16", O_RDONLY);
    virtual_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (mouse_fd < 0 || virtual_fd < 0) {
        cerr << "Failed to open mouse device" << endl;
        return 1;
    }
    
    cout << "Starting..." << endl;
    struct input_event device_event;
    struct uinput_setup virtual_event = setup_uinput_mouse(virtual_fd);

    int result = ioctl(mouse_fd, EVIOCGRAB, 1);
    if (result < 0) {
        cerr << "Failed to grab mouse device" << endl;
        return 1;
    }

    int accumulated_x = 0;
    int accumulated_y = 0;

    while (true)
    {
        ssize_t read_bytes = read(mouse_fd, &device_event, sizeof(struct input_event));
        //#cout << "Read " << read_bytes << " bytes from mouse device" << endl;
        //#cout << "Event type: " << ev.type << ", code: " << ev.code << ", value: " << ev.value << endl;

        if (read_bytes < 0) {
            cerr << "Failed to read from mouse device " << endl;
            return 1;
        }

        if (device_event.type == EV_REL) 
        {
            if (device_event.code == REL_WHEEL)
            {
                cout << "Mouse wheel event: " << device_event.value << endl;
            }

            struct input_event scroll_event = device_event;

            //* > 0, right
            //* < 0, left
            if (device_event.code == REL_X)
            {
                cout << "Mouse X movement: " << device_event.value << endl;

                accumulated_x += device_event.value;
                if (abs(accumulated_x) > threshold_x) {
                    scroll_event.code = REL_HWHEEL;
                    scroll_event.value = accumulated_x > 0 ? 1 : -1;
                    accumulated_x %= threshold_x;
                }

            }
            //* > 0, down
            //* < 0, up
            if (device_event.code == REL_Y)
            {
                cout << "Mouse Y movement: " << device_event.value << endl;
                
                accumulated_y += device_event.value;
                if (abs(accumulated_y) > threshold_y) {
                    scroll_event.code = REL_WHEEL;
                    scroll_event.value = accumulated_y < 0 ? 1 : -1;
                    accumulated_y %= threshold_y;
                }   
            }
            
            write(virtual_fd, &scroll_event, sizeof(struct input_event));
            
        }
        if (device_event.type == EV_SYN)
        {
            struct input_event sync_event = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};
            write(virtual_fd, &sync_event, sizeof(struct input_event));
        }
    }
}

struct uinput_setup setup_uinput_mouse(int v_mouse_fd)
{
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    ioctl(v_mouse_fd, UI_SET_EVBIT, EV_REL);        //* relative movement (for mouse)
    ioctl(v_mouse_fd, UI_SET_RELBIT, REL_X);        //* x
    ioctl(v_mouse_fd, UI_SET_RELBIT, REL_Y);        //* y
    ioctl(v_mouse_fd, UI_SET_RELBIT, REL_WHEEL);    //* wheel

    ioctl(v_mouse_fd, UI_SET_EVBIT, EV_KEY);
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "Virtual Mouse");

    ioctl(v_mouse_fd, UI_DEV_SETUP, &usetup);
    ioctl(v_mouse_fd, UI_DEV_CREATE);

    return usetup;
}