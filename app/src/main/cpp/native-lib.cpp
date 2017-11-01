#include <jni.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <android/bitmap.h>
#include "bitmap.h"
#include <math.h>
//#include "pb_algorithm.h"
#include <android/log.h>

#include "pb_algorithm.h"
#include "pb_template.h"
#include "pb_image.h"
#include "pb_user.h"
#include "pb_finger.h"
#include "pb_returncodes.h"
#include "pb_product_information.h"
#include "pbpng.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define SVF_DLY2X_OSC        svf10_resister_frame[0]
#define SVF_S_GAIN           svf10_resister_frame[1]
#define SVF_ADC_RSEL         svf10_resister_frame[2]
#define SVF_DYL2X            svf10_resister_frame[3]
#define SVF_CLK_SEL          svf10_resister_frame[4]
#define SVF_PERIOD           svf10_resister_frame[5]
#define SVF_ISOL             svf10_resister_frame[6]
#define SVF_SL_TIME          svf10_resister_frame[7]
#define SVF_NAVI             svf10_resister_frame[8]
#define SVF_SCK_UT           svf10_resister_frame[9]
#define SVF_SLV_STU          svf10_resister_frame[10]
#define SVF_MULTIPLYING      svf10_resister_frame[11]
#define SVF_ADC_REF          svf10_resister_frame[12]
#define SVF_DISPLAY_THRESHOLD   svf10_resister_frame[13]
#define SVF_GPB_REG          svf10_resister_frame[15]

#define OSC_10MHZ            0
#define OSC_5MHZ             1

#define GAIN_1X              0
#define GAIN_1X2             1
#define GAIN_1X5             2
#define GAIN_2X              3

#define RSEL_INTERAL         0
#define RSEL_EXTERNAL        1

#define DYL2X_10NS           0
#define DYL2X_15NS           1

#define CK_SYS               0
#define CK_DIV_2             1
#define CK_DIV_4             2
#define CK_DIV_8             3

#define PERIOD_000           0
#define PERIOD_001           1
#define PERIOD_010           2
#define PERIOD_011           3
#define PERIOD_100           4
#define PERIOD_101           5
#define PERIOD_110           6

#define ISOL_6LINE           0
#define ISOL_10LINE          1

#define SL_DEEP              0
#define SL_50MS              1
#define SL_100MS             2
#define SL_150MS             3
#define SL_300MS             4
#define SL_500MS             5
#define SL_700MS             6
#define SL_1000MS            7

#define NAVI_OFF             0
#define NAVI_ON              1

#define SCK_UT_8INT          0
#define SCK_UT_16INT         1

#define SLV_SUT_CAP_O        0
#define SLV_SUT_CAP_N        1
#define SLV_SUT_SLEEP        2
#define SLV_SUT_MEM_R        3

#define AUTO_SAVE
#define AUTO_SAVE_FRAME      3

uint16_t svf10_x_pixel = 96;
uint16_t svf10_y_pixel = 96;
uint16_t svf10_receive_count = 9412;
uint16_t svf10_header_count = 10296;
uint8_t svf10_origine_buffer[9412];
//uint8_t svf10_origine_buffer[2048];

static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay;
static uint8_t order;

/* Private variables ---------------------------------------------------------*/
uint8_t svf10_chip_id[8];
uint8_t svf10_status_buffer[8];
uint16_t origine_image_count = 4;
uint16_t recon_image_count = 1078;

uint8_t uart_receive_frame[18];
uint8_t svf10_resister_frame[15];
uint8_t receive_end_flag = 0;
uint8_t chip_id_buffer[6];
uint8_t test_buffer[2];

/* variables for Histogram Function ------------------------------------------*/

uint8_t image_data[96][96];
double result_data[96][96];
double histogram[256];
double histogram1[256], histogram2[256];
double temp, temp2, temp3, temp4;
uint8_t INDEX;

double average;
int16_t         N_Capture_Frame;

uint16_t display_threshold = 300;

