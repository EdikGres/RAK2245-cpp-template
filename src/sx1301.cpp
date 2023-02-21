#include <iostream>
#include <cstring>

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

extern "C" {
#include "loragw_hal.h"
}

#define MSG(args...) fprintf(stderr, args) /* message that is destined to the user */

#define TX_RF_CHAIN                 0 /* TX only supported on radio A */

struct lgw_conf_board_s boardconf;
struct lgw_conf_rxrf_s rfconf;
static struct lgw_tx_gain_lut_s txgain_lut = {};
struct lgw_pkt_tx_s txpkt;


int main() {

    memset(&boardconf, 0, sizeof(boardconf));
    boardconf.lorawan_public = true;
    boardconf.clksrc = 1; //radio B
    lgw_board_setconf(boardconf);

    rfconf.enable = true;
    rfconf.freq_hz = (uint32_t) ((868 * 1e6) + 0.5);
    rfconf.rssi_offset = 0.0;
    rfconf.type = LGW_RADIO_TYPE_SX1257;

    //разобраться
    for (int i = 0; i < LGW_RF_CHAIN_NB; i++) {
        if (i == TX_RF_CHAIN) {
            rfconf.tx_enable = true;
            rfconf.tx_notch_freq = 129000U;
        } else {
            rfconf.tx_enable = false;
        }
        lgw_rxrf_setconf(i, rfconf);
    }
    txgain_lut.lut[0] = {
            .dig_gain = 0,
            .pa_gain = 0,
            .dac_gain = 3,
            .mix_gain = 12,
            .rf_power = 0
    },
    txgain_lut.lut[1] = {
            .dig_gain = 0,
            .pa_gain = 1,
            .dac_gain = 3,
            .mix_gain = 12,
            .rf_power = 10
    },
    txgain_lut.lut[2] = {
            .dig_gain = 0,
            .pa_gain = 2,
            .dac_gain = 3,
            .mix_gain = 10,
            .rf_power = 14
    },
    txgain_lut.lut[3] = {
            .dig_gain = 0,
            .pa_gain = 3,
            .dac_gain = 3,
            .mix_gain = 9,
            .rf_power = 20
    },
    txgain_lut.lut[4] = {
            .dig_gain = 0,
            .pa_gain = 3,
            .dac_gain = 3,
            .mix_gain = 14,
            .rf_power = 27
    },
    txgain_lut.size = 5;


    lgw_txgain_setconf(&txgain_lut);

    int i = lgw_start();
    if (i == LGW_HAL_SUCCESS) {
        MSG("INFO: concentrator started, packet can be sent\n");
    } else {
        MSG("ERROR: failed to start the concentrator\n");
        return EXIT_FAILURE;
    }

    memset(&txpkt, 0, sizeof(txpkt));
    txpkt.freq_hz = rfconf.freq_hz;
    txpkt.tx_mode = IMMEDIATE;
    txpkt.rf_chain = TX_RF_CHAIN;
    txpkt.rf_power = 14;
    txpkt.modulation = MOD_LORA;
    txpkt.bandwidth = BW_250KHZ;
    txpkt.datarate = DR_LORA_SF8;
    txpkt.coderate = CR_LORA_4_5;
    txpkt.invert_pol = false;
    txpkt.preamble = 12;
    txpkt.size = 50;

    uint8_t text[50];
    for (int j = 0; j < 50; ++j) {
        text[j] = j % 5;
    }
    memcpy(txpkt.payload, text, 50);

    printf("sending packet...\n");

    i = lgw_send(txpkt); /* non-blocking scheduling of TX packet */
    if (i == LGW_HAL_ERROR) {
        printf("ERROR\n");
        return EXIT_FAILURE;
    } else if (LGW_HAL_SUCCESS){
        printf("All OK!!!\n");
    }

    lgw_stop();

    std::cout << lgw_version_info() << std::endl;
    return 0;
}
