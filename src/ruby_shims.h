#ifndef HEADER__RUBY_SHIMS_H__INCLUDED
#define HEADER__RUBY_SHIMS_H__INCLUDED

#define RUBY_VM_THREAD_MODEL 2

#include "ruby.h"
#include "ruby/st.h"

// TODO: Scrape all this crap automatically

typedef struct RNode {
  VALUE flags;
  VALUE nd_reserved; /* ex nd_file */
  union {
    struct RNode *node;
    ID id;
    VALUE value;
    VALUE (*cfunc)(ANYARGS);
    ID *tbl;
  } u1;
  union {
    struct RNode *node;
    ID id;
    long argc;
    VALUE value;
  } u2;
  union {
    struct RNode *node;
    ID id;
    long state;
    struct rb_global_entry *entry;
    struct rb_args_info *args;
    long cnt;
    VALUE value;
  } u3;
} NODE;

struct rb_global_entry {
  struct rb_global_variable *var;
  ID id;
};

struct rb_args_info {
  NODE *pre_init;
  NODE *post_init;

  int pre_args_num; /* count of mandatory pre-arguments */
  int post_args_num; /* count of mandatory post-arguments */

  ID first_post_arg;

  ID rest_arg;
  ID block_arg;

  NODE *kw_args;
  NODE *kw_rest_arg;

  NODE *opt_args;
};

struct parser_params;

// TODO: id.h???

typedef enum {
  NOEX_PUBLIC    = 0x00,
  NOEX_NOSUPER   = 0x01,
  NOEX_PRIVATE   = 0x02,
  NOEX_PROTECTED = 0x02,
  NOEX_MASK      = 0x06,
  NOEX_BASIC     = 0x08,
  NOEX_UNDEF     = NOEX_NOSUPER,
  NOEX_MODFUNC   = 0x12,
  NOEX_SUPER     = 0x20,
  NOEX_VCALL     = 0x40,
  NOEX_RESPONDS  = 0x80,

  NOEX_BIT_WIDTH = 8,
  NOEX_SAFE_SHIFT_OFFSET = ((NOEX_BIT_WIDTH+3)/4)*4
} rb_method_flag_t;

typedef enum {
  VM_METHOD_TYPE_ISEQ,
  VM_METHOD_TYPE_CFUNC,
  VM_METHOD_TYPE_ATTRSET,
  VM_METHOD_TYPE_IVAR,
  VM_METHOD_TYPE_BMETHOD,
  VM_METHOD_TYPE_ZSUPER,
  VM_METHOD_TYPE_UNDEF,
  VM_METHOD_TYPE_NOTIMPLEMENTED,
  VM_METHOD_TYPE_OPTIMIZED,
  VM_METHOD_TYPE_MISSING,
  VM_METHOD_TYPE_REFINED,

  END_OF_VM_METHOD_TYPE_PLACEHOLDER = 0
} rb_method_type_t;

typedef struct rb_method_cfunc_struct {
  VALUE (*func)(ANYARGS);
  VALUE (*invoker)(VALUE (*func)(ANYARGS), VALUE recv, int argc, const VALUE *argv);
  int argc;
} rb_method_cfunc_t;

typedef struct rb_method_attr_struct {
  ID id;
  const VALUE location;
} rb_method_attr_t;

typedef struct rb_iseq_struct rb_iseq_t;

typedef struct rb_method_definition_struct {
  rb_method_type_t type;
  int alias_count;
  ID original_id;
  union {
    rb_iseq_t * const iseq;
    rb_method_cfunc_t cfunc;
    rb_method_attr_t attr;
    const VALUE proc;
    enum method_optimized_type {
      OPTIMIZED_METHOD_TYPE_SEND,
      OPTIMIZED_METHOD_TYPE_CALL,

      OPTIMIZED_METHOD_TYPE__MAX
    } optimize_type;
    struct rb_method_entry_struct *orig_me;
  } body;
} rb_method_definition_t;

typedef struct rb_method_entry_struct {
  rb_method_flag_t flag;
  char mark;
  rb_method_definition_t *def;
  ID called_id;
  VALUE klass;
} rb_method_entry_t;

struct unlinked_method_entry_list_entry {
  struct unlinked_method_entry_list_entry *next;
  rb_method_entry_t *me;
};

/* ruby_atomic.h */

typedef int rb_atomic_t;

/* vm_core.h */

// TODO: Ensure that this is the actual type...
typedef unsigned long long rb_serial_t;

