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
    short FLAG_DEVICE_ID = (1 << 0);
    short FLAG_LENGTH    = (1 << 1);
    short FLAG_PAYLOAD   = (1 << 2);
    short FLAG_CHECKSUM  = (1 << 3);
    short FLAG_ACK_NACK  = (1 << 4);
    
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
    
    short current_index = 0;
};

#endif
