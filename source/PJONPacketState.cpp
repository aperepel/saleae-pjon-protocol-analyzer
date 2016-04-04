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
        case Packet_DeviceId:
            return (char*) "Device ID";
            break;
            
        case Packet_Length:
            return (char*) "Length";
            break;
            
        case Packet_Payload:
            return (char*) "Payload";
            break;
            
        case Packet_Checksum:
            return (char*) "Checksum";
            break;
            
        case Packet_AckNack:
            return (char*) "AckNack";
            break;
            
        default:
            return (char*) "(Unknown)";

    }
}

short PJONPacketState::asDisplayFlag() {
    Packet current = this->current();
    switch (current) {
        case Packet_DeviceId:
            return FLAG_DEVICE_ID;
            break;

        case Packet_Length:
            return FLAG_LENGTH;
            break;

        case Packet_Payload:
            return FLAG_PAYLOAD;
            break;

        case Packet_Checksum:
            return FLAG_CHECKSUM;
            break;
            
        case Packet_AckNack:
            return FLAG_ACK_NACK;
            break;

        default:
            return 0;
    }
}