struct symbols {
  ID last_id;
  st_table *str_sym;
  VALUE ids;
  VALUE dsymbol_fstr_hash;
};

struct global_method_cache_t {
  unsigned int size;
  unsigned int mask;
  void *entries;
};

typedef struct origarg_struct {
  int argc;
  char **argv;
} origarg_t;

#include <pthread.h>
typedef pthread_t rb_nativethread_id_t;
typedef pthread_mutex_t rb_nativethread_lock_t;

struct list_node {
  struct list_node *next, *prev;
};

struct list_head {
  struct list_node n;
};

typedef struct rb_thread_cond_struct {
  pthread_cond_t cond;
#ifdef HAVE_CLOCKID_T
  clockid_t clockid;
#endif
} rb_nativethread_cond_t;

typedef struct native_thread_data_struct {
  struct list_node ubf_list;
  rb_nativethread_cond_t sleep_cond;
} native_thread_data_t;

typedef struct rb_global_vm_lock_struct {
  /* fast path */
  unsigned long acquired;
  rb_nativethread_lock_t lock;

  /* slow path */
  volatile unsigned long waiting;
  rb_nativethread_cond_t cond;

  /* yield */
  rb_nativethread_cond_t switch_cond;
  rb_nativethread_cond_t switch_wait_cond;
  int need_yield;
  int wait_yield;
} rb_global_vm_lock_t;

#ifndef ENABLE_VM_OBJSPACE
# ifdef _WIN32 // Why
#  define ENABLE_VM_OBJSPACE 0
# else
#  define ENABLE_VM_OBJSPACE 1
# endif
#endif

#include <setjmp.h>
#include <signal.h>

#ifndef NSIG
# define NSIG (_SIGMAX + 1)
#endif

#define RUBY_NSIG NSIG

#if defined(SIGSEGV) && defined(HAVE_SIGALTSTACK) && defined(SA_SIGINFO) && !defined(__NetBSD__)
# define USE_SIGALTSTACK
#endif

typedef unsigned long rb_num_t;

/* iseq data type */

struct iseq_compile_data_ensure_node_stack;

typedef struct rb_compile_option_struct rb_compile_option_t;

struct iseq_inline_cache_entry {
  rb_serial_t ic_serial;
  union {
    size_t index;
    VALUE value;
  } ic_value;
};

union iseq_inline_storage_entry {
  struct {
    struct rb_thread_struct *running_thread;
    VALUE value;
  } once;
  struct iseq_inline_cache_entry cache;
};

/* to avoid warning */
struct rb_thread_struct;
struct rb_control_frame_struct;

typedef struct rb_call_info_kw_arg_struct {
  int keyword_len;
  VALUE keywords[1];
} rb_call_info_kw_arg_t;

/* rb_call_info_t contains calling information including inline cache */
typedef struct rb_call_info_struct {
  /* Fixed at compile time */
  ID mid;

  unsigned int flag;
  int orig_argc;
  rb_iseq_t *blockiseq;
  rb_call_info_kw_arg_t *kw_arg;

  /* inline cache: keys */
  rb_serial_t method_state;
  rb_serial_t class_serial;
  VALUE klass;

  /* inline cache: values */
  const rb_method_entry_t *me;
  VALUE defined_class;

  /* temporary values for method calling */
  struct rb_block_struct *blockptr;
  VALUE recv;
  int argc;
  union {
    int opt_pc; /* used by iseq */
    int index; /* used by ivar */
    int missing_reason; /* used by method_missing */
    int inc_sp; /* used by cfunc */
  } aux;

  VALUE (*call)(struct rb_thread_struct *th, struct rb_control_frame_struct *cfp, struct rb_call_info_struct *ci);
} rb_call_info_t;

typedef struct rb_iseq_location_struct {
  const VALUE path;
  const VALUE absolute_path;
  const VALUE base_label;
  const VALUE label;
  VALUE first_lineno;
} rb_iseq_location_t;

struct rb_iseq_struct {
  /***************/
  /* static data */
  /***************/

  enum iseq_type {
    ISEQ_TYPE_TOP,
    ISEQ_TYPE_METHOD,
    ISEQ_TYPE_BLOCK,
    ISEQ_TYPE_CLASS,
    ISEQ_TYPE_RESCUE,
    ISEQ_TYPE_ENSURE,
    ISEQ_TYPE_EVAL,
    ISEQ_TYPE_MAIN,
    ISEQ_TYPE_DEFINED_GUARD
  } type;
#if defined(WORDS_BIGENDIAN) && (SIZEOF_VALUE > SIZEOF_INT)
  char dummy[SIZEOF_VALUE - SIZEOF_INT];
#endif
  int stack_max;

