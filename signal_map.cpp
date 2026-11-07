#include "signal_map.hpp"

SignalMap::SignalMap(std::unordered_map<std::string, DecodedSignal> sig_map) : sig_map(sig_map) {}

std::optional<DecodedSignal> SignalMap::getSignalByName(std::string sig_name) {
    if (sig_map.find(sig_name) != sig_map.end()) {
        DecodedSignal signal = sig_map[sig_name];
        return signal;
    } else {
        fprintf(stderr, "Signal %s not found", sig_name.c_str());
        return std::nullopt;
    }
}

std::optional<double> SignalMap::getSignalValue(std::string sig_name) {
    auto signal = getSignalByName(sig_name);
    if (signal) {
        return signal->value;
    }
    return std::nullopt;
}