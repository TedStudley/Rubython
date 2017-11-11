#include "ruby_shims.h"
#include "config.h"

origarg_t *origarg = ORIGARG_ADDR;
void (*Init_prelude)(void) = INIT_PRELUDE_ADDR;
void (*Init_enc)(void) = INIT_ENC_ADDR;
void (*Init_ext)(void) = INIT_EXT_ADDR;
rb_vm_t volatile **ruby_current_vm_p = RUBY_CURRENT_VM_ADDR;
rb_thread_t volatile **ruby_current_thread_p = RUBY_CURRENT_THREAD_ADDR;
int volatile *ruby_setup_initialized_p = RUBY_SETUP_INITIALIZED_ADDR;
st_table volatile **frozen_strings_p = FROZEN_STRINGS_ADDR;
rb_serial_t volatile *ruby_vm_global_method_state_p = RUBY_VM_GLOBAL_METHOD_STATE_ADDR;
rb_serial_t volatile *ruby_vm_global_constant_state_p = RUBY_VM_GLOBAL_CONSTANT_STATE_ADDR;
rb_serial_t volatile *ruby_vm_class_serial_p = RUBY_VM_CLASS_SERIAL_ADDR;
struct symbols *global_symbols_p = GLOBAL_SYMBOLS_ADDR;
struct global_method_cache_t *global_method_cache_p = GLOBAL_METHOD_CACHE_ADDR;