  rb_iseq_location_t location;

  VALUE *iseq_encoded; /* encoded iseq (insn addr and operands) */
  unsigned int iseq_size;
  unsigned int line_info_size;

  const VALUE mark_ary;  /* Array: includes operands which should be GC marked */
  const VALUE coverage;  /* Coverage array */

  /* insn info, must be freed */
  struct iseq_line_info_entry *line_info_table;

  ID *local_table;            /* must free */
  int local_table_size;

  /* sizeof(vars) + 1 */
  int local_size;

  union iseq_inline_storage_entry *is_entries;
  int is_size;

  int callinfo_size;
  rb_call_info_t *callinfo_entries;

  struct {
    struct {
      unsigned int has_lead   : 1;
      unsigned int has_opt    : 1;
      unsigned int has_rest   : 1;
      unsigned int has_post   : 1;
      unsigned int has_kw     : 1;
      unsigned int has_kwrest : 1;
      unsigned int has_block  : 1;

      unsigned int ambiguous_param0 : 1; /* {|a|} */
    } flags;

    int size;

    int lead_num;
    int opt_num;
    int rest_start;
    int post_start;
    int post_num;
    int block_start;

    VALUE *opt_table; /* (opt_num + 1) entries. */

    struct rb_iseq_param_keyword {
      int num;
      int required_num;
      int bits_start;
      int rest_start;
      ID *table;
      VALUE *default_values;
    } *keywords;
  } param;

  /* catch table */
  struct iseq_catch_table *catch_table;

  /* for child iseq */
  struct rb_iseq_struct *parent_iseq;
  struct rb_iseq_struct *local_iseq;

  /****************/
  /* dynamic data */
  /****************/

  VALUE self;
  const VALUE orig;

  /* klass/module nest information stack (cref) */
  NODE * const cref_stack;
  const VALUE klass;

  /* misc */
  ID defined_method_id;
  rb_num_t flip_cnd;

  /* used at compile time */
  struct iseq_compile_data *compile_data;

  /* original iseq, before encoding
   * used for debug/dump (TODO: union with compile_data) */
  VALUE *iseq;
};

enum ruby_special_exceptions {
  ruby_error_reenter,
  ruby_error_nomemory,
  ruby_error_sysstack,
  ruby_error_closed_stream,
  ruby_special_error_count
};

enum ruby_basic_operators {
  BOP_PLUS,
  BOP_MINUS,
  BOP_MULT,
  BOP_DIV,
  BOP_MOD,
  BOP_EQ,
  BOP_EQQ,
  BOP_LT,
  BOP_LE,
  BOP_LTLT,
  BOP_AREF,
  BOP_ASET,
  BOP_LENGTH,
  BOP_SIZE,
  BOP_EMPTY_P,
  BOP_SUCC,
  BOP_GT,
  BOP_GE,
  BOP_NOT,
  BOP_NEQ,
  BOP_MATCH,
  BOP_FREEZE,

  BOP_LAST_
};

struct rb_vm_struct;
typedef void rb_vm_at_exit_struct(struct rb_vm_struct *);
typedef struct rb_at_exit_list {
  rb_vm_at_exit_struct *func;
  struct rb_at_exit_list *next;
} rb_at_exit_list;

#if defined(ENABLE_VM_OBJSPACE) && ENABLE_VM_OBJSPACE
struct rb_objspace;
#endif

typedef struct rb_hook_list_struct {
  struct rb_event_hook_struct *hooks;
  rb_event_flag_t events;
  int need_clean;
} rb_hook_list_t;

typedef struct rb_vm_struct {
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

  /* object management */
  VALUE mark_object_ary;

  const VALUE special_exceptions[ruby_special_error_count];

  /* load */
  VALUE top_self;
  VALUE load_path;
  VALUE load_path_snapshot;
  VALUE load_path_check_cache;
  VALUE expanded_load_path;
  VALUE loaded_features;
  VALUE loaded_features_snapshot;
  struct st_table *loaded_features_index;
  struct st_table *loading_table;

  /* signal */
  struct {
    VALUE cmd;
    int safe;
  } trap_list[RUBY_NSIG];

  /* hook */
  rb_hook_list_t event_hooks;

  /* relation table of ensure - rollback for callcc */
  struct st_table *ensure_rollback_table;

  /* postponed_job */
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

  /* params */
  struct { /* size in byte */
    size_t thread_vm_stack_size;
    size_t thread_machine_stack_size;
    size_t fiber_vm_stack_size;
    size_t fiber_machine_stack_size;
  } default_params;

  short redefined_flag[BOP_LAST_];
} rb_vm_t;

