<p align="center"><b>
A discrete PID implementation optimized for embedded systems
</b></p>

### Example

```cpp
using namespace pidlib;

int main(){
    Pid<uint8_t> controller;
    controller.init(10000, 1);
    controller.tune(1, .5, .5);
    controller.limit(0, 256);

    while (1)
    {
        error = current_position - setpoint;
        controll_signal = controller.run(error);
    }
}
```

### Features

- [x] Optimized for microcontrollers without floating points hardware
- [x] Header only library. No `cpp` files to compile
- [x] No dynamic memory allocation
- [x] Templating