#include "ashe/config.hpp"
#include "ashe/byteorder.hpp"

bool ashe::ByteOrder::IsHostBigEndian() {
    int nl = 0x12345678;
    short ns = 0x1234;
    bool big_endian = false;

    unsigned char* p = (unsigned char*)(&nl);
    unsigned char* sp = (unsigned char*)(&ns);

    if (p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78) {
        big_endian = true;
    }
    else if (p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12) {
        big_endian = false;
    }
    else {
        big_endian = (*sp != 0x12);
    }

    return big_endian;
}

void ashe::ByteOrder::Set8(void* memory, size_t offset, uint8_t v) {
    static_cast<uint8_t*>(memory)[offset] = v;
}

uint8_t ashe::ByteOrder::Get8(const void* memory, size_t offset) {
    return static_cast<const uint8_t*>(memory)[offset];
}

void ashe::ByteOrder::SetBE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htobe16(v);
}

void ashe::ByteOrder::SetBE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htobe32(v);
}

#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602  // Win8
void ashe::ByteOrder::SetBE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htobe64(v);
}

uint64_t ashe::ByteOrder::GetBE64(const void* memory) {
    return be64toh(*static_cast<const uint64_t*>(memory));
}

uint64_t ashe::ByteOrder::HostToNetwork64(uint64_t n) {
    return htobe64(n);
}

uint64_t ashe::ByteOrder::NetworkToHost64(uint64_t n) {
    return be64toh(n);
}
#endif
uint16_t ashe::ByteOrder::GetBE16(const void* memory) {
    return be16toh(*static_cast<const uint16_t*>(memory));
}

uint32_t ashe::ByteOrder::GetBE32(const void* memory) {
    return be32toh(*static_cast<const uint32_t*>(memory));
}

void ashe::ByteOrder::SetLE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htole16(v);
}

void ashe::ByteOrder::SetLE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htole32(v);
}

void ashe::ByteOrder::SetLE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htole64(v);
}

uint16_t ashe::ByteOrder::GetLE16(const void* memory) {
    return le16toh(*static_cast<const uint16_t*>(memory));
}

uint32_t ashe::ByteOrder::GetLE32(const void* memory) {
    return le32toh(*static_cast<const uint32_t*>(memory));
}

uint64_t ashe::ByteOrder::GetLE64(const void* memory) {
    return le64toh(*static_cast<const uint64_t*>(memory));
}

uint16_t ashe::ByteOrder::HostToNetwork16(uint16_t n) {
    return htobe16(n);
}

uint32_t ashe::ByteOrder::HostToNetwork32(uint32_t n) {
    return htobe32(n);
}

uint16_t ashe::ByteOrder::NetworkToHost16(uint16_t n) {
    return be16toh(n);
}

uint32_t ashe::ByteOrder::NetworkToHost32(uint32_t n) {
    return be32toh(n);
}

void ashe::ByteOrder::ByteSwap(void* inp, size_t size) {
    for (size_t i = 0; i < size / 2; i++) {
        uint8_t t = ((uint8_t*)inp)[size - 1 - i];
        ((uint8_t*)inp)[size - 1 - i] = ((uint8_t*)inp)[i];
        ((uint8_t*)inp)[i] = t;
    }
}