typedef struct rb_control_frame_struct {
  VALUE *pc;
  VALUE *sp;
  rb_iseq_t *iseq;
  VALUE flag;
  VALUE self;
  VALUE klass;
  VALUE *ep;
  rb_iseq_t *block_iseq;
  VALUE proc;
  const rb_method_entry_t *me;

#if VM_DEBUG_BP_CHECK
  VALUE *bp_check;
#endif
} rb_control_frame_t;

typedef struct rb_block_struct {
  VALUE self;
  VALUE klass;
  VALUE *ep;
  rb_iseq_t *iseq;
  VALUE proc;
} rb_block_t;

enum rb_thread_status {
  THREAD_RUNNABLE,
  THREAD_STOPPED,
  THREAD_STOPPED_FOREVER,
  THREAD_KILLED
};

typedef RUBY_JMP_BUF rb_jmpbuf_t;

struct rb_vm_tag {
  VALUE tag;
  VALUE retval;
  rb_jmpbuf_t buf;
  struct rb_vm_tag *prev;
};

struct rb_vm_protect_tag {
  struct rb_vm_protect_tag *prev;
};

struct rb_unblock_callback {
  rb_unblock_function_t *func;
  void *arg;
};

struct rb_mutex_struct;

struct rb_thread_struct;
typedef struct rb_thread_list_struct {
  struct rb_thread_list_struct *next;
  struct rb_thread_struct *th;
} rb_thread_list_t;

typedef struct rb_ensure_entry {
  VALUE marker;
  VALUE (*e_proc)(ANYARGS);
  VALUE data2;
} rb_ensure_entry_t;

typedef struct rb_ensure_list {
  struct rb_ensure_list *next;
  struct rb_ensure_entry entry;
} rb_ensure_list_t;

typedef char rb_thread_id_string_t[sizeof(rb_nativethread_id_t) * 2 + 3];

typedef struct rb_fiber_struct rb_fiber_t;

typedef struct rb_thread_struct {
  struct list_node vmlt_node;
  VALUE self;
  rb_vm_t *vm;

  /* execution information */
  VALUE *stack;
  size_t stack_size;
  rb_control_frame_t *cfp;
  int safe_level;
  int raised_flag;
  VALUE last_status; /* $? */

  /* passing state */
  int state;

  int waiting_fd;

  /* for rb_iterate */
  const rb_block_t *passed_block;

  /* for bmethod */
  const rb_method_entry_t *passed_bmethod_me;

  /* for cfunc */
  rb_call_info_t *passed_ci;

  /* for load(true) */
  VALUE top_self;
  VALUE top_wrapper;

  /* eval env */
  rb_block_t *base_block;

  VALUE *root_lep;
  VALUE root_svar;

  /* thread control */
  rb_nativethread_id_t thread_id;
#ifdef NON_SCALAR_THREAD_ID
  rb_thread_id_string_t thread_id_string;
#endif
  enum rb_thread_status status;
  int to_kill;
  int priority;

  native_thread_data_t native_thread_data;
  void *blocking_region_buffer;

  VALUE thgroup;
  VALUE value;

  /* temporary place of errinfo */
  VALUE errinfo;

  /* temporary place of retval on OPT_CALL_THREADED_CODE */
#if OPT_CALL_THREADED_CODE
  VALUE retval;
#endif

  /* async errinfo queue */
  VALUE pending_interrupt_queue;
  VALUE pending_interrupt_mask_stack;
  int pending_interrupt_queue_checked;

  rb_atomic_t interrupt_flag;
  unsigned long interrupt_mask;
  rb_nativethread_lock_t interrupt_lock;
  rb_nativethread_cond_t interrupt_cond;
  struct rb_unblock_callback unblock;
  VALUE locking_mutex;
  struct rb_mutex_struct *keeping_mutexes;

  struct rb_vm_tag *tag;
  struct rb_vm_protect_tag *protect_tag;

  /*! Thread-local state of evaluation context.
   *
   * If non-zero, the parser does not automatically print error messages to
   * stderr. */
  int mild_compile_error;

  /* storage */
  st_table *local_storage;
  VALUE local_storage_recursive_hash;
  VALUE local_storage_recursive_hash_for_trace;

  rb_thread_list_t *join_list;

  VALUE first_proc;
  VALUE first_args;
  VALUE (*first_func)(ANYARGS);

  /* for GC */
  struct {
    VALUE *stack_start;
    VALUE *stack_end;
    size_t stack_maxsize;
#ifdef __ia64
    VALUE *register_stack_start;
    VALUE *register_stack_end;
    size_t register_stack_maxsize;
#endif
    jmp_buf regs;
  } machine;

  /* statistics data for profiler */
  VALUE stat_insn_usage;

  /* tracer */
  rb_hook_list_t event_hooks;
  struct rb_trace_arg_struct *trace_arg; /* trace information */

  /* fiber */
  rb_fiber_t *fiber;
  rb_fiber_t *root_fiber;
  rb_jmpbuf_t root_jmpbuf;

  /* ensure & callcc */
  rb_ensure_list_t *ensure_list;

  /* misc */
  int method_missing_reason;
  int abort_on_exception;
#ifdef USE_SIGALTSTACK
  void *altstack;
#endif
  unsigned long running_time_us;
} rb_thread_t;

