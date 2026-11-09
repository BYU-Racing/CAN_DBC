#include "signal_map.hpp"
#include <catch2/catch_test_macros.hpp>
#include <type_traits>

TEST_CASE("SignalMap cannot be default constructured", "[signal_map][errors]") {
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<SignalMap>);
}

TEST_CASE("SignalMap returns a stored value", "[signal_map]") {
    std::string sig_name = "TorqueCommand";
    SignalMap signals({{sig_name, 26.5}});

    auto val = signals.getSignalValue(sig_name);

    REQUIRE(val.has_value());
    CHECK(*val == 26.5);
}

TEST_CASE("SignalMap returns nullopt with wrong/missing sig_name", "[signal_map]") {
    std::string sig_name = "TorqueCommand";
    SignalMap signals2({{sig_name, 26.5}});

    CHECK_FALSE(signals2.getSignalValue("Not a signal").has_value());
}