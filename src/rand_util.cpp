#include "ashe/rand_util.h"
#include "ashe/check_failure.h"
#ifdef ASHE_POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <cmath>

namespace ashe {
#ifdef ASHE_POSIX
#define HANDLE_EINTR(x) ({                                  \
    decltype(x) eintr_wrapper_result;                       \
    do {                                                    \
        eintr_wrapper_result = (x);                         \
    } while (eintr_wrapper_result == -1 && errno == EINTR); \
    eintr_wrapper_result;                                   \
})

namespace {
static constexpr int kOpenFlags = O_RDONLY | O_CLOEXEC;

// We keep the file descriptor for /dev/urandom around so we don't need to
// reopen it (which is expensive), and since we may not even be able to reopen
// it if we are later put in a sandbox. This class wraps the file descriptor so
// we can use a static-local variable to handle opening it on the first access.
class URandomFd {
   public:
    URandomFd() :
        fd_(HANDLE_EINTR(open("/dev/urandom", kOpenFlags))) {
        ASHE_CHECK_FAILURE(fd_ >= 0, "Cannot open /dev/urandom");
    }

    ~URandomFd() { close(fd_); }

    int fd() const { return fd_; }

   private:
    const int fd_;
};
}  // namespace
#endif

#ifdef ASHE_WIN
// Prototype for ProcessPrng.
// See: https://learn.microsoft.com/en-us/windows/win32/seccng/processprng
extern "C" {
BOOL WINAPI ProcessPrng(PBYTE pbData, SIZE_T cbData);
}

// Import bcryptprimitives!ProcessPrng rather than cryptbase!RtlGenRandom to
// avoid opening a handle to \\Device\KsecDD in the renderer.
decltype(&ProcessPrng) GetProcessPrng() {
    HMODULE hmod = LoadLibraryW(L"bcryptprimitives.dll");
    ASHE_D_CHECK_FAILURE(hmod, nullptr);
    decltype(&ProcessPrng) process_prng_fn =
        reinterpret_cast<decltype(&ProcessPrng)>(
            GetProcAddress(hmod, "ProcessPrng"));
    ASHE_D_CHECK_FAILURE(process_prng_fn, nullptr);
    return process_prng_fn;
}

void RandBytes2(void* output, size_t output_length) {
    static decltype(&ProcessPrng) process_prng_fn = GetProcessPrng();
    BOOL success =
        process_prng_fn(static_cast<BYTE*>(output), output_length);
    // ProcessPrng is documented to always return TRUE.
    ASHE_D_CHECK_FAILURE(success, nullptr);
}

void RandBytes(void* output, size_t output_length) {
    RandBytes2(output, output_length);
}
#elif ASHE_POSIX

int GetUrandomFD() {
    static URandomFd urandom_fd;
    return urandom_fd.fd();
}

bool ReadFromFD(int fd, void* output, size_t output_length) {
    size_t total_read = 0;
    while (total_read < output_length) {
        ssize_t bytes_read = HANDLE_EINTR(read(fd, (void*)((char*)output + total_read), output_length - total_read));

        if (bytes_read <= 0) {
            return false;
        }
        total_read += bytes_read;
    }
    return true;
}

void RandBytes(void* output, size_t output_length) {
#if ASHE_MACOS
    // TODO(crbug.com/995996): Enable this on iOS too, when sys/random.h arrives
    // in its SDK.
    if (getentropy(output.data(), output.size()) == 0) {
        return;
    }
#endif

    // If the OS-specific mechanisms didn't work, fall through to reading from
    // urandom.
    //
    // TODO(crbug.com/995996): When we no longer need to support old Linux
    // kernels, we can get rid of this /dev/urandom branch altogether.
    const int urandom_fd = GetUrandomFD();
    const bool success = ReadFromFD(urandom_fd, output, output_length);
    ASHE_D_CHECK_FAILURE(success, nullptr);
}
#else
#error Unsupport platform
#endif

uint64_t RandUint64() {
    uint64_t number;
    RandBytes(&number, sizeof(number));
    return number;
}

uint64_t RandGenerator(uint64_t range) {
    ASHE_D_CHECK_FAILURE(range > 0u, nullptr);
    // We must discard random results above this number, as they would
    // make the random generator non-uniform (consider e.g. if
    // MAX_UINT64 was 7 and |range| was 5, then a result of 1 would be twice
    // as likely as a result of 3 or 4).
    uint64_t max_acceptable_value =
        (std::numeric_limits<uint64_t>::max() / range) * range - 1;

    uint64_t value;
    do {
        value = RandUint64();
    } while (value > max_acceptable_value);

    return value % range;
}

int RandInt(int min, int max) {
    ASHE_D_CHECK_FAILURE(min != max, nullptr);

    uint64_t range = static_cast<uint64_t>(max) - static_cast<uint64_t>(min) + 1;
    // |range| is at most UINT_MAX + 1, so the result of RandGenerator(range)
    // is at most UINT_MAX.  Hence it's safe to cast it from uint64_t to int64_t.
    int result =
        static_cast<int>(min + static_cast<int64_t>(RandGenerator(range)));
    ASHE_D_CHECK_FAILURE(result >= min, nullptr);
    ASHE_D_CHECK_FAILURE(result <= max, nullptr);
    return result;
}

double RandDouble() {
    return BitsToOpenEndedUnitInterval(RandUint64());
}

float RandFloat() {
    return BitsToOpenEndedUnitIntervalF(RandUint64());
}

double BitsToOpenEndedUnitInterval(uint64_t bits) {
    // We try to get maximum precision by masking out as many bits as will fit
    // in the target type's mantissa, and raising it to an appropriate power to
    // produce output in the range [0, 1).  For IEEE 754 doubles, the mantissa
    // is expected to accommodate 53 bits (including the implied bit).
    static_assert(std::numeric_limits<double>::radix == 2, "otherwise use scalbn");
    constexpr int kBits = std::numeric_limits<double>::digits;
    return ldexp(bits & ((UINT64_C(1) << kBits) - 1u), -kBits);
}

float BitsToOpenEndedUnitIntervalF(uint64_t bits) {
    // We try to get maximum precision by masking out as many bits as will fit
    // in the target type's mantissa, and raising it to an appropriate power to
    // produce output in the range [0, 1).  For IEEE 754 floats, the mantissa is
    // expected to accommodate 12 bits (including the implied bit).
    static_assert(std::numeric_limits<float>::radix == 2, "otherwise use scalbn");
    constexpr int kBits = std::numeric_limits<float>::digits;
    return ldexpf(bits & ((UINT64_C(1) << kBits) - 1u), -kBits);
}

std::string RandBytesAsString(size_t length) {
    ASHE_D_CHECK_FAILURE(length > 0u, nullptr);
    std::string result(length, '\0');
    RandBytes((void*)result.data(), length);
    return result;
}

std::vector<uint8_t> RandBytesAsVector(size_t length) {
    std::vector<uint8_t> result(length);
    if (result.size()) {
        RandBytes(&result[0], length);
    }
    return result;
}

}  // namespace ashe