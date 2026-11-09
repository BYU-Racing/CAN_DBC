#include "can_parser.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstring>
#include <type_traits>

struct CanParserFixture {
    CanParser parser{TEST_DBC_PATH};
    CAN_message_t frame{};
};

void makeFrameHighSpeed(CAN_message_t &frame) {
    frame.id = 0xB0;
    const uint8_t bytes[8] = {0, 0x1, 0x1, 0x1, 0, 0x10, 0x10, 0};
    std::memcpy(frame.buf, bytes, sizeof(bytes));
    frame.len = sizeof(bytes);
}

TEST_CASE_METHOD(CanParserFixture, "Parse a single CAN frame", "[can_parser]") {
    makeFrameHighSpeed(frame);

    auto result = parser.parse_frame(frame);
    REQUIRE(result.has_value());
}

TEST_CASE_METHOD(CanParserFixture, "Parse a single CAN frame, get one signal", "[can_parser]") {
    makeFrameHighSpeed(frame);

    auto sigmap = parser.parse_frame(frame);
    REQUIRE(sigmap.has_value());

    auto val = (*sigmap).getSignalValue("TorqueCommand");
    REQUIRE(val.has_value());
    CHECK_THAT(*val, Catch::Matchers::WithinAbs(25.6, 1e-9));
}

TEST_CASE_METHOD(CanParserFixture, "Parse several signals within same CAN ID", "[can_parser]") {
    makeFrameHighSpeed(frame);

    auto sigmap = parser.parse_frame(frame);
    REQUIRE(sigmap.has_value());

    auto val1 = (*sigmap).getSignalValue("TorqueCommand");
    REQUIRE(val1.has_value());
    CHECK_THAT(*val1, Catch::Matchers::WithinAbs(25.6, 1e-9));

    auto val2 = (*sigmap).getSignalValue("TorqueFeedback");
    REQUIRE(val2.has_value());
    CHECK_THAT(*val2, Catch::Matchers::WithinAbs(25.7, 1e-9));

    auto val3 = (*sigmap).getSignalValue("MotorSpeed");
    REQUIRE(val3.has_value());
    CHECK_THAT(*val3, Catch::Matchers::WithinAbs(4096.0, 1e-9));

    auto val4 = (*sigmap).getSignalValue("DCBusVoltage");
    REQUIRE(val4.has_value());
    CHECK_THAT(*val4, Catch::Matchers::WithinAbs(1.6, 1e-9));
}

TEST_CASE_METHOD(CanParserFixture, "Unknown CAN Ids not accepted", "[can_parser][errors]") {
    frame.id = 0xFFFF;

    auto result = parser.parse_frame(frame);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE_METHOD(CanParserFixture, "Create_frame creates CAN frame successfully", "[can_parser]") {
    uint32_t canId = 0xB0;
    std::unordered_map<std::string, double> signals = {
        {"TorqueCommand", 11.1},
        {"TorqueFeedback", 22.2},
        {"MotorSpeed", 333},
        {"DCBusVoltage", 4.4}
    };
    auto result = parser.create_frame(canId, signals);

    STATIC_REQUIRE(std::is_same_v<decltype(result), std::optional<CAN_message_t>>);
    REQUIRE(result.has_value());

    const auto &res_frame = *result;

    CHECK(res_frame.id == canId);
    CHECK(res_frame.len == 8);
    CHECK_FALSE(res_frame.flags.extended);
    CHECK_FALSE(res_frame.flags.remote);

    const uint8_t expected[] = {
        0x6F, 0x00, // TorqueCommand:  11.1 / 0.1 = 111
        0xDE, 0x00, // TorqueFeedback: 22.2 / 0.1 = 222
        0x4D, 0x01, // MotorSpeed:     333 / 1   = 333
        0x2C, 0x00  // DCBusVoltage:   4.4 / 0.1 = 44
    };

    for (std::size_t i = 0; i < sizeof(expected); i++) {
        CAPTURE(i);
        CHECK(res_frame.buf[i] == expected[i]);
    }
}