typedef enum {
  VM_DEFINECLASS_TYPE_CLASS             = 0x00,
  VM_DEFINECLASS_TYPE_SINGLETON_CLASS   = 0x01,
  VM_DEFINECLASS_TYPE_MODULE            = 0x02,

  VM_DEFINECLASS_TYPE_MASK              = 0x07
} rb_vm_defineclass_type_t;

typedef struct {
  rb_block_t block;

  VALUE envval;
  VALUE blockprocval;
  int8_t safe_level;
  int8_t is_from_method;
  int8_t is_lambda;
} rb_proc_t;

typedef struct {
  int env_size;
  int local_size;
  VALUE prev_envval;
  rb_block_t block;
  VALUE env[1];
} rb_env_t;

typedef struct {
  VALUE env;
  VALUE path;
  VALUE blockprocval;
  unsigned short first_lineno;
} rb_binding_t;

enum vm_check_match_type {
  VM_CHECKMATCH_TYPE_WHEN = 1,
  VM_CHECKMATCH_TYPE_CASE = 2,
  VM_CHECKMATCH_TYPE_RESCUE = 3
};

enum vm_special_object_type {
  VM_SPECIAL_OBJECT_VMCORE = 1,
  VM_SPECIAL_OBJECT_CBASE,
  VM_SPECIAL_OBJECT_CONST_BASE
};

typedef struct iseq_inline_cache_entry *IC;
typedef rb_call_info_t *CALL_INFO;

typedef VALUE CDHASH;

typedef rb_control_frame_t *
  (*rb_insn_func_t)(rb_thread_t *, rb_control_frame_t *);

typedef int rb_backtrace_iter_func(void *, VALUE, int, VALUE);

enum {
  TIMER_INTERRUPT_MASK = 0x01,
  PENDING_INTERRUPT_MASK = 0x02,
  POSTPONED_JOB_INTERRUPT_MASK = 0x04,
  TRAP_INTERRUPT_MASK = 0x08
};

struct rb_trace_arg_struct {
  rb_event_flag_t evet;
  rb_thread_t *th;
  rb_control_frame_t *cfp;
  VALUE self;
  ID id;
  VALUE klass;
  VALUE data;

  int klass_solved;

  /* calc from cfp */
  int lineno;
  VALUE path;
};

extern origarg_t *origarg;
extern void (*Init_prelude)(void);
extern void (*Init_enc)(void);
extern void (*Init_ext)(void);
extern rb_vm_t volatile **ruby_current_vm_p;
extern rb_thread_t volatile **ruby_current_thread_p;
extern int volatile *ruby_setup_initialized_p;
extern st_table volatile **frozen_strings_p;
extern rb_serial_t volatile *ruby_vm_global_method_state_p;
extern rb_serial_t volatile *ruby_vm_global_constant_state_p;
extern rb_serial_t volatile *ruby_vm_class_serial_p;
extern struct symbols *global_symbols_p;
extern struct global_method_cache_t *global_method_cache_p;

#endif // HEADER__RUBY_SHIMS_H__INCLUDED