extern "C"
JNIEXPORT int JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_test(
        JNIEnv *env,
        jobject /* this */) {
    int ret = 6.0 * 100;
    return ret;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

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

uint8_t ConvertLSBtoMSB(uint8_t input)
{
    uint8_t ret = input;

    ret = (ret & 0xF0) >> 4 | (ret & 0x0F) << 4;
    ret = (ret & 0xCC) >> 2 | (ret & 0x33) << 2;
    ret = (ret & 0xAA) >> 1 | (ret & 0x55) << 1;

    return ret;
}

std::string SVF10_Chip_ID_Read(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x10,0x00};
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

//    HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
//    HAL_SPI_Receive(&hspi1,svf10_chip_id,8,10);

    int ret;

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    struct spi_ioc_transfer tr[2] = {{
             .tx_buf = (unsigned long) spi_tx_buffer,
             .rx_buf = (unsigned long) rx,
             .len = ARRAY_SIZE(spi_tx_buffer),
             .delay_usecs = delay,
             .speed_hz = speed,
             .bits_per_word = bits,
     }, {
            .tx_buf = NULL,
            .rx_buf = (unsigned long) svf10_chip_id,
            .len = ARRAY_SIZE(svf10_chip_id),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    }};

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    for(int i = ARRAY_SIZE(svf10_chip_id); i > 0; i--) {
        svf10_chip_id[i] = (svf10_chip_id[i] >> 1) | (svf10_chip_id[i - 1] & 0x01) << 7;
        svf10_chip_id[i] = ConvertLSBtoMSB(svf10_chip_id[i]);
    }
    svf10_chip_id[0] = svf10_chip_id[0] >> 1;
    svf10_chip_id[0] = ConvertLSBtoMSB(svf10_chip_id[0]);

    for (ret = 0; ret < ARRAY_SIZE(svf10_chip_id); ret++)
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X,[%c] ", svf10_chip_id[ret], svf10_chip_id[ret]);
    return (char *)"Chip_ID_READ_SUCCESS\n";
}

std::string SVF10_Sreg_Set(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x00,0x00};

    spi_tx_buffer[0] = (SVF_SL_TIME << 7) | (SVF_NAVI << 6) | (SVF_SCK_UT << 5) | ((SVF_SLV_STU*2) <<2);
    spi_tx_buffer[1] = 0x70 | (SVF_ISOL << 2) | (SVF_SL_TIME >> 1);
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);
    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);

    test_buffer[0] = spi_tx_buffer[0];
    test_buffer[1] = spi_tx_buffer[1];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)spi_tx_buffer,
            .rx_buf = (unsigned long)rx,
            .len = ARRAY_SIZE(spi_tx_buffer),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    return (char *)"SVF10_Sreg_Set_SUCCESS\n";
}

std::string SVF10_Sreg_Read_Creg_Set(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x00,0x00};

    spi_tx_buffer[0] = (SVF_DYL2X << 7) | (SVF_PERIOD << 2);
    spi_tx_buffer[1] = 0x60 | (SVF_DLY2X_OSC << 3) | (SVF_S_GAIN << 1) | SVF_ADC_RSEL ;
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    //HAL_SPI_Receive(&hspi1,svf10_status_buffer,5,10);

    test_buffer[0] = spi_tx_buffer[0];
    test_buffer[1] = spi_tx_buffer[1];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr[2] = {{
        .tx_buf = (unsigned long) spi_tx_buffer,
        .rx_buf = (unsigned long) rx,
        .len = ARRAY_SIZE(spi_tx_buffer),
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    },{
        .tx_buf = NULL,
        .rx_buf = (unsigned long) svf10_status_buffer,
        .len = ARRAY_SIZE(svf10_status_buffer),
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    }};

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    for(int i = ARRAY_SIZE(svf10_status_buffer); i > 0; i--) {
        svf10_status_buffer[i] = (svf10_status_buffer[i] >> 1) | (svf10_status_buffer[i - 1] & 0x01) << 7;
        svf10_status_buffer[i] = ConvertLSBtoMSB(svf10_status_buffer[i]);
    }
    svf10_status_buffer[0] = svf10_status_buffer[0] >> 1;
    svf10_status_buffer[0] = ConvertLSBtoMSB(svf10_status_buffer[0]);

    //for (ret = 0; ret < ARRAY_SIZE(svf10_status_buffer); ret++)
    //    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X,[%c] ", svf10_status_buffer[ret], svf10_status_buffer[ret]);

    return (char *)"SVF10_Sreg_Read_Creg_Set_SUCCESS\n";
}

