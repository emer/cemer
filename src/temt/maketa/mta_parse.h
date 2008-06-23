typedef struct {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  const char*   chr;
  int	   	rval;
} YYSTYPE;
#define	CLASS	257
#define	TYPENAME	258
#define	STRUCT	259
#define	UNION	260
#define	ENUM	261
#define	FUNTYPE	262
#define	STATIC	263
#define	TEMPLATE	264
#define	CONST	265
#define	TYPEDEF	266
#define	TA_TYPEDEF	267
#define	TYPE	268
#define	NUMBER	269
#define	FUNCTION	270
#define	ARRAY	271
#define	NAME	272
#define	COMMENT	273
#define	FUNCALL	274
#define	SCOPER	275
#define	EQUALS	276
#define	PUBLIC	277
#define	PRIVATE	278
#define	PROTECTED	279
#define	OPERATOR	280
#define	FRIEND	281
#define	THISNAME	282
#define	REGFUN	283
#define	VIRTUAL	284


extern YYSTYPE yylval;
