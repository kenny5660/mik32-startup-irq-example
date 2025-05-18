#include <stdint.h>

#define SYSTEM_FREQ_HZ 32000000UL

#define SCR1_TIMER__MTIMEH PREG(0x00490000 + 0xc)
#define SCR1_TIMER__MTIME PREG(0x00490000 + 0x8)
#define SCR1_TIMER_GET_TIME()                                                  \
  (((uint64_t)(SCR1_TIMER__MTIMEH) << 32) | (SCR1_TIMER__MTIME))

#define LED_PIN_NUM (7)
#define LED_PIN_PORT (2)

#define GPIO(N) (0x00084000 + (N) * 0x400)
#define PREG(addr) (*(volatile uint32_t *)(addr))

#define LED_PIN_GPIO__OUTPUT PREG(GPIO(LED_PIN_PORT) + 0x10)
#define LED_PIN_GPIO__DIRECTION_OUT PREG(GPIO(LED_PIN_PORT) + 0x8)
#define PM__CLK_APB_P_SET PREG(0x00050000 + 0x1c)

void delay(uint32_t ms);

int main() {

  /**< Enable clk for all GPIO */
  PM__CLK_APB_P_SET = 0x7000;

  LED_PIN_GPIO__DIRECTION_OUT = 1 << LED_PIN_NUM;
  LED_PIN_GPIO__OUTPUT = 1 << LED_PIN_NUM;

  while (1) {
    LED_PIN_GPIO__OUTPUT ^= 1 << LED_PIN_NUM;
    delay(100);
  }
}

void delay(uint32_t ms) {
  uint64_t end_mtimer = SCR1_TIMER_GET_TIME() + ms * (SYSTEM_FREQ_HZ / 1000);
  while (SCR1_TIMER_GET_TIME() < end_mtimer)
    ;
}