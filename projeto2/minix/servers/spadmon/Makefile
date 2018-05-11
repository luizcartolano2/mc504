.include <bsd.own.mk>

# Makefile for Process Manager (PM)
PROG=	spadmon
SRCS=	main.c table.c forward.c get_table.c

DPADD+=	${LIBSYS} ${LIBTIMERS}
LDADD+=	-lsys -ltimers

CPPFLAGS.main.c+=	-I${NETBSDSRCDIR}/minix
CPPFLAGS.misc.c+=	-I${NETBSDSRCDIR}/minix
CPPFLAGS.schedule.c+=	-I${NETBSDSRCDIR}/minix
CPPFLAGS.utility.c+=	-I${NETBSDSRCDIR}/minix

.include <minix.service.mk>
