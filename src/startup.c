#include <stdint.h>
#include <string.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// These symbols are defined by the linker script.
// See linker.lds
extern uint8_t __bss_start;
extern uint8_t __bss_end;
extern uint8_t __bss_start;
extern uint8_t __global_pointer;
extern uint8_t __sp;
extern const uint8_t __data_source_start;
extern uint8_t __data_target_start;
extern uint8_t __data_target_end;

extern function_t __init_array_start;
extern function_t __init_array_end;
extern function_t __preinit_array_start;
extern function_t __preinit_array_end;
extern function_t __fini_array_start;
extern function_t __fini_array_end;

// This function will be placed by the linker script according to the section
// Raw function 'called' by the CPU with no runtime.
extern void _enter(void) __attribute__((naked, section(".text.init.enter")));
extern void trap_handler_raw(void)
    __attribute__((interrupt("machine"),
                   section(".text.riscv_trap.trap_handler_raw")));
void trap_handler(void) __attribute__((weak));

// Entry and exit points as C functions.
extern void _start(void) __attribute__((noreturn));
void _Exit(int exit_code) __attribute__((noreturn, noinline));

// Standard entry point, no arguments.
extern int main(void);
extern void SystemInit(void);

void trap_handler_raw() { trap_handler(); }

void trap_handler() {
  while (1) {
  }
}

// The linker script will place this in the reset entry point.
// It will be 'called' with no stack or C runtime configuration.
// NOTE - this only supports a single hart.
// tp will not be initialized
void _enter(void) {
  // Setup SP and GP
  // The locations are defined in the linker script
  asm(".option push\n\t"
      ".option norelax\n\t"
      "lui   gp, %hi(__global_pointer)\n\t"
      "addi   gp,gp, %lo(__global_pointer)\n\t"
      "lui   sp, %hi(__sp)\n\t"
      "addi   sp,sp, %lo(__sp)\n\t"
      "lui ra, %hi(_start)\n\t"
      "jalr ra, %lo(_start)(ra)\n\t"
      ".option pop\n\t");

  // This point will not be executed, _start() will be called with no return.
}

static void _small_delay() {
  asm("li t0, 128000\n\t"
      "start_loop_delay:\n\t"
      "nop\n\t"
      "addi t0, t0, -1\n\t"
      "bnez t0, start_loop_delay\n\t");
}

// At this point we have a stack and global poiner, but no access to global
// variables.
void _start(void) {
  // Init memory regions
  // Clear the .bss section (global variables with no initial values)
  memset((void *)&__bss_start, 0, (&__bss_end - &__bss_start));

  // Initialize the .data section (global variables with initial values)
  memcpy((void *)&__data_target_start, (const void *)&__data_source_start,
         (&__data_target_end - &__data_target_start));

  _small_delay(); // small delay for wait start debugger
  int rc = main();

  _Exit(rc);
}

// This should never be called. Busy loop with the CPU in idle state.
void _Exit(int exit_code) {
  (void)exit_code;
  // Halt
  while (1) {
    __asm__ volatile("wfi");
  }
}