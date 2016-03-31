#ifndef PJON_PACKET_STATE_H
#define PJON_PACKET_STATE_H

#define DEVICE_ID_FLAG (1 << 0)
#define LENGTH_FLAG    (1 << 1)
#define PAYLOAD_FLAG   (1 << 2)
#define CHECKSUM_FLAG  (1 << 3)
#define ACK_NACK_FLAG  (1 << 4)

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
    
private:
    
    // ugh, c++ enums, i'm looking at you :[
    static const short enum_element_count = 5;
    
    Packet pjon_packet_types[5] = {
        Packet::DeviceId,
        Packet::Length,
        Packet::Payload,
        Packet::Checksum,
        Packet::AckNack
    };
    
    short current_index = 0;
};

#endif
