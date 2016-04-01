#ifndef PJON_PACKET_STATE_H
#define PJON_PACKET_STATE_H

class PJONPacketState
{
public:
    PJONPacketState();
    ~PJONPacketState();
 
    enum class Packet {
        DeviceId,
        Length,
        Payload,
        Checksum,
        AckNack
    };
    
    Packet next();
    Packet current();
    void reset();
    static char* asDisplayString(short flag);
    short asDisplayFlag();

    // vars/constants
    static const short FLAG_DEVICE_ID = (1 << 0);
    static const short FLAG_LENGTH    = (1 << 1);
    static const short FLAG_PAYLOAD   = (1 << 2);
    static const short FLAG_CHECKSUM  = (1 << 3);
    static const short FLAG_ACK_NACK  = (1 << 4);
    
    // from PJON.h
    static const short PJON_ACK_VALUE  = 0x06;
    static const short PJON_NACK_VALUE = 0x15; // 21 decimal

    //define BROADCAST_ADDRESS 124

    // subtract this number from the wire-level payload packet for actual data length
    static const short PJON_PACKET_OVERHEAD = 3;
    
private:
    
    // ugh, c++ enums, i'm looking at you :[
    static const short enum_element_count = 5;
    
    Packet pjon_packet_types[enum_element_count] = {
        Packet::DeviceId,
        Packet::Length,
        Packet::Payload,
        Packet::Checksum,
        Packet::AckNack
    };
    
    // vars/constants
    short current_index = 0;
};

#endif
