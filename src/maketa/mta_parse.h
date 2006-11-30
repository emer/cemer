typedef struct {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
} YYSTYPE;
#define	CLASS	257
#define	STRUCT	258
#define	UNION	259
#define	ENUM	260
#define	FUNTYPE	261
#define	STATIC	262
#define	TEMPLATE	263
#define	CONST	264
#define	TYPEDEF	265
#define	TA_TYPEDEF	266
#define	TYPE	267
#define	NUMBER	268
#define	FUNCTION	269
#define	ARRAY	270
#define	NAME	271
#define	COMMENT	272
#define	FUNCALL	273
#define	SCOPER	274
#define	EQUALS	275
#define	PUBLIC	276
#define	PRIVATE	277
#define	PROTECTED	278
#define	OPERATOR	279
#define	FRIEND	280
#define	THISNAME	281
#define	REGFUN	282
#define	VIRTUAL	283


extern YYSTYPE yylval;
