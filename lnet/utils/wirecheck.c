/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 */
#include <stdio.h>
#include <portals/api-support.h>
#include <portals/list.h>
#include <portals/lib-types.h>

#define BLANK_LINE()				\
do {						\
	printf ("\n");				\
} while (0)

#define COMMENT(c)				\
do {						\
	printf ("        /* "c" */\n");		\
} while (0)

#define STRINGIFY(a) #a

#define CHECK_DEFINE(a)						\
do {								\
	printf ("        LASSERT ("#a" == "STRINGIFY(a)");\n");	\
} while (0)

#define CHECK_VALUE(a)					\
do {							\
	printf ("        LASSERT ("#a" == %d);\n", a);	\
} while (0)

#define CHECK_MEMBER_OFFSET(s,m)		\
do {						\
	CHECK_VALUE(offsetof(s, m));	        \
} while (0)

#define CHECK_MEMBER_SIZEOF(s,m)		\
do {						\
	CHECK_VALUE((int)sizeof(((s *)0)->m));	\
} while (0)

#define CHECK_MEMBER(s,m)			\
do {						\
	CHECK_MEMBER_OFFSET(s, m);		\
	CHECK_MEMBER_SIZEOF(s, m);		\
} while (0)

#define CHECK_STRUCT(s)                         \
do {                                            \
        BLANK_LINE ();                          \
        COMMENT ("Checks for struct "#s);       \
	CHECK_VALUE((int)sizeof(s));            \
} while (0)

void
check_ptl_handle_wire (void)
{
	CHECK_STRUCT (ptl_handle_wire_t);
	CHECK_MEMBER (ptl_handle_wire_t, wh_interface_cookie);
	CHECK_MEMBER (ptl_handle_wire_t, wh_object_cookie);
}

void
check_ptl_magicversion (void)
{
	CHECK_STRUCT (ptl_magicversion_t);
	CHECK_MEMBER (ptl_magicversion_t, magic);
	CHECK_MEMBER (ptl_magicversion_t, version_major);
	CHECK_MEMBER (ptl_magicversion_t, version_minor);
}

void
check_ptl_hdr (void)
{
	CHECK_STRUCT (ptl_hdr_t);
	CHECK_MEMBER (ptl_hdr_t, dest_nid);
	CHECK_MEMBER (ptl_hdr_t, src_nid);
	CHECK_MEMBER (ptl_hdr_t, dest_pid);
	CHECK_MEMBER (ptl_hdr_t, src_pid);
	CHECK_MEMBER (ptl_hdr_t, type);

        BLANK_LINE ();
        COMMENT ("Ack");
        CHECK_MEMBER (ptl_hdr_t, msg.ack.mlength);
        CHECK_MEMBER (ptl_hdr_t, msg.ack.dst_wmd);
        CHECK_MEMBER (ptl_hdr_t, msg.ack.match_bits);
        CHECK_MEMBER (ptl_hdr_t, msg.ack.length);

        BLANK_LINE ();
        COMMENT ("Put");
	CHECK_MEMBER (ptl_hdr_t, msg.put.ptl_index);
	CHECK_MEMBER (ptl_hdr_t, msg.put.ack_wmd);
	CHECK_MEMBER (ptl_hdr_t, msg.put.match_bits);
	CHECK_MEMBER (ptl_hdr_t, msg.put.length);
	CHECK_MEMBER (ptl_hdr_t, msg.put.offset);
	CHECK_MEMBER (ptl_hdr_t, msg.put.hdr_data);

        BLANK_LINE ();
        COMMENT ("Get");
	CHECK_MEMBER (ptl_hdr_t, msg.get.ptl_index);
	CHECK_MEMBER (ptl_hdr_t, msg.get.return_wmd);
	CHECK_MEMBER (ptl_hdr_t, msg.get.match_bits);
	CHECK_MEMBER (ptl_hdr_t, msg.get.length);
	CHECK_MEMBER (ptl_hdr_t, msg.get.src_offset);
	CHECK_MEMBER (ptl_hdr_t, msg.get.return_offset);
	CHECK_MEMBER (ptl_hdr_t, msg.get.sink_length);

        BLANK_LINE ();
        COMMENT ("Reply");
	CHECK_MEMBER (ptl_hdr_t, msg.reply.dst_wmd);
	CHECK_MEMBER (ptl_hdr_t, msg.reply.dst_offset);
	CHECK_MEMBER (ptl_hdr_t, msg.reply.length);
}

int
main (int argc, char **argv)
{
	printf ("void lib_assert_wire_constants (void)\n"
		"{\n");

	COMMENT ("Wire protocol assertions generated by 'wirecheck'");
	BLANK_LINE ();
	
	COMMENT ("Constants...");
	CHECK_DEFINE (PORTALS_PROTO_MAGIC);
	CHECK_DEFINE (PORTALS_PROTO_VERSION_MAJOR);
	CHECK_DEFINE (PORTALS_PROTO_VERSION_MINOR);

	CHECK_VALUE (PTL_MSG_ACK);
	CHECK_VALUE (PTL_MSG_PUT);
	CHECK_VALUE (PTL_MSG_GET);
	CHECK_VALUE (PTL_MSG_REPLY);
	CHECK_VALUE (PTL_MSG_HELLO);

	check_ptl_handle_wire ();
	check_ptl_magicversion ();
	check_ptl_hdr ();
	
	printf ("}\n\n");
	
	return (0);
}
