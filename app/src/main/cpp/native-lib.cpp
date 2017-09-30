#include <jni.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

static int transfer(int fd)
{
    int ret;
    uint8_t tx[] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
            0xF0, 0x0D,
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)tx,
            .rx_buf = (unsigned long)rx,
            .len = ARRAY_SIZE(tx),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        return -1;

    for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
        if (!(ret % 6))
            puts("");
        printf("%.2X ", rx[ret]);
    }
    puts("");
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_SpiOpen(
        JNIEnv *env,
        jobject /* this */) {

    int ret = 0;
    int fd;
    std::string hello;
    fd = open(device, O_RDWR);
    if (fd < 0) {
        hello = "can't open device";
        return env->NewStringUTF(hello.c_str());
    }
/*
	 * spi mode
	 */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        hello = "can't set spi mode";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1) {
        hello = "can't get spi mode";
        return env->NewStringUTF(hello.c_str());
    }

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        hello = "can't set bits per word";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1) {
        hello = "can't get bits per word";
        return env->NewStringUTF(hello.c_str());
    }

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        hello = "can't set max speed hz";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        hello = "can't get max speed hz";
        return env->NewStringUTF(hello.c_str());
    }

    //printf("spi mode: %d\n", mode);
    //printf("bits per word: %d\n", bits);
    //printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    ret = transfer(fd);
    if (ret == -1) {
        hello = "can't send spi message";
        return env->NewStringUTF(hello.c_str());
    }
    close(fd);

    return env->NewStringUTF(hello.c_str());
}