/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef CS_FW_SRC_MGOS_UART_H_
#define CS_FW_SRC_MGOS_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common/mbuf.h"
#include "common/platform.h"

#include "fw/src/mgos_init.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct mgos_uart_config {
  int baud_rate;

  int rx_buf_size;
  bool rx_fc_ena;
  int rx_linger_micros;

  int tx_buf_size;
  bool tx_fc_ena;

#if CS_PLATFORM == CS_P_ESP32 || CS_PLATFORM == CS_P_ESP8266
  int rx_fifo_full_thresh;
  int rx_fifo_fc_thresh;
  int rx_fifo_alarm;
  int tx_fifo_empty_thresh;
#if CS_PLATFORM == CS_P_ESP8266
  bool swap_rxcts_txrts;
#endif
#endif
};

bool mgos_uart_configure(int uart_no, const struct mgos_uart_config *cfg);
void mgos_uart_config_set_defaults(int uart_no, struct mgos_uart_config *cfg);

/*
 * UART dispatcher gets when there is data in the input buffer
 * or space available in the output buffer.
 */
typedef void (*mgos_uart_dispatcher_t)(int uart_no, void *arg);
void mgos_uart_set_dispatcher(int uart_no, mgos_uart_dispatcher_t cb,
                              void *arg);

/*
 * Write data to the UART.
 * Note: if there is enough space in the output buffer, the call will return
 * immediately, otherwise it will wait for buffer to drain.
 * If you want the call to not block, check mgos_uart_write_avail() first.
 */
size_t mgos_uart_write(int uart_no, const void *buf, size_t len);
/* Returns amount of space availabe in the output buffer. */
size_t mgos_uart_write_avail(int uart_no);

/*
 * Read data from UART input buffer.
 * The _mbuf variant is a convenice function that reads into an mbuf.
 * Note: unlike write, read will not block if there are not enough bytes in the
 * input buffer.
 */
size_t mgos_uart_read(int uart_no, void *buf, size_t len);
size_t mgos_uart_read_mbuf(int uart_no, struct mbuf *mb, size_t len);
/* Returns the number of bytes available for reading. */
size_t mgos_uart_read_avail(int uart_no);

/* Controls whether UART receiver is enabled. */
void mgos_uart_set_rx_enabled(int uart_no, bool enabled);
bool mgos_uart_rx_enabled(int uart_no);

/* Flush the UART output buffer - waits for data to be sent. */
void mgos_uart_flush(int uart_no);

void mgos_uart_schedule_dispatcher(int uart_no, bool from_isr);

struct mgos_uart_stats {
  uint32_t ints;

  uint32_t rx_ints;
  uint32_t rx_bytes;
  uint32_t rx_overflows;
  uint32_t rx_linger_conts;

  uint32_t tx_ints;
  uint32_t tx_bytes;
  uint32_t tx_throttles;

  void *dev_data;
};

const struct mgos_uart_stats *mgos_uart_get_stats(int uart_no);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CS_FW_SRC_MGOS_UART_H_ */