std::string SVF10_Memory_Read_Mode5(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x3e,0x00};

    spi_tx_buffer[0] = (SVF_MULTIPLYING << 2);
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    //HAL_SPI_Receive(&hspi1,svf10_origine_buffer,svf10_receive_count,500);

    test_buffer[0] = spi_tx_buffer[0];
    test_buffer[1] = spi_tx_buffer[1];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr[2] = {{
         .tx_buf = (unsigned long) spi_tx_buffer,
         .rx_buf = (unsigned long) rx,
         .len = ARRAY_SIZE(spi_tx_buffer),
         .delay_usecs = delay,
         .speed_hz = speed,
         .bits_per_word = bits,
    },{
         .tx_buf = NULL,
         .rx_buf = (unsigned long) svf10_origine_buffer,
         .len = ARRAY_SIZE(svf10_origine_buffer),
         .delay_usecs = delay,
         .speed_hz = speed,
         .bits_per_word = bits,
    }};

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    for(int i = ARRAY_SIZE(svf10_origine_buffer); i > 0; i--) {
        svf10_origine_buffer[i] = (svf10_origine_buffer[i] >> 1) | (svf10_origine_buffer[i - 1] & 0x01) << 7;
        svf10_origine_buffer[i] = ConvertLSBtoMSB(svf10_origine_buffer[i]);
    }
    svf10_origine_buffer[0] = svf10_origine_buffer[0] >> 1;
    svf10_origine_buffer[0] = ConvertLSBtoMSB(svf10_origine_buffer[0]);

    //for (ret = 0; ret < ARRAY_SIZE(svf10_origine_buffer); ret++)
    //    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X,[%c] ", svf10_origine_buffer[ret], svf10_origine_buffer[ret]);
    return (char *)"SVF10_Memory_Read_Mode5_SUCCESS\n";
}

std::string SVF10_Capture_Offset(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x00,0x00};
    spi_tx_buffer[0] = (SVF_DYL2X << 7) | (SVF_CLK_SEL << 5) | (SVF_PERIOD << 2);
    spi_tx_buffer[1] = (SVF_DLY2X_OSC << 3) | (SVF_S_GAIN << 1) | SVF_ADC_RSEL ;

    uint8_t spi_test_buffer[]={0x00};
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);
    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    spi_test_buffer[0] = spi_tx_buffer[0];
    test_buffer[0] = spi_tx_buffer[1];
    test_buffer[1] = spi_tx_buffer[2];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr[2] = {{
         .tx_buf = (unsigned long)spi_test_buffer,
         .rx_buf = (unsigned long)rx,
         .len = ARRAY_SIZE(spi_test_buffer),
         .delay_usecs = 1000,
         .speed_hz = speed,
         .bits_per_word = bits,
    },{
        .tx_buf = (unsigned long)test_buffer,
        .rx_buf = (unsigned long)rx,
        .len = ARRAY_SIZE(test_buffer),
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    }};

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    return (char *)"SVF10_Capture_Offset_SUCCESS\n";
}

std::string SVF10_Status_write(int fd)
{
    uint8_t spi_tx_buffer[]={0x20,0x76,0x00};
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)spi_tx_buffer,
            .rx_buf = (unsigned long)rx,
            .len = ARRAY_SIZE(spi_tx_buffer),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

/*
    for (ret = 0; ret < ARRAY_SIZE(rx); ret++) {
        if (!(ret % 6))
            __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "");
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X ", rx[ret]);
    }
    */
    return (char *)"SVF10_Status_write_SUCCESS\n";
}

std::string SVF10_Status_read(int fd)
{
    uint8_t spi_tx_buffer[]={0x10,0x6a,0x00};
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    //HAL_SPI_Receive(&hspi1,svf10_status_buffer,5,10);

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr[2] = {{
         .tx_buf = (unsigned long) spi_tx_buffer,
         .rx_buf = (unsigned long) rx,
         .len = ARRAY_SIZE(spi_tx_buffer),
         .delay_usecs = delay,
         .speed_hz = speed,
         .bits_per_word = bits,
    },{
         .tx_buf = NULL,
         .rx_buf = (unsigned long) svf10_status_buffer,
         .len = ARRAY_SIZE(svf10_status_buffer),
         .delay_usecs = delay,
         .speed_hz = speed,
         .bits_per_word = bits,
    }};

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    for(int i = ARRAY_SIZE(svf10_status_buffer); i > 0; i--) {
        svf10_status_buffer[i] = (svf10_status_buffer[i] >> 1) | (svf10_status_buffer[i - 1] & 0x01) << 7;
        svf10_status_buffer[i] = ConvertLSBtoMSB(svf10_status_buffer[i]);
    }
    svf10_status_buffer[0] = svf10_status_buffer[0] >> 1;
    svf10_status_buffer[0] = ConvertLSBtoMSB(svf10_status_buffer[0]);

    for (ret = 0; ret < ARRAY_SIZE(svf10_status_buffer); ret++)
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X,[%c] ", svf10_status_buffer[ret], svf10_status_buffer[ret]);
    return (char *)"SVF10_Status_read_SUCCESS\n";
}

