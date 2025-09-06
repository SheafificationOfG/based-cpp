#ifdef UN_OP
#define PURE_UN_OP(O) UN_OP(O)
#define MOD_UN_OP(O) UN_OP(O##O)
#endif

#ifdef PURE_UN_OP
PURE_UN_OP(+)
PURE_UN_OP(-)
PURE_UN_OP(*)
PURE_UN_OP(~)
PURE_UN_OP(&)
PURE_UN_OP(!)
#undef PURE_UN_OP
#endif

#ifdef MOD_UN_OP
MOD_UN_OP(+)
MOD_UN_OP(-)
#undef MOD_UN_OP
#endif

#ifdef POST_UN_OP
POST_UN_OP(+)
POST_UN_OP(-)
#undef POST_UN_OP
#endif

#ifdef UN_OP
#undef UN_OP
#endif

#ifdef BIN_OP
#define PURE_BIN_OP(O) BIN_OP(O)
#define ASSIGN_OP() BIN_OP(=)
#define ASSIGN_BIN_OP(O) BIN_OP(O## =)
#endif

#ifdef PURE_BIN_OP
PURE_BIN_OP(+)
PURE_BIN_OP(-)
PURE_BIN_OP(*)
PURE_BIN_OP(/)
PURE_BIN_OP(%)
PURE_BIN_OP(^)
PURE_BIN_OP(&)
PURE_BIN_OP(|)
PURE_BIN_OP(<)
PURE_BIN_OP(>)
PURE_BIN_OP(<<)
PURE_BIN_OP(>>)
PURE_BIN_OP(==)
PURE_BIN_OP(!=)
PURE_BIN_OP(<=)
PURE_BIN_OP(>=)
PURE_BIN_OP(->*)
#ifndef NO_SHORTCIRCUIT_OPS
PURE_BIN_OP(&&)
PURE_BIN_OP(||)
#else
#undef NO_SHORTCIRCUIT_OPS
#endif
#undef PURE_BIN_OP
#endif

#ifdef ASSIGN_OP
ASSIGN_OP()
#undef ASSIGN_OP
#endif

#ifdef ASSIGN_BIN_OP
ASSIGN_BIN_OP(+)
ASSIGN_BIN_OP(-)
ASSIGN_BIN_OP(*)
ASSIGN_BIN_OP(/)
ASSIGN_BIN_OP(%)
ASSIGN_BIN_OP(^)
ASSIGN_BIN_OP(&)
ASSIGN_BIN_OP(|)
ASSIGN_BIN_OP(<<)
ASSIGN_BIN_OP(>>)
#undef ASSIGN_BIN_OP
#endif

#ifdef BIN_OP
#undef BIN_OP
#endif

#ifdef POLY_OP
POLY_OP(())
POLY_OP([])
#undef POLY_OP
#endif
