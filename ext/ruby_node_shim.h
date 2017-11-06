#ifndef HEADER__RUBY_NODE_SHIM_H__INCLUDED
#define HEADER__RUBY_NODE_SHIM_H__INCLUDED

enum node_type {
  NODE_SCOPE,
#define NODE_SCOPE       NODE_SCOPE
  NODE_BLOCK,
#define NODE_BLOCK       NODE_BLOCK
  NODE_IF,
#define NODE_IF          NODE_IF
  NODE_UNLESS,
#define NODE_UNLESS      NODE_UNLESS
  NODE_CASE,
#define NODE_CASE        NODE_CASE
  NODE_CASE2,
#define NODE_CASE2       NODE_CASE2
  NODE_WHEN,
#define NODE_WHEN        NODE_WHEN
  NODE_WHILE,
#define NODE_WHILE       NODE_WHILE
  NODE_UNTIL,
#define NODE_UNTIL       NODE_UNTIL
  NODE_ITER,
#define NODE_ITER        NODE_ITER
  NODE_FOR,
#define NODE_FOR         NODE_FOR
  NODE_BREAK,
#define NODE_BREAK       NODE_BREAK
  NODE_NEXT,
#define NODE_NEXT        NODE_NEXT
  NODE_REDO,
#define NODE_REDO        NODE_REDO
  NODE_RETRY,
#define NODE_RETRY       NODE_RETRY
  NODE_BEGIN,
#define NODE_BEGIN       NODE_BEGIN
  NODE_RESCUE,
#define NODE_RESCUE      NODE_RESCUE
  NODE_RESBODY,
#define NODE_RESBODY     NODE_RESBODY
  NODE_ENSURE,
#define NODE_ENSURE      NODE_ENSURE
  NODE_AND,
#define NODE_AND         NODE_AND
  NODE_OR,
#define NODE_OR          NODE_OR
  NODE_MASGN,
#define NODE_MASGN       NODE_MASGN
  NODE_LASGN,
#define NODE_LASGN       NODE_LASGN
  NODE_DASGN,
#define NODE_DASGN       NODE_DASGN
  NODE_DASGN_CURR,
#define NODE_DASGN_CURR  NODE_DASGN_CURR
  NODE_GASGN,
#define NODE_GASGN       NODE_GASGN
  NODE_IASGN,
#define NODE_IASGN       NODE_IASGN
  NODE_CDECL,
#define NODE_CDECL       NODE_CDECL
  NODE_CVASGN,
#define NODE_CVASGN      NODE_CVASGN
  NODE_OP_ASGN1,
#define NODE_OP_ASGN1    NODE_OP_ASGN1
  NODE_OP_ASGN2,
#define NODE_OP_ASGN2    NODE_OP_ASGN2
  NODE_OP_ASGN_AND,
#define NODE_OP_ASGN_AND NODE_OP_ASGN_AND
  NODE_OP_ASGN_OR,
#define NODE_OP_ASGN_OR  NODE_OP_ASGN_OR
  NODE_OP_CDECL,
#define NODE_OP_CDECL    NODE_OP_CDECL
  NODE_CALL,
#define NODE_CALL        NODE_CALL
  NODE_OPCALL,
#define NODE_OPCALL      NODE_OPCALL
  NODE_FCALL,
#define NODE_FCALL       NODE_FCALL
  NODE_VCALL,
#define NODE_VCALL       NODE_VCALL
  NODE_QCALL,
#define NODE_QCALL       NODE_QCALL
  NODE_SUPER,
#define NODE_SUPER       NODE_SUPER
  NODE_ZSUPER,
#define NODE_ZSUPER      NODE_ZSUPER
  NODE_ARRAY,
#define NODE_ARRAY       NODE_ARRAY
  NODE_ZARRAY,
#define NODE_ZARRAY      NODE_ZARRAY
  NODE_VALUES,
#define NODE_VALUES      NODE_VALUES
  NODE_HASH,
#define NODE_HASH        NODE_HASH
  NODE_RETURN,
#define NODE_RETURN      NODE_RETURN
  NODE_YIELD,
#define NODE_YIELD       NODE_YIELD
  NODE_LVAR,
#define NODE_LVAR        NODE_LVAR
  NODE_DVAR,
#define NODE_DVAR        NODE_DVAR
  NODE_GVAR,
#define NODE_GVAR        NODE_GVAR
  NODE_IVAR,
#define NODE_IVAR        NODE_IVAR
  NODE_CONST,
#define NODE_CONST       NODE_CONST
  NODE_CVAR,
#define NODE_CVAR        NODE_CVAR
  NODE_NTH_REF,
#define NODE_NTH_REF     NODE_NTH_REF
  NODE_BACK_REF,
#define NODE_BACK_REF    NODE_BACK_REF
  NODE_MATCH,
#define NODE_MATCH       NODE_MATCH
  NODE_MATCH2,
#define NODE_MATCH2      NODE_MATCH2
  NODE_MATCH3,
#define NODE_MATCH3      NODE_MATCH3
  NODE_LIT,
#define NODE_LIT         NODE_LIT
  NODE_STR,
#define NODE_STR         NODE_STR
  NODE_DSTR,
#define NODE_DSTR        NODE_DSTR
  NODE_XSTR,
#define NODE_XSTR        NODE_XSTR
  NODE_DXSTR,
#define NODE_DXSTR       NODE_DXSTR
  NODE_EVSTR,
#define NODE_EVSTR       NODE_EVSTR
  NODE_DREGX,
#define NODE_DREGX       NODE_DREGX
  NODE_ARGS,
#define NODE_ARGS        NODE_ARGS
  NODE_ARGS_AUX,
#define NODE_ARGS_AUX    NODE_ARGS_AUX
  NODE_OPT_ARG,
#define NODE_OPT_ARG     NODE_OPT_ARG
  NODE_KW_ARG,
#define NODE_KW_ARG     NODE_KW_ARG
  NODE_POSTARG,
#define NODE_POSTARG     NODE_POSTARG
  NODE_ARGSCAT,
#define NODE_ARGSCAT     NODE_ARGSCAT
  NODE_ARGSPUSH,
#define NODE_ARGSPUSH    NODE_ARGSPUSH
  NODE_SPLAT,
#define NODE_SPLAT       NODE_SPLAT
  NODE_BLOCK_PASS,
#define NODE_BLOCK_PASS  NODE_BLOCK_PASS
  NODE_DEFN,
#define NODE_DEFN        NODE_DEFN
  NODE_DEFS,
#define NODE_DEFS        NODE_DEFS
  NODE_ALIAS,
#define NODE_ALIAS       NODE_ALIAS
  NODE_VALIAS,
#define NODE_VALIAS      NODE_VALIAS
  NODE_UNDEF,
#define NODE_UNDEF       NODE_UNDEF
  NODE_CLASS,
#define NODE_CLASS       NODE_CLASS
  NODE_MODULE,
#define NODE_MODULE      NODE_MODULE
  NODE_SCLASS,
#define NODE_SCLASS      NODE_SCLASS
  NODE_COLON2,
#define NODE_COLON2      NODE_COLON2
  NODE_COLON3,
#define NODE_COLON3      NODE_COLON3
  NODE_DOT2,
#define NODE_DOT2        NODE_DOT2
  NODE_DOT3,
#define NODE_DOT3        NODE_DOT3
  NODE_FLIP2,
#define NODE_FLIP2       NODE_FLIP2
  NODE_FLIP3,
#define NODE_FLIP3       NODE_FLIP3
  NODE_SELF,
#define NODE_SELF        NODE_SELF
  NODE_NIL,
#define NODE_NIL         NODE_NIL
  NODE_TRUE,
#define NODE_TRUE        NODE_TRUE
  NODE_FALSE,
#define NODE_FALSE       NODE_FALSE
  NODE_ERRINFO,
#define NODE_ERRINFO     NODE_ERRINFO
  NODE_DEFINED,
#define NODE_DEFINED     NODE_DEFINED
  NODE_POSTEXE,
#define NODE_POSTEXE     NODE_POSTEXE
  NODE_DSYM,
#define NODE_DSYM        NODE_DSYM
  NODE_ATTRASGN,
#define NODE_ATTRASGN    NODE_ATTRASGN
  NODE_PRELUDE,
#define NODE_PRELUDE     NODE_PRELUDE
  NODE_LAMBDA,
#define NODE_LAMBDA      NODE_LAMBDA
  NODE_LAST
#define NODE_LAST        NODE_LAST
};