std::string SVF10_Sleep_Mode(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x51,0x00};
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)spi_tx_buffer,
            .rx_buf = (unsigned long)rx,
            .len = ARRAY_SIZE(spi_tx_buffer),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    return (char *)"SVF10_Sleep_Mode_SUCCESS\n";
}

std::string SVF10_Sleep_Sens(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x00,0x00};

    spi_tx_buffer[0] = (SVF_ADC_REF << 2);
    spi_tx_buffer[1] = 0x40 | (SVF_PERIOD << 2) | (SVF_ADC_REF >> 6);
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);

    test_buffer[0] = spi_tx_buffer[0];
    test_buffer[1] = spi_tx_buffer[1];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)spi_tx_buffer,
            .rx_buf = (unsigned long)rx,
            .len = ARRAY_SIZE(spi_tx_buffer),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    return (char *)"SVF10_Sleep_Sens_SUCCESS\n";
}

std::string SVF10_Memory_Read_Mode0(int fd)
{
    uint8_t spi_tx_buffer[]={0x00,0x30,0x00};

    spi_tx_buffer[0] = (SVF_MULTIPLYING << 2);
    for (int i = 0; i < 2; i++)
        spi_tx_buffer[i] = ConvertLSBtoMSB(spi_tx_buffer[i]);

    //HAL_SPI_Transmit(&hspi1,spi_tx_buffer,3,10);
    //HAL_SPI_Receive(&hspi1,svf10_origine_buffer,svf10_receive_count,500);

    test_buffer[0] = spi_tx_buffer[0];
    test_buffer[1] = spi_tx_buffer[1];

    uint8_t rx[ARRAY_SIZE(spi_tx_buffer)] = {0, };
    int ret;
    struct spi_ioc_transfer tr[2] = {{
             .tx_buf = (unsigned long) spi_tx_buffer,
             .rx_buf = (unsigned long) rx,
             .len = ARRAY_SIZE(spi_tx_buffer),
             .delay_usecs = delay,
             .speed_hz = speed,
             .bits_per_word = bits,
     },{
            .tx_buf = NULL,
            .rx_buf = (unsigned long) svf10_origine_buffer,
            .len = ARRAY_SIZE(svf10_origine_buffer),
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    } };

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret < 1)
        return (char *)"Error ioctl";

    //for(int i = ARRAY_SIZE(svf10_origine_buffer); i > 0; i--) {
    for(int i = 0; i < ARRAY_SIZE(svf10_origine_buffer); i++) {
        //svf10_origine_buffer[i] = (svf10_origine_buffer[i] >> 1) | (svf10_origine_buffer[i - 1] & 0x01) << 7;
        svf10_origine_buffer[i] = 255 - ConvertLSBtoMSB(svf10_origine_buffer[i]);
    }
    //svf10_origine_buffer[0] = svf10_origine_buffer[0] >> 1;
    //svf10_origine_buffer[0] = ConvertLSBtoMSB(svf10_origine_buffer[0]);

    //for (ret = 0; ret < ARRAY_SIZE(svf10_origine_buffer); ret++)
    for (ret = 0; ret < 16; ret++)
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "%.2X,[%c] ", svf10_origine_buffer[ret], svf10_origine_buffer[ret]);

    return (char *)"SVF10_Memory_Read_Mode0SUCCESS\n";
}

std::string SVF10_Send_Image(int fd)
{
    origine_image_count = 4;
    recon_image_count = 1078;

    for(uint16_t i=0; i< svf10_y_pixel; i++)
    {
        for(uint16_t j = 0; j< svf10_x_pixel ; j++)
        {
            bitmap_header_data[recon_image_count] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
            recon_image_count++;
        }
        origine_image_count +=2;
    }

    //HAL_UART_Transmit(&huart4,bitmap_header_data,svf10_header_count,2000);
}

void SVF10_Send_Image_mark(void)
{
    origine_image_count = 4;
    recon_image_count = 1078;

    for(uint16_t i=0; i< svf10_y_pixel; i++)
    {
        for(uint16_t j = 0; j< svf10_x_pixel ; j++)
        {
            bitmap_header_data[recon_image_count] = 255 - bitmap_header_data[recon_image_count] ;
            origine_image_count++;
            recon_image_count++;
        }
        origine_image_count +=2;
    }

    //Save Image File
    //HAL_UART_Transmit(&huart4,bitmap_header_data,svf10_header_count,2000);
}

