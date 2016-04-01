#include "PJONPacketState.h"
//#include <stdio.h>

PJONPacketState::PJONPacketState() {
    
}

PJONPacketState::~PJONPacketState() {
    
}

PJONPacketState::Packet PJONPacketState::next() {
    current_index = (++current_index) % enum_element_count;
    return (this->current());
}

PJONPacketState::Packet PJONPacketState::current() {
    return pjon_packet_types[current_index];
}

void PJONPacketState::reset() {
    current_index = 0;
}

char* PJONPacketState::asDisplayString(short flag) {   
    // meh, lazy
    if (flag == 0) {
        return (char*) "(Unknown)";
    }
    
    bool found = false;
    PJONPacketState p;
    for (int i = 0; i < enum_element_count; i++) {
        if (flag & p.asDisplayFlag()) {
            found = true;
            break;
        } else {
            p.next();
        }
    }

    if (!found) {
        return (char*) "(Unknown)";
    }
    
    Packet packet = p.current();
    switch (packet) {
        case PJONPacketState::Packet::DeviceId:
            return (char*) "Device ID";
            break;
            
        case PJONPacketState::Packet::Length:
            return (char*) "Length";
            break;
            
        case PJONPacketState::Packet::Payload:
            return (char*) "Payload";
            break;
            
        case PJONPacketState::Packet::Checksum:
            return (char*) "Checksum";
            break;
            
        case PJONPacketState::Packet::AckNack:
            return (char*) "AckNack";
            break;
            
        default:
            return (char*) "(Unknown)";

    }
}

short PJONPacketState::asDisplayFlag() {
    Packet current = this->current();
    switch (current) {
        case PJONPacketState::Packet::DeviceId:
            return FLAG_DEVICE_ID;
            break;

        case PJONPacketState::Packet::Length:
            return FLAG_LENGTH;
            break;

        case PJONPacketState::Packet::Payload:
            return FLAG_PAYLOAD;
            break;

        case PJONPacketState::Packet::Checksum:
            return FLAG_CHECKSUM;
            break;
            
        case PJONPacketState::Packet::AckNack:
            return FLAG_ACK_NACK;
            break;

        default:
            return 0;
    }
}