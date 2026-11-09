#include "can_parser.hpp"

int main() {
    CanParser canparse = CanParser();
    // canparse.print_messages();
    CAN_message_t frame{};
    frame.len = 8;
    frame.id = 0xB0;
    frame.buf[0] = 0;
    frame.buf[1] = 0x01;
    frame.buf[2] = 0;
    frame.buf[3] = 0x01;
    frame.buf[4] = 0;
    frame.buf[5] = 0x10;
    frame.buf[6] = 0x10;
    frame.buf[7] = 0;
    auto sigmap = canparse.parse_frame(frame);
    if (!sigmap) {
        return 1;
    }

    std::string name = "TorqueCommand";
    auto val = sigmap->getSignalValue(name);
    if (!val) {
        return 1;
    }
    printf("Signal %s has value %f\n", name.c_str(), *val);
}