void image_quality(void)
{
    origine_image_count = 4;
    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
        }
        origine_image_count +=2;
    }

    temp=0.0;
    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            temp=temp+image_data[i][j];
        }
    }

    temp = temp / 96.0 / 96.0;

    temp2 = 0.0;
    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            temp2=temp2+(image_data[i][j]-temp)*(image_data[i][j]-temp);
        }
    }
    temp2 = temp2 / 96.0 / 96.0;
}

void moving_aver_by2(void)
{

    int16_t i_pre, j_pre;
    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            result_data[i][j] = 0;
        }
    }


    for (uint16_t i = 0; i<96; i++)
    {
        i_pre = i - 1;
        if (i_pre < 0)
            i_pre = 0;
        for (uint16_t j = 0; j<96; j++)
        {
            j_pre = j - 1;
            if (j_pre < 0)
                j_pre = 0;
            result_data[i][j] = 1.0 / 4.0 *(image_data[i_pre][j_pre] + image_data[i_pre][j] +
                                            image_data[i][j_pre] + image_data[i][j]);
        }
    }


    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            svf10_origine_buffer[origine_image_count] = (uint8_t)result_data[i][j];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

}

void moving_aver_by3(void)
{

    int16_t i_pre, j_pre, i_post, j_post;
    origine_image_count = 4;
    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
        }
        origine_image_count +=2;
    }

    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            result_data[i][j] = 0;
        }
    }


    for(uint16_t i=0; i<96; i++)
    {
        i_pre = i - 1;
        if (i_pre < 0)
            i_pre = 0;
        i_post = i + 1;
        if (i_post > 95)
            i_post = 95;
        for(uint16_t j=0; j<96; j++)
        {
            j_pre = j - 1;
            if (j_pre < 0)
                j_pre = 0;
            j_post = j + 1;
            if (j_post > 95)
                j_post = 95;
            result_data[i][j] = 1.0/9.0 *(image_data[i_pre][j_pre] + image_data[i_pre][j] + image_data[i_pre][j_post] +
                                          image_data[i][j_pre] + image_data[i][j] + image_data[i][j_post] +
                                          image_data[i_post][j_pre] + image_data[i_post][j] + image_data[i_post][j_post]);
        }
    }


    origine_image_count = 4;
    for(uint16_t i=0; i<96; i++)
    {
        for(uint16_t j=0; j<96; j++)
        {
            svf10_origine_buffer[origine_image_count]= (uint8_t) result_data[i][j];
            origine_image_count++;
        }
        origine_image_count +=2;
    }

}

void moving_aver_by4(void)
{

    int16_t i_pre2, i_pre, j_pre2, j_pre, i_post, j_post;
    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            result_data[i][j] = 0;
        }
    }


    for (uint16_t i = 0; i<96; i++)
    {
        i_pre2 = i - 2;
        if (i_pre2 < 0)
            i_pre2 = 0;
        i_pre = i - 1;
        if (i_pre < 0)
            i_pre = 0;
        i_post = i + 1;
        if (i_post > 95)
            i_post = 95;

        for (uint16_t j = 0; j<96; j++)
        {
            j_pre2 = j - 2;
            if (j_pre2 < 0)
                j_pre2 = 0;
            j_pre = j - 1;
            if (j_pre < 0)
                j_pre = 0;
            j_post = j + 1;
            if (j_post > 95)
                j_post = 95;

            result_data[i][j] = 1.0 / 16.0 *(image_data[i_pre2][j_pre2] + image_data[i_pre2][j_pre] + image_data[i_pre2][j] + image_data[i_pre2][j_post] +
                                             image_data[i_pre][j_pre2] + image_data[i_pre][j_pre] + image_data[i_pre][j] + image_data[i_pre][j_post] +
                                             image_data[i][j_pre2] + image_data[i][j_pre] + image_data[i][j] + image_data[i][j_post] +
                                             image_data[i_post][j_pre2] + image_data[i_post][j_pre] + image_data[i_post][j] + image_data[i_post][j_post]);

        }
    }
    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            svf10_origine_buffer[origine_image_count] = (uint8_t) result_data[i][j];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

}

