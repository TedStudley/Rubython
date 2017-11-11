#ifndef HEADER__RUBY_SHIM_H__INCLUDED
#define HEADER__RUBY_SHIM_H__INCLUDED

#include "ruby/ruby.h"

struct rb_thread_struct;

typedef struct {
} rb_global_vm_lock_t;

typedef struct {
} rb_nativethread_lock_t;

struct list_head {
};

typedef struct {
} rb_hook_list_t;

struct st_table {
};

struct rb_postponed_job_struct {
};

struct unlinked_method_entry_list_entry {
};

typedef struct {
} rb_at_exit_list;

typedef ruby_vm_struct {
  VALUE self;

  rb_global_vm_lock_t gvl;
  rb_nativethread_lock_t thread_destruct_lock;

  struct rb_thread_struct *main_thread;
  struct rb_thread_struct *running_thread;

  struct list_head living_threads;
  size_t living_thread_num;
  VALUE thgroup_default;

  int running;
  int thread_abort_on_exception;
  int trace_running;
  volatile int sleeper;

  VALUE mark_object_ary;

  const VALUE special_exceptions[ruby_special_error_count];

  VALUE top_self;
  VALUE load_path;
  VALUE load_path_snapshot;
  VALUE load_path_check_cache;
  VALUE expanded_load_path;
  VALUE loaded_features;
  VALUE loaded_features_snapshot;
  struct st_table *loaded_features_index;
  struct st_table *loading_table;

  struct {
    VALUE cmd;
    int safe;
  } trap_list[RUBY_NSIG];

  rb_hook_list_t event_hooks;

  struct st_table *ensure_rollback_table;

  struct rb_postponed_job_struct *postponed_job_buffer;
  int postponed_job_index;

  int src_encoding_index;

  VALUE verbose, debug, orig_progname, progname;
  VALUE coverages;

  struct unlinked_method_entry_list_entry *unlinked_method_entry_list;

  VALUE defined_module_hash;

#if defined(ENABLE_VM_OBJSPACE) && ENABLE_VM_OBJSPACE
  struct rb_objspace *objspace;
#endif

  rb_at_exit_list *at_exit;

  VALUE *defined_strings;
  st_table *frozen_strings;

  struct {
    size_t thread_vm_stack_size;
    size_t thread_machine_stack_size;
    size_t fiber_vm_stack_size;
    size_t fiber_machine_stack_size;
  } default_params;

  short redefined_flag[BOP_LAST_];
} rb_vm_t;

typedef struct rb_thread_struct {
  struct list_node vmlt_node;
  VALUE self;
  rb_vm_t *vm;

  VALUE *stack;
  size_t stack_size;
  rb_control_frame_t *cfp;
  int safe_level;
  int raised_flag;
  VALUE last_status;

  int state;
} rb_thread_t;


#endif // HEADER__RUBY_SHIM_H__INCLUDED
