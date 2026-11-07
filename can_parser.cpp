// File:    can_parser.cpp
// Author:  Blake Hill
// Usage:   Parses CAN messages through the CAN dbc file, create a CanParser object and
//          then pass in CAN_message_t objects for their signals.

#include "can_parser.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

using msgMap = std::unordered_map<uint64_t, const dbcppp::IMessage *>;

CanParser::CanParser() {
    this->dbc = load_dbc();
    this->msgMap = loadMessages();
}

CanParser::CanParser(const std::filesystem::path dbc_file_path) : dbc_file_path(dbc_file_path) {
    dbc = load_dbc();
    msgMap = loadMessages();
}

std::unique_ptr<dbcppp::INetwork> CanParser::load_dbc() {
    std::ifstream dbc_file(this->dbc_file_path);
    if (!dbc_file.is_open()) {
        throw std::runtime_error("Failed to open DBC file:" + this->dbc_file_path.string());
    }
    std::unique_ptr<dbcppp::INetwork> dbc = dbcppp::INetwork::LoadDBCFromIs(dbc_file);
    if (!dbc) {
        throw std::runtime_error("Failed to load DBC file:" + this->dbc_file_path.string());
    }
    return dbc;
}

messageMap CanParser::loadMessages() {
    std::unordered_map<uint64_t, const dbcppp::IMessage *> messages;
    for (const dbcppp::IMessage &msg : this->dbc->Messages()) {
        messages.insert(std::make_pair(msg.Id(), &msg));
    }
    return messages;
}

messageMap CanParser::getMessages() { return this->msgMap; }

std::optional<SignalMap> CanParser::parse_frame(const CAN_message_t &frame) {
    auto iter = msgMap.find(frame.id);
    if (iter != msgMap.end()) {
        const dbcppp::IMessage *message = iter->second;

        std::unordered_map<std::string, DecodedSignal> sigMap;
        for (const dbcppp::ISignal &sig : message->Signals()) {
            auto raw_sig = sig.Decode(frame.buf);
            double sig_val = sig.RawToPhys(raw_sig);

            DecodedSignal decodeSig;
            decodeSig.name = sig.Name();
            decodeSig.value = sig_val;
            decodeSig.unit = sig.Unit();

            sigMap[sig.Name()] = decodeSig;
        }
        return SignalMap(sigMap);
    } else {
        std::cerr << "ID " << frame.id << " not found\n";
        return std::nullopt;
    }
}

void CanParser::print_network_info() {
    std::cout << "DBC Version: " << this->dbc->Version() << "\n";
    std::cout << "Number of Nodes: " << this->dbc->Nodes_Size() << "\n";
    std::cout << "Number of Messages: " << this->dbc->Messages_Size() << "\n";
    std::cout << "Number of Value Tables: " << this->dbc->ValueTables_Size() << "\n";
    std::cout << "Number of Environment Variables: " << this->dbc->EnvironmentVariables_Size()
              << "\n";
    std::cout << "Number of Attribute Definitions: " << this->dbc->AttributeDefinitions_Size()
              << "\n";
    std::cout << "Number of Attribute Defaults: " << this->dbc->AttributeDefaults_Size() << "\n";
    std::cout << "Number of Attribute Values: " << this->dbc->AttributeValues_Size() << "\n";
    std::cout << "Comment: " << this->dbc->Comment() << "\n";
}

void CanParser::print_messages() {
    try {
        for (const auto &msg : this->dbc->Messages()) {
            std::cout << msg.Name() << " | ID: " << msg.Id() << " | Bytes: " << msg.MessageSize()
                      << '\n';
            for (const auto &sig : msg.Signals()) {
                std::cout << "  " << sig.Name() << " | Start Bit: " << sig.StartBit()
                          << " | Length: " << sig.BitSize() << " | Factor: " << sig.Factor()
                          << " | Offset: " << sig.Offset() << " | Unit: " << sig.Unit() << '\n';
            }
        }
    } catch (const std::exception &err) {
        std::cerr << "Error: " << err.what() << '\n';
    }
}

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