void hist_eq(void)
{
    uint8_t gray;

    for (uint16_t i = 0; i < 256; i++)
        histogram[i] = 0;

    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            gray = image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
            histogram[gray] = histogram[gray] + 1;
        }
        origine_image_count += 2;
    }

    for (uint16_t i = 0; i < 256; i++)
        histogram[i] = histogram[i] / 96.0 / 96.0 * 255.0;

    for (uint16_t i = 1; i < 256; i++)
        histogram[i] = histogram[i - 1] + histogram[i];

    for (uint16_t i = 1; i < 256; i++)
        histogram[i] = (uint8_t) histogram[i];

    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            svf10_origine_buffer[origine_image_count] = (uint8_t) histogram[(uint8_t) image_data[i][j]];
            origine_image_count++;
        }
        origine_image_count += 2;
    }
}

void histo(void)
{
    uint8_t gray;

    for (uint16_t i = 0; i < 256; i++)
        histogram[i] = 0;

    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            gray = image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
            histogram[gray] = histogram[gray] + 1;
        }
        origine_image_count += 2;
    }

    for (uint16_t i = 0; i < 256; i++)
        histogram[i] = histogram[i] / 96.0 / 96.0;

    for (uint16_t i = 1; i < 256; i++)
        histogram[i] = histogram[i - 1] + histogram[i];
}

void gaussian_filter_by3(void)
{
    float factor[3][3], sum = 0;
    int16_t i_pre, j_pre, i_post, j_post;
    origine_image_count = 4;

    sum = 0;
    for (uint16_t i = 0; i < 3; i++)
    {
        for (uint16_t j = 1; j < 3; j++)
        {
            factor[i][j] = exp(-((i - 1.0)*(i - 1.0) + (j - 1.0)*(j - 1.0)) / (2.0*2.0));
            sum = sum + factor[i][j];
        }

    }

    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            image_data[i][j] = svf10_origine_buffer[origine_image_count];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            result_data[i][j] = 0;
        }
    }


    for (uint16_t i = 0; i<96; i++)
    {
        i_pre = i - 1;
        if (i_pre < 0)
            i_pre = 0;
        i_post = i + 1;
        if (i_post > 95)
            i_post = 95;
        for (uint16_t j = 0; j<96; j++)
        {
            j_pre = j - 1;
            if (j_pre < 0)
                j_pre = 0;
            j_post = j + 1;
            if (j_post > 95)
                j_post = 95;
            result_data[i][j] = 1.0 / sum *(factor[0][0]*image_data[i_pre][j_pre] + factor[0][1]*image_data[i_pre][j] + factor[0][2]*image_data[i_pre][j_post] +
                                            factor[1][0]*image_data[i][j_pre] + factor[1][1]*image_data[i][j] + factor[1][2]*image_data[i][j_post] +
                                            factor[2][0]*image_data[i_post][j_pre] + factor[2][1]*image_data[i_post][j] + factor[2][2]*image_data[i_post][j_post]);
        }
    }


    origine_image_count = 4;
    for (uint16_t i = 0; i<96; i++)
    {
        for (uint16_t j = 0; j<96; j++)
        {
            svf10_origine_buffer[origine_image_count] = (uint8_t)result_data[i][j];
            origine_image_count++;
        }
        origine_image_count += 2;
    }

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_SpiOpen(
        JNIEnv *env,
        jobject,
        int r1,
        int r2,
        int r3,
        int r4,
        int r5,
        int r6,
        int r7,
        int r8,
        int r9,
        int r10,
        int r11,
        int r12,
        int r13,
        int r14
) {

    int ret = 0;
    int fd;
    std::string hello;

    fd = open(device, O_RDWR);
    if (fd < 0) {
        hello = "can't open device\n";
        return env->NewStringUTF(hello.c_str());
    }

    // spi mode
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        hello = "can't set spi mode \n";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1) {
        hello = "can't get spi mode\n";
        return env->NewStringUTF(hello.c_str());
    }

    // bits per word
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        hello = "can't set bits per word\n";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1) {
        hello = "can't get bits per word\n";
        return env->NewStringUTF(hello.c_str());
    }

    order = 0x00;
    ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &order);
    if (ret == -1) {
        hello = "SPI Write LSB failed\n";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &order);
    if (ret == -1) {
        hello = "SPI WRite LSB Failed\n";
        return env->NewStringUTF(hello.c_str());
    }

    //max speed hz
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        hello = "can't set max speed hz\n";
        return env->NewStringUTF(hello.c_str());
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        hello = "can't get max speed hz\n";
        return env->NewStringUTF(hello.c_str());
    }

    chip_id_buffer[0] = 'S';
    chip_id_buffer[1] = 'V';
    chip_id_buffer[2] = 'F';
    chip_id_buffer[3] = '1';
    chip_id_buffer[4] = '0';
    chip_id_buffer[5] = 'P';

    SVF_DLY2X_OSC = (uint8_t)r1;
    SVF_S_GAIN = (uint8_t)r2;
    SVF_ADC_RSEL = (uint8_t)r3;
    SVF_DYL2X = (uint8_t)r4;
    SVF_CLK_SEL = (uint8_t)r5;
    SVF_PERIOD = (uint8_t)r6;
    SVF_ISOL = (uint8_t)r7;
    SVF_SL_TIME = (uint8_t)r8;
    SVF_NAVI = (uint8_t)r9;
    SVF_SCK_UT = (uint8_t)r10;
    SVF_SLV_STU = (uint8_t)r11;
    SVF_MULTIPLYING = (uint8_t)r12;
    SVF_ADC_REF = (uint8_t)r13;
    SVF_GPB_REG = 0xf0;
    SVF_DISPLAY_THRESHOLD = (uint8_t)r14;

    //display_threshold = 300;   // value/100

    hello = SVF10_Chip_ID_Read(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Read_Creg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Memory_Read_Mode5(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Set(fd);
    usleep(1000*1);
    //for (int i=0; i < 10; i++) {
        hello += SVF10_Capture_Offset(fd);
        usleep(1000 * 1000);
    //}
    close(fd);

    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT int JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_FPgetTemp2(
        JNIEnv *env,
        jobject obj) {

    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "Call FPgetTemp2 Success!");
    int ret = 0;
    int fd;
    std::string hello;

    fd = open(device, O_RDWR);
    if (fd < 0) {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "can't open device");
    }

    hello = SVF10_Sreg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Read_Creg_Set(fd);
    usleep(1000*1);
//    hello += SVF10_Sreg_Set(fd);
//    usleep(1000*1);
    hello += SVF10_Sleep_Sens(fd);
    usleep(1000*200);
    hello += SVF10_Memory_Read_Mode0(fd);
    moving_aver_by3();
    image_quality();

    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "Return FPgetTemp2 Success!");
    return (int)temp2*100;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_FPLoop(
        JNIEnv *env,
        jobject obj,
        jobject bitmap) {

    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "Call FPLoop Success!");
    int ret = 0;
    int fd;
    std::string hello;

    fd = open(device, O_RDWR);
    if (fd < 0) {
        hello = "can't open device";
        return env->NewStringUTF(hello.c_str());
    }

    hello = SVF10_Sreg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Read_Creg_Set(fd);
    usleep(1000*1);
//    hello += SVF10_Sreg_Set(fd);
//    usleep(1000*1);
    hello += SVF10_Sleep_Sens(fd);
    usleep(1000*200);
    hello += SVF10_Memory_Read_Mode0(fd);
/*
    FILE* file = fopen("/sdcard/hello.txt","w+");

    if (file != NULL)
    {
        for (ret = 0; ret < ARRAY_SIZE(svf10_origine_buffer); ret++)
            fprintf(file, "%.2X", svf10_origine_buffer[ret]);
        fflush(file);
        fclose(file);
    }
*/
    image_quality();

/*
    moving_aver_by3();
    //average = temp2;

//    if (temp2 > ((float)(display_threshold)/100))
//    {
//      N_Capture_Frame++;
      moving_aver_by2();
      hist_eq();
      gaussian_filter_by3();

    AndroidBitmapInfo info;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "width:%d height:%d stride:%d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "Bitmap format is not RGBA_8888!");
        return NULL;
    }

    void* bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_lockPixels() 11 failed ! error=%d", ret);
        return NULL;
    }

    jclass bitmapCls = env->GetObjectClass(bitmap);
    jmethodID recycleFunction = env->GetMethodID(bitmapCls, "recycle", "()V");
    if (recycleFunction == 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "error recycling!");
        return NULL;
    }
    env->CallVoidMethod(bitmap, recycleFunction);

    AndroidBitmap_unlockPixels(env, bitmap);
    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //

    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "creating new bitmap...");

    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.height, info.width, bitmapConfig);


    //uint32_t* src = (uint32_t*) bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_lockPixels() 22 failed ! error=%d", ret);
        return NULL;
    }

    int whereToPut = 0;
    int cnt = 4;
    uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
    for (int y = 0; y < info.height; y++) {
        for (int x = 0; x < info.width; x++) {
            uint32_t pixel =
                    0xFF << 24 | (svf10_origine_buffer[cnt] << 16) |
                    (svf10_origine_buffer[cnt] << 8) | svf10_origine_buffer[cnt];

            newBitmapPixels[whereToPut++] = pixel;
            cnt++;
        }
        cnt += 2;
    }
    AndroidBitmap_unlockPixels(env, newBitmap);
    close(fd);
*/
    /*
#ifdef AUTO_SAVE
      if (N_Capture_Frame == AUTO_SAVE_FRAME)
      {
        //SVF10_Send_Image();


      }
#else
      SVF10_Send_Image();
#endif
    }
    else
    {
      N_Capture_Frame = 0;
    }

#ifndef AUTO_SAVE
    #ifdef MATLAB_TEST
      HAL_UART_Transmit(&huart4, svf10_origine_buffer, 9412, 2000);
    #else
      SVF10_Send_Image();
    #endif
#endif
     */
    if (temp2 > SVF_DISPLAY_THRESHOLD) {
        moving_aver_by4();
        hist_eq();
    }
    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "temp = %f, Threshold = %d", temp2, SVF_DISPLAY_THRESHOLD);



    AndroidBitmapInfo info;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "width:%d height:%d stride:%d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "Bitmap format is not RGBA_8888!");
        return NULL;
    }

    void* bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_lockPixels() 11 failed ! error=%d", ret);
        return NULL;
    }

    jclass bitmapCls = env->GetObjectClass(bitmap);
    jmethodID recycleFunction = env->GetMethodID(bitmapCls, "recycle", "()V");
    if (recycleFunction == 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "error recycling!");
        return NULL;
    }
    env->CallVoidMethod(bitmap, recycleFunction);

    AndroidBitmap_unlockPixels(env, bitmap);
    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //

    __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "creating new bitmap...");

    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.height, info.width, bitmapConfig);


    //uint32_t* src = (uint32_t*) bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "ShinJAE", "AndroidBitmap_lockPixels() 22 failed ! error=%d", ret);
        return NULL;
    }

    int whereToPut = 0;
    int cnt = 4;
    uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
    for (int y = 0; y < info.height; y++) {
        for (int x = 0; x < info.width; x++) {
            uint32_t pixel =
                    0xFF << 24 | (svf10_origine_buffer[cnt] << 16) |
                    (svf10_origine_buffer[cnt] << 8) | svf10_origine_buffer[cnt];

            newBitmapPixels[whereToPut++] = pixel;
            cnt++;
        }
        cnt += 2;
    }
    AndroidBitmap_unlockPixels(env, newBitmap);

    //display_threshold = SVF_DISPLAY_THRESHOLD;
