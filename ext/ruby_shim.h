#ifndef HEADER__RUBY_SHIM_H__INCLUDED
#define HEADER__RUBY_SHIM_H__INCLUDED

typedef rb_vm_struct {
  VALUE self;

  rb_global_vm_lock gvl;
  rb_nativethread_lock_t thread_destruct_lock;

  struct rb_thread_struct *main_thread;
  struct rb_thread_
}


static inline rb_vm_t *
ruby_current_vm(void) {
  VM_ASSERT(ruby_current_vm_ptr == NULL ||
            ruby_current_execution_context_ptr == NULL ||
            ruby_ec_thread_ptr(GET_EC()) == NULL ||
            ruby_ec_vm_ptr(GET_EC()) == ruby_current_vm_ptr);
  return ruby_current_vm_ptr;
}

extern rb_vm_t *ruby_current_vm_ptr;
extern rb_execution_context_t *ruby_current_execution_context_ptr;
extern rb_event_flag_t ruby_vm_event_flags;

#endif // HEADER__RUBY_SHIM_H__INCLUDED