typedef struct RNode {
  VALUE flags;
  VALUE nd_reserved;        /* ex nd_file */
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

NODE *rb_compile_cstr(const char *f, const char *s, int len, int line);

#define RNODE(obj)  (R_CAST(RNode)(obj))

#define NODE_FL_NEWLINE              (((VALUE)1)<<7)

#define NODE_TYPESHIFT 8
#define NODE_TYPEMASK  (((VALUE)0x7f)<<NODE_TYPESHIFT)

#define nd_type(n) ((int) (((RNODE(n))->flags & NODE_TYPEMASK)>>NODE_TYPESHIFT))
#define nd_set_type(n,t) \
      RNODE(n)->flags=((RNODE(n)->flags&~NODE_TYPEMASK)|((((unsigned long)(t))<<NODE_TYPESHIFT)&NODE_TYPEMASK))

#define NODE_LSHIFT (NODE_TYPESHIFT+7)
#define NODE_LMASK  (((SIGNED_VALUE)1<<(sizeof(VALUE)*CHAR_BIT-NODE_LSHIFT))-1)
#define nd_line(n) (int)(((SIGNED_VALUE)RNODE(n)->flags)>>NODE_LSHIFT)
#define nd_set_line(n,l) \
      RNODE(n)->flags=((RNODE(n)->flags&~((VALUE)(-1)<<NODE_LSHIFT))|((VALUE)((l)&NODE_LMASK)<<NODE_LSHIFT))
#define nd_column(n) (int)(RNODE(n)->nd_reserved & 0xffff)
#define nd_set_column(n, v) \
      RNODE(n)->nd_reserved = (RNODE(n)->nd_reserved & ~0xffff) | ((v) > 0xffff ? 0xffff : ((unsigned int)(v)) & 0xffff)

#define nd_lineno(n) (int)((RNODE(n)->nd_reserved >> 16) & 0xffff)
#define nd_set_lineno(n, v) \
      RNODE(n)->nd_reserved = (RNODE(n)->nd_reserved & ~0xffff0000) | (((v) > 0xffff ? 0xffff : ((unsigned int)(v)) & 0xffff) << 16)

#define nd_head  u1.node
#define nd_alen  u2.argc
#define nd_next  u3.node

#define nd_cond  u1.node
#define nd_body  u2.node
#define nd_else  u3.node

#define nd_orig  u3.value

#define nd_resq  u2.node
#define nd_ensr  u3.node

#define nd_1st   u1.node
#define nd_2nd   u2.node

#define nd_stts  u1.node

#define nd_entry u3.entry
#define nd_vid   u1.id
#define nd_cflag u2.id
#define nd_cval  u3.value

#define nd_oid   u1.id
#define nd_cnt   u3.cnt
#define nd_tbl   u1.tbl

#define nd_var   u1.node
#define nd_iter  u3.node

#define nd_value u2.node
#define nd_aid   u3.id

#define nd_lit   u1.value

#define nd_frml  u2.argc
#define nd_rest  u1.id
#define nd_opt   u1.node
#define nd_pid   u1.id
#define nd_plen  u2.argc

#define nd_recv  u1.node
#define nd_mid   u2.id
#define nd_args  u3.node
#define nd_ainfo u3.args

#define nd_noex  u3.id
#define nd_defn  u3.node

#define nd_cfnc  u1.cfunc
#define nd_argc  u2.argc

#define nd_cpath u1.node
#define nd_super u3.node

#define nd_modl  u1.id
#define nd_clss_  u1.value

#define nd_beg   u1.node
#define nd_end   u2.node
#define nd_state u3.state
#define nd_rval  u2.value

#define nd_nth   u2.argc

#define nd_tag   u1.id
#define nd_tval  u2.value

#define nd_visi_  u2.argc

VALUE rb_parser_dump_tree(NODE *node, int comment);

#endif // HEADER__RUBY_NODE_SHIM_H__INCLUDED