/*
    hello += SVF10_Sreg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Read_Creg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Memory_Read_Mode5(fd);
    usleep(1000*1);
    hello += SVF10_Sreg_Set(fd);
    usleep(1000*1);
    hello += SVF10_Capture_Offset(fd);
    usleep(1000*500);
*/
    //hello += SVF10_Send_Image(fd);
/*
    if (receive_end_flag) {

        receive_end_flag = 0;
        if ((uart_receive_frame[0] == 'R') && (uart_receive_frame[1] == 'E') &&
            (uart_receive_frame[2] == 'S')) {
            for (uint8_t i = 0; i < 18; i++) {
                svf10_resister_frame[i] = uart_receive_frame[i + 3];
            }
        }

        hello += SVF10_Sreg_Set(fd);

        hello += SVF10_Sreg_Read_Creg_Set(fd);

        hello += SVF10_Memory_Read_Mode5(fd);

        hello += SVF10_Sreg_Set(fd);

        hello += SVF10_Capture_Offset(fd);
    }
*/

    close(fd);

    return newBitmap;

    //return env->NewStringUTF(hello.c_str());
}

/*
extern "C"
JNIEXPORT jstring JNICALL
Java_com_senvis_finger_senvisdemo_MainActivity_PBTest(
        JNIEnv *env,
        jobject,
        int id) {

    pb_user_t* user;
    int rev;
    char buf[128] = {0,};
    rev = pb_prod_rev();

    user = pb_user_create(id);

    sprintf(buf, "Version %s (%d.%d.%d.%d)\nHello BMF user %d\n", pb_prod_revision(),
            PB_PROD_MAJOR(rev), PB_PROD_MINOR(rev),
            PB_PROD_RELEA(rev), PB_PROD_PATCH(rev), pb_user_get_id(user));

    pb_user_delete(user);

    std::string hello = buf;
    return env->NewStringUTF(hello.c_str());
}
*/