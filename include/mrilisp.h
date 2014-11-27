
#define TPV_IMAGE       TPV_USER1
#define TPV_SOLID	TPV_USER2
#define TPV_XVOPAQUE	TPV_USER3
#define TPV_UI		TPV_USER4

#define IMAGEP(x)       (TYPEOFX(x)==TPV_IMAGE)
#define XVOPAQUEP(x)	(TYPEOFX(x)==TPV_XVOPAQUE)
#define SOLIDP(x)	(TYPEOFX(x)==TPV_SOLID)
#define UI_P(x)		(TYPEOFX(x)==TPV_UI)

#define CIMAGE(x)       ((Bimage*)CDR(x))
#define CXVOPAQUE(x)	((Xv_opaque)CDR(x))
#define CSOLID(x)	((Solid*)CDR(x))
#define CUI(x)		((UserI*)CDR(x))

extern Obj makimage(Bimage&);
extern Obj makxvopaque(Xv_opaque it);
extern Obj maksolid(Solid&);
extern Obj mak_ui(UserI*);


