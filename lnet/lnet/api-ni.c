/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (c) 2001-2003 Cluster File Systems, Inc.
 *
 *   This file is part of Lustre, http://www.sf.net/projects/lustre/
 *
 *   Lustre is free software; you can redistribute it and/or
 *   modify it under the terms of version 2 of the GNU General Public
 *   License as published by the Free Software Foundation.
 *
 *   Lustre is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Lustre; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define DEBUG_SUBSYSTEM S_PORTALS
#include <portals/lib-p30.h>

#define DEFAULT_NETWORKS  "tcp"
static char *networks = DEFAULT_NETWORKS;
CFS_MODULE_PARM(networks, "s", charp, 0444,
                "local networks (default='"DEFAULT_NETWORKS"')");

ptl_apini_t       ptl_apini;                    /* THE network interface (at the API) */

void ptl_assert_wire_constants (void)
{
        /* Wire protocol assertions generated by 'wirecheck'
         * running on Linux robert.bartonsoftware.com 2.6.8-1.521
         * #1 Mon Aug 16 09:01:18 EDT 2004 i686 athlon i386 GNU/Linux
         * with gcc version 3.3.3 20040412 (Red Hat Linux 3.3.3-7) */

        /* Constants... */
        CLASSERT (PTL_PROTO_TCP_MAGIC == 0xeebc0ded);
        CLASSERT (PTL_PROTO_TCP_VERSION_MAJOR == 1);
        CLASSERT (PTL_PROTO_TCP_VERSION_MINOR == 0);
        CLASSERT (PTL_MSG_ACK == 0);
        CLASSERT (PTL_MSG_PUT == 1);
        CLASSERT (PTL_MSG_GET == 2);
        CLASSERT (PTL_MSG_REPLY == 3);
        CLASSERT (PTL_MSG_HELLO == 4);

        /* Checks for struct ptl_handle_wire_t */
        CLASSERT ((int)sizeof(ptl_handle_wire_t) == 16);
        CLASSERT ((int)offsetof(ptl_handle_wire_t, wh_interface_cookie) == 0);
        CLASSERT ((int)sizeof(((ptl_handle_wire_t *)0)->wh_interface_cookie) == 8);
        CLASSERT ((int)offsetof(ptl_handle_wire_t, wh_object_cookie) == 8);
        CLASSERT ((int)sizeof(((ptl_handle_wire_t *)0)->wh_object_cookie) == 8);

        /* Checks for struct ptl_magicversion_t */
        CLASSERT ((int)sizeof(ptl_magicversion_t) == 8);
        CLASSERT ((int)offsetof(ptl_magicversion_t, magic) == 0);
        CLASSERT ((int)sizeof(((ptl_magicversion_t *)0)->magic) == 4);
        CLASSERT ((int)offsetof(ptl_magicversion_t, version_major) == 4);
        CLASSERT ((int)sizeof(((ptl_magicversion_t *)0)->version_major) == 2);
        CLASSERT ((int)offsetof(ptl_magicversion_t, version_minor) == 6);
        CLASSERT ((int)sizeof(((ptl_magicversion_t *)0)->version_minor) == 2);

        /* Checks for struct ptl_hdr_t */
        CLASSERT ((int)sizeof(ptl_hdr_t) == 72);
        CLASSERT ((int)offsetof(ptl_hdr_t, dest_nid) == 0);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->dest_nid) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, src_nid) == 8);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->src_nid) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, dest_pid) == 16);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->dest_pid) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, src_pid) == 20);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->src_pid) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, type) == 24);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->type) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, payload_length) == 28);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->payload_length) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg) == 40);

        /* Ack */
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.ack.dst_wmd) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.ack.dst_wmd) == 16);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.ack.match_bits) == 48);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.ack.match_bits) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.ack.mlength) == 56);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.ack.mlength) == 4);

        /* Put */
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.put.ack_wmd) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.put.ack_wmd) == 16);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.put.match_bits) == 48);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.put.match_bits) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.put.hdr_data) == 56);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.put.hdr_data) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.put.ptl_index) == 64);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.put.ptl_index) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.put.offset) == 68);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.put.offset) == 4);

        /* Get */
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.get.return_wmd) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.get.return_wmd) == 16);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.get.match_bits) == 48);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.get.match_bits) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.get.ptl_index) == 56);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.get.ptl_index) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.get.src_offset) == 60);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.get.src_offset) == 4);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.get.sink_length) == 64);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.get.sink_length) == 4);

        /* Reply */
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.reply.dst_wmd) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.reply.dst_wmd) == 16);

        /* Hello */
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.hello.incarnation) == 32);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.hello.incarnation) == 8);
        CLASSERT ((int)offsetof(ptl_hdr_t, msg.hello.type) == 40);
        CLASSERT ((int)sizeof(((ptl_hdr_t *)0)->msg.hello.type) == 4);
}

ptl_nal_t *
ptl_find_nal_by_type (int type) 
{
        ptl_nal_t          *nal;
        struct list_head   *tmp;

        /* holding nal mutex */
        list_for_each (tmp, &ptl_apini.apini_nals) {
                nal = list_entry(tmp, ptl_nal_t, nal_list);

                if (nal->nal_type == type)
                        return nal;
        }
        
        return NULL;
}

void
ptl_register_nal (ptl_nal_t *nal)
{
        PTL_MUTEX_DOWN(&ptl_apini.apini_nal_mutex);

        LASSERT (ptl_apini.apini_init);
        LASSERT (libcfs_isknown_nal(nal->nal_type));
        LASSERT (ptl_find_nal_by_type(nal->nal_type) == NULL);
        
        list_add (&nal->nal_list, &ptl_apini.apini_nals);
        nal->nal_refcount = 0;

        if (nal->nal_type != LONAL)
                LCONSOLE(0, "%s NAL registered\n",
                         libcfs_nal2str(nal->nal_type));

        PTL_MUTEX_UP(&ptl_apini.apini_nal_mutex);
}

void
ptl_unregister_nal (ptl_nal_t *nal)
{
        PTL_MUTEX_DOWN(&ptl_apini.apini_nal_mutex);

        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_find_nal_by_type(nal->nal_type) == nal);
        LASSERT (nal->nal_refcount == 0);
        
        list_del (&nal->nal_list);
        if (nal->nal_type != LONAL)
                LCONSOLE(0, "%s NAL unregistered\n",
                         libcfs_nal2str(nal->nal_type));

        PTL_MUTEX_UP(&ptl_apini.apini_nal_mutex);
}

#ifndef PTL_USE_LIB_FREELIST
int
ptl_descriptor_setup (ptl_ni_limits_t *requested_limits,
                      ptl_ni_limits_t *actual_limits)
{
        /* Ignore requested limits! */
        actual_limits->max_mes = INT_MAX;
        actual_limits->max_mds = INT_MAX;
        actual_limits->max_eqs = INT_MAX;

        return PTL_OK;
}

void
ptl_descriptor_cleanup (void)
{
}

#else

int
ptl_freelist_init (ptl_freelist_t *fl, int n, int size)
{
        char *space;

        LASSERT (n > 0);

        size += offsetof (ptl_freeobj_t, fo_contents);

        PORTAL_ALLOC(space, n * size);
        if (space == NULL)
                return (PTL_NO_SPACE);

        CFS_INIT_LIST_HEAD (&fl->fl_list);
        fl->fl_objs = space;
        fl->fl_nobjs = n;
        fl->fl_objsize = size;

        do
        {
                memset (space, 0, size);
                list_add ((struct list_head *)space, &fl->fl_list);
                space += size;
        } while (--n != 0);

        return (PTL_OK);
}

void
ptl_freelist_fini (ptl_freelist_t *fl)
{
        struct list_head *el;
        int               count;

        if (fl->fl_nobjs == 0)
                return;

        count = 0;
        for (el = fl->fl_list.next; el != &fl->fl_list; el = el->next)
                count++;

        LASSERT (count == fl->fl_nobjs);

        PORTAL_FREE(fl->fl_objs, fl->fl_nobjs * fl->fl_objsize);
        memset (fl, 0, sizeof (fl));
}

int
ptl_descriptor_setup (ptl_ni_limits_t *requested_limits,
                      ptl_ni_limits_t *actual_limits)
{
        /* NB on failure caller must still call ptl_descriptor_cleanup */
        /*               ******                                        */
        int        rc;

        memset (&ptl_apini.apini_free_mes,  0, sizeof (ptl_apini.apini_free_mes));
        memset (&ptl_apini.apini_free_msgs, 0, sizeof (ptl_apini.apini_free_msgs));
        memset (&ptl_apini.apini_free_mds,  0, sizeof (ptl_apini.apini_free_mds));
        memset (&ptl_apini.apini_free_eqs,  0, sizeof (ptl_apini.apini_free_eqs));

        /* Ignore requested limits! */
        actual_limits->max_mes = MAX_MES;
        actual_limits->max_mds = MAX_MDS;
        actual_limits->max_eqs = MAX_EQS;
        /* Hahahah what a load of bollocks.  There's nowhere to
         * specify the max # messages in-flight */

        rc = ptl_freelist_init (&ptl_apini.apini_free_mes,
                                MAX_MES, sizeof (ptl_me_t));
        if (rc != PTL_OK)
                return (rc);

        rc = ptl_freelist_init (&ptl_apini.apini_free_msgs,
                                MAX_MSGS, sizeof (ptl_msg_t));
        if (rc != PTL_OK)
                return (rc);

        rc = ptl_freelist_init (&ptl_apini.apini_free_mds,
                                MAX_MDS, sizeof (ptl_libmd_t));
        if (rc != PTL_OK)
                return (rc);

        rc = ptl_freelist_init (&ptl_apini.apini_free_eqs,
                                MAX_EQS, sizeof (ptl_eq_t));
        return (rc);
}

void
ptl_descriptor_cleanup (void)
{
        ptl_freelist_fini (&ptl_apini.apini_free_mes);
        ptl_freelist_fini (&ptl_apini.apini_free_msgs);
        ptl_freelist_fini (&ptl_apini.apini_free_mds);
        ptl_freelist_fini (&ptl_apini.apini_free_eqs);
}

#endif

__u64
ptl_create_interface_cookie (void)
{
        /* NB the interface cookie in wire handles guards against delayed
         * replies and ACKs appearing valid after reboot. Initialisation time,
         * even if it's only implemented to millisecond resolution is probably
         * easily good enough. */
        struct timeval tv;
        __u64          cookie;
#ifndef __KERNEL__
        int            rc = gettimeofday (&tv, NULL);
        LASSERT (rc == 0);
#else
	do_gettimeofday(&tv);
#endif
        cookie = tv.tv_sec;
        cookie *= 1000000;
        cookie += tv.tv_usec;
        return cookie;
}

int
ptl_setup_handle_hash (void) 
{
        int       i;
        
        /* Arbitrary choice of hash table size */
#ifdef __KERNEL__
        ptl_apini.apini_lh_hash_size = PAGE_SIZE / sizeof (struct list_head);
#else
        ptl_apini.apini_lh_hash_size = (MAX_MES + MAX_MDS + MAX_EQS)/4;
#endif
        PORTAL_ALLOC(ptl_apini.apini_lh_hash_table,
                     ptl_apini.apini_lh_hash_size * sizeof (struct list_head));
        if (ptl_apini.apini_lh_hash_table == NULL)
                return (PTL_NO_SPACE);
        
        for (i = 0; i < ptl_apini.apini_lh_hash_size; i++)
                CFS_INIT_LIST_HEAD (&ptl_apini.apini_lh_hash_table[i]);

        ptl_apini.apini_next_object_cookie = PTL_COOKIE_TYPES;
        
        return (PTL_OK);
}

void
ptl_cleanup_handle_hash (void)
{
        if (ptl_apini.apini_lh_hash_table == NULL)
                return;
        
        PORTAL_FREE(ptl_apini.apini_lh_hash_table,
                    ptl_apini.apini_lh_hash_size * sizeof (struct list_head));
}

ptl_libhandle_t *
ptl_lookup_cookie (__u64 cookie, int type) 
{
        /* ALWAYS called with PTL_LOCK held */
        struct list_head    *list;
        struct list_head    *el;
        unsigned int         hash;

        if ((cookie & (PTL_COOKIE_TYPES - 1)) != type)
                return (NULL);
        
        hash = ((unsigned int)cookie) % ptl_apini.apini_lh_hash_size;
        list = &ptl_apini.apini_lh_hash_table[hash];
        
        list_for_each (el, list) {
                ptl_libhandle_t *lh = list_entry (el, ptl_libhandle_t,
                                                  lh_hash_chain);
                
                if (lh->lh_cookie == cookie)
                        return (lh);
        }
        
        return (NULL);
}

void
ptl_initialise_handle (ptl_libhandle_t *lh, int type) 
{
        /* ALWAYS called with PTL_LOCK held */
        unsigned int    hash;

        LASSERT (type >= 0 && type < PTL_COOKIE_TYPES);
        lh->lh_cookie = ptl_apini.apini_next_object_cookie | type;
        ptl_apini.apini_next_object_cookie += PTL_COOKIE_TYPES;
        
        hash = ((unsigned int)lh->lh_cookie) % ptl_apini.apini_lh_hash_size;
        list_add (&lh->lh_hash_chain, &ptl_apini.apini_lh_hash_table[hash]);
}

void
ptl_invalidate_handle (ptl_libhandle_t *lh)
{
        /* ALWAYS called with PTL_LOCK held */
        list_del (&lh->lh_hash_chain);
}

int
ptl_apini_init(ptl_pid_t requested_pid,
               ptl_ni_limits_t *requested_limits,
               ptl_ni_limits_t *actual_limits)
{
        int               rc = PTL_OK;
        int               ptl_size;
        int               i;
        ENTRY;

        LASSERT (ptl_apini.apini_refcount == 0);

        ptl_apini.apini_pid = requested_pid;

        rc = ptl_descriptor_setup (requested_limits, 
                                   &ptl_apini.apini_actual_limits);
        if (rc != PTL_OK)
                goto out;

        memset(&ptl_apini.apini_counters, 0, 
               sizeof(ptl_apini.apini_counters));

        CFS_INIT_LIST_HEAD (&ptl_apini.apini_active_msgs);
        CFS_INIT_LIST_HEAD (&ptl_apini.apini_active_mds);
        CFS_INIT_LIST_HEAD (&ptl_apini.apini_active_eqs);
        CFS_INIT_LIST_HEAD (&ptl_apini.apini_test_peers);
        CFS_INIT_LIST_HEAD (&ptl_apini.apini_nis);
        CFS_INIT_LIST_HEAD (&ptl_apini.apini_zombie_nis);

        ptl_apini.apini_interface_cookie = ptl_create_interface_cookie();

        rc = ptl_setup_handle_hash ();
        if (rc != PTL_OK)
                goto out;
        
        if (requested_limits != NULL)
                ptl_size = requested_limits->max_pt_index + 1;
        else
                ptl_size = 64;

        ptl_apini.apini_nportals = ptl_size;
        PORTAL_ALLOC(ptl_apini.apini_portals, 
                     ptl_size * sizeof(*ptl_apini.apini_portals));
        if (ptl_apini.apini_portals == NULL) {
                rc = PTL_NO_SPACE;
                goto out;
        }

        for (i = 0; i < ptl_size; i++)
                CFS_INIT_LIST_HEAD(&(ptl_apini.apini_portals[i]));

        /* max_{mes,mds,eqs} set in ptl_descriptor_setup */

        /* We don't have an access control table! */
        ptl_apini.apini_actual_limits.max_ac_index = -1;

        ptl_apini.apini_actual_limits.max_pt_index = ptl_size - 1;
        ptl_apini.apini_actual_limits.max_md_iovecs = PTL_MD_MAX_IOV;
        ptl_apini.apini_actual_limits.max_me_list = INT_MAX;

        /* We don't support PtlGetPut! */
        ptl_apini.apini_actual_limits.max_getput_md = 0;

        if (actual_limits != NULL)
                *actual_limits = ptl_apini.apini_actual_limits;
 out:
        if (rc != PTL_OK) {
                ptl_cleanup_handle_hash ();
                ptl_descriptor_cleanup ();
        }

        RETURN (rc);
}

int
ptl_apini_fini (void)
{
        int       idx;
        
        /* NB no PTL_LOCK since this is the last reference.  All NAL instances
         * have shut down already, so it is safe to unlink and free all
         * descriptors, even those that appear committed to a network op (eg MD
         * with non-zero pending count) */

        ptl_fail_nid(PTL_NID_ANY, 0);

        LASSERT (list_empty(&ptl_apini.apini_test_peers));
        LASSERT (ptl_apini.apini_refcount == 0);
        LASSERT (list_empty(&ptl_apini.apini_nis));
        LASSERT (list_empty(&ptl_apini.apini_zombie_nis));
        LASSERT (ptl_apini.apini_nzombie_nis == 0);
               
        for (idx = 0; idx < ptl_apini.apini_nportals; idx++)
                while (!list_empty (&ptl_apini.apini_portals[idx])) {
                        ptl_me_t *me = list_entry (ptl_apini.apini_portals[idx].next,
                                                   ptl_me_t, me_list);

                        CERROR ("Active me %p on exit\n", me);
                        list_del (&me->me_list);
                        ptl_me_free (me);
                }

        while (!list_empty (&ptl_apini.apini_active_mds)) {
                ptl_libmd_t *md = list_entry (ptl_apini.apini_active_mds.next,
                                           ptl_libmd_t, md_list);

                CERROR ("Active md %p on exit\n", md);
                list_del (&md->md_list);
                ptl_md_free (md);
        }

        while (!list_empty (&ptl_apini.apini_active_eqs)) {
                ptl_eq_t *eq = list_entry (ptl_apini.apini_active_eqs.next,
                                           ptl_eq_t, eq_list);

                CERROR ("Active eq %p on exit\n", eq);
                list_del (&eq->eq_list);
                ptl_eq_free (eq);
        }

        while (!list_empty (&ptl_apini.apini_active_msgs)) {
                ptl_msg_t *msg = list_entry (ptl_apini.apini_active_msgs.next,
                                             ptl_msg_t, msg_list);

                CERROR ("Active msg %p on exit\n", msg);
                list_del (&msg->msg_list);
                ptl_msg_free (msg);
        }

        PORTAL_FREE(ptl_apini.apini_portals,  
                    ptl_apini.apini_nportals * sizeof(*ptl_apini.apini_portals));

        ptl_cleanup_handle_hash ();
        ptl_descriptor_cleanup ();

#ifndef __KERNEL__
        pthread_mutex_destroy(&ptl_apini.apini_mutex);
        pthread_cond_destroy(&ptl_apini.apini_cond);
#endif

        return (PTL_OK);
}

ptl_ni_t  *
ptl_net2ni (__u32 net)
{
        struct list_head *tmp;
        ptl_ni_t         *ni;
        unsigned long     flags;

        PTL_LOCK(flags);
        list_for_each (tmp, &ptl_apini.apini_nis) {
                ni = list_entry(tmp, ptl_ni_t, ni_list);

                if (PTL_NIDNET(ni->ni_nid) == net) {
                        ptl_ni_addref_locked(ni);
                        PTL_UNLOCK(flags);
                        return ni;
                }
        }
        
        PTL_UNLOCK(flags);
        return NULL;
}

int
ptl_count_acceptor_nis (ptl_ni_t **first_ni)
{
        /* Return the # of NIs that need the acceptor.  Return the first one in
         * *first_ni so the acceptor can pass it connections "blind" to retain
         * binary compatibility. */
        int                count = 0;
#ifdef __KERNEL__
        unsigned long      flags;
        struct list_head  *tmp;
        ptl_ni_t          *ni;

        PTL_LOCK(flags);
        list_for_each (tmp, &ptl_apini.apini_nis) {
                ni = list_entry(tmp, ptl_ni_t, ni_list);

                if (ni->ni_nal->nal_accept != NULL) {
                        /* This NAL uses the acceptor */
                        if (count == 0 && first_ni != NULL) {
                                ptl_ni_addref_locked(ni);
                                *first_ni = ni;
                        }
                        count++;
                }
        }
        
        PTL_UNLOCK(flags);
#endif
        return count;
}

int
ptl_islocalnid (ptl_nid_t nid)
{
        struct list_head *tmp;
        ptl_ni_t         *ni;
        unsigned long     flags;
        int               islocal = 0;

        PTL_LOCK(flags);

        list_for_each (tmp, &ptl_apini.apini_nis) {
                ni = list_entry(tmp, ptl_ni_t, ni_list);

                if (ni->ni_nid == nid) {
                        islocal = 1;
                        break;
                }
        }
        
        PTL_UNLOCK(flags);
        return islocal;
}

void
ptl_shutdown_nalnis (void)
{
        int                i;
        int                islo;
        ptl_ni_t          *ni;
        unsigned long      flags;

        /* NB called holding the global mutex */

        /* All quiet on the API front */
        LASSERT (ptl_apini.apini_refcount == 0);
        LASSERT (list_empty(&ptl_apini.apini_zombie_nis));
        LASSERT (ptl_apini.apini_nzombie_nis == 0);

        /* First unlink the NIs from the global list and drop its ref.  When
         * the last ref goes, the NI is queued on apini_zombie_nis....*/

        PTL_LOCK(flags);
        while (!list_empty(&ptl_apini.apini_nis)) {
                ni = list_entry(ptl_apini.apini_nis.next, 
                                ptl_ni_t, ni_list);
                list_del (&ni->ni_list);

                ni->ni_shutdown = 1;
                ptl_apini.apini_nzombie_nis++;

                ptl_ni_decref_locked(ni); /* drop apini's ref (shutdown on last ref) */
        }

        /* Drop the cached loopback NI. */
        if (ptl_loni != NULL) {
                ptl_ni_decref_locked(ptl_loni);
                ptl_loni = NULL;
        }

        /* Now wait for the NI's I just nuked to show up on apini_zombie_nis
         * and shut them down in guaranteed thread context */
        i = 2;
        while (ptl_apini.apini_nzombie_nis != 0) {

                while (list_empty(&ptl_apini.apini_zombie_nis)) {
                        PTL_UNLOCK(flags);
                        ++i;
                        if ((i & (-i)) == i)
                                CDEBUG(D_WARNING,"Waiting for %d zombie NIs\n",
                                       ptl_apini.apini_nzombie_nis);
                        cfs_pause(cfs_time_seconds(1));
                        PTL_LOCK(flags);
                }

                ni = list_entry(ptl_apini.apini_zombie_nis.next,
                                ptl_ni_t, ni_list);
                list_del(&ni->ni_list);
                ni->ni_nal->nal_refcount--;

                PTL_UNLOCK(flags);

                islo = ni->ni_nal->nal_type == LONAL;

                LASSERT (!in_interrupt());
                (ni->ni_nal->nal_shutdown)(ni);

                /* can't deref nal anymore now; it might have unregistered
                 * itself...  */

                if (!islo)
                        LCONSOLE(0, "Removed NI %s\n", 
                                 libcfs_nid2str(ni->ni_nid));

                PORTAL_FREE(ni, sizeof(*ni));

                PTL_LOCK(flags);
                ptl_apini.apini_nzombie_nis--;
        }
        PTL_UNLOCK(flags);

        if (ptl_apini.apini_network_tokens != NULL) {
                PORTAL_FREE(ptl_apini.apini_network_tokens,
                            ptl_apini.apini_network_tokens_nob);
                ptl_apini.apini_network_tokens = NULL;
        }
}

ptl_err_t
ptl_startup_nalnis (void)
{
        ptl_nal_t         *nal;
        ptl_ni_t          *ni;
        struct list_head   nilist;
        ptl_err_t          rc = PTL_OK;
        unsigned long      flags;
        int                nal_type;
        int                retry;

        INIT_LIST_HEAD(&nilist);
        rc = ptl_parse_networks(&nilist, networks);
        if (rc != PTL_OK) 
                goto failed;

        while (!list_empty(&nilist)) {
                ni = list_entry(nilist.next, ptl_ni_t, ni_list);
                nal_type = PTL_NETNAL(PTL_NIDNET(ni->ni_nid));

                LASSERT (libcfs_isknown_nal(nal_type));

                PTL_MUTEX_DOWN(&ptl_apini.apini_nal_mutex);

                for (retry = 0;; retry = 1) {
                        nal = ptl_find_nal_by_type(nal_type);
                        if (nal != NULL) 
                                break;

                        PTL_MUTEX_UP(&ptl_apini.apini_nal_mutex);
#ifdef __KERNEL__
                        if (retry) {
                                CERROR("Can't load NAL %s, module %s\n",
                                       libcfs_nal2str(nal_type),
                                       libcfs_nal2modname(nal_type));
                                goto failed;
                        }

                        request_module(libcfs_nal2modname(nal_type));
#else
                        CERROR("NAL %s not supported\n",
                               libcfs_nal2str(nal_type));
                        goto failed;
#endif
                        PTL_MUTEX_DOWN(&ptl_apini.apini_nal_mutex);
                }

                ni->ni_refcount = 1;

                PTL_LOCK(flags);
                nal->nal_refcount++;
                PTL_UNLOCK(flags);
                
                ni->ni_nal = nal;

                rc = (nal->nal_startup)(ni);

                PTL_MUTEX_UP(&ptl_apini.apini_nal_mutex);

                if (rc != PTL_OK) {
                        CERROR("Error %d starting up NI %s\n",
                               rc, libcfs_nal2str(nal->nal_type));
                        PTL_LOCK(flags);
                        nal->nal_refcount--;
                        PTL_UNLOCK(flags);
                        goto failed;
                }

                if (nal->nal_type != LONAL)
                        LCONSOLE(0, "Added NI %s\n", 
                                 libcfs_nid2str(ni->ni_nid));

                list_del(&ni->ni_list);
                
                PTL_LOCK(flags);
                list_add_tail(&ni->ni_list, &ptl_apini.apini_nis);
                PTL_UNLOCK(flags);
        }

        ptl_loni = ptl_net2ni(PTL_MKNET(LONAL, 0));
        LASSERT (ptl_loni != NULL);

        return PTL_OK;
        
 failed:
        ptl_shutdown_nalnis();

        while (!list_empty(&nilist)) {
                ni = list_entry(nilist.next, ptl_ni_t, ni_list);
                list_del(&ni->ni_list);
                PORTAL_FREE(ni, sizeof(*ni));
        }
        
        return PTL_FAIL;
}

#ifndef __KERNEL__
extern ptl_nal_t tcpnal_nal;
#endif

ptl_err_t
PtlInit(int *max_interfaces)
{
        LASSERT(!strcmp(ptl_err_str[PTL_MAX_ERRNO], "PTL_MAX_ERRNO"));
        ptl_assert_wire_constants ();

        LASSERT (!ptl_apini.apini_init);
        
        ptl_apini.apini_refcount = 0;
        CFS_INIT_LIST_HEAD(&ptl_apini.apini_nals);

#ifdef __KERNEL__
        spin_lock_init (&ptl_apini.apini_lock);
        cfs_waitq_init (&ptl_apini.apini_waitq);
        init_mutex(&ptl_apini.apini_nal_mutex);
        init_mutex(&ptl_apini.apini_api_mutex);
#else
        pthread_mutex_init(&ptl_apini.apini_mutex, NULL);
        pthread_cond_init(&ptl_apini.apini_cond, NULL);
        pthread_mutex_init(&ptl_apini.apini_nal_mutex, NULL);
        pthread_mutex_init(&ptl_apini.apini_api_mutex, NULL);
#endif

        ptl_apini.apini_init = 1;

        if (max_interfaces != NULL)
                *max_interfaces = 1;

        /* NALs in separate modules register themselves when their module
         * loads, and unregister themselves when their module is unloaded.
         * Otherwise they are plugged in explicitly here... */

        ptl_register_nal (&ptl_lonal);
#ifndef __KERNEL__
        ptl_register_nal (&tcpnal_nal);
#endif

        return PTL_OK;
}

void
PtlFini(void)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount == 0);

        /* See comment where tcpnal_nal registers itself */
#ifndef __KERNEL__
        ptl_unregister_nal(&tcpnal_nal);
#endif
        ptl_unregister_nal(&ptl_lonal);

        LASSERT (list_empty(&ptl_apini.apini_nals));

        ptl_apini.apini_init = 0;
}

ptl_err_t
PtlNIInit(ptl_interface_t interface, ptl_pid_t requested_pid,
          ptl_ni_limits_t *requested_limits, ptl_ni_limits_t *actual_limits,
          ptl_handle_ni_t *handle)
{
        int         rc;

        PTL_MUTEX_DOWN(&ptl_apini.apini_api_mutex);

        LASSERT (ptl_apini.apini_init);
        CDEBUG(D_OTHER, "refs %d\n", ptl_apini.apini_refcount);

        if (ptl_apini.apini_refcount > 0) {
                ptl_apini.apini_refcount++;
                rc = PTL_IFACE_DUP;
                goto out;
        }

        rc = ptl_apini_init(requested_pid, requested_limits, actual_limits);
        if (rc != PTL_OK)
                goto out;

        rc = kpr_initialise();
        if (rc != 0) {
                ptl_apini_fini();
                goto out;
        }
        
        rc = ptl_startup_nalnis();
        if (rc != PTL_OK) {
                kpr_finalise();
                ptl_apini_fini();
                goto out;
        }

        rc = ptl_acceptor_start();
        if (rc != PTL_OK) {
                ptl_shutdown_nalnis();
                kpr_finalise();
                ptl_apini_fini();
                goto out;
        }

        ptl_apini.apini_refcount = 1;

        memset (handle, 0, sizeof(*handle));
        LASSERT (!PtlHandleIsEqual(*handle, PTL_INVALID_HANDLE));
        /* Handle can be anything; PTL_INVALID_HANDLE isn't wise though :) */

 out:
        PTL_MUTEX_UP(&ptl_apini.apini_api_mutex);
        return rc;
}

ptl_err_t
PtlNIFini(ptl_handle_ni_t ni)
{
        PTL_MUTEX_DOWN(&ptl_apini.apini_api_mutex);

        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        ptl_apini.apini_refcount--;
        if (ptl_apini.apini_refcount == 0) {
                ptl_acceptor_stop();
                ptl_shutdown_nalnis();
                kpr_finalise();
                ptl_apini_fini();
        }

        PTL_MUTEX_UP(&ptl_apini.apini_api_mutex);
        return PTL_OK;
}

int
PtlNICtl(ptl_handle_ni_t nih, unsigned int cmd, void *arg)
{
        struct portal_ioctl_data *data = arg;
        struct list_head         *tmp;
        ptl_ni_t                 *ni;
        int                       rc;
        unsigned long             flags;
        int                       count;

        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        switch (cmd) {
        case IOC_PORTAL_GET_NI:
                count = data->ioc_count;
                data->ioc_nid = PTL_NID_ANY;
                rc = -ENOENT;

                PTL_LOCK(flags);
                list_for_each (tmp, &ptl_apini.apini_nis) {
                        if (count-- != 0)
                                continue;
         
                        ni = list_entry(tmp, ptl_ni_t, ni_list);
                        data->ioc_nid = ni->ni_nid;
                        rc = 0;
                        break;
                }
                PTL_UNLOCK(flags);
                return rc;

        case IOC_PORTAL_FAIL_NID:
                return ptl_fail_nid(data->ioc_nid, data->ioc_count);
                
        case IOC_PORTAL_ADD_ROUTE:
        case IOC_PORTAL_DEL_ROUTE:
        case IOC_PORTAL_GET_ROUTE:
        case IOC_PORTAL_NOTIFY_ROUTER:
                return kpr_ctl(cmd, arg);

        default:
                ni = ptl_net2ni(data->ioc_net);
                if (ni == NULL)
                        return -EINVAL;

                if (ni->ni_nal->nal_ctl == NULL)
                        rc = -EINVAL;
                else
                        rc = ni->ni_nal->nal_ctl(ni, cmd, arg);

                ptl_ni_decref(ni);
                return rc;
        }
        /* not reached */
}

ptl_err_t
PtlGetId(ptl_handle_ni_t ni_handle, ptl_process_id_t *id)
{
        ptl_ni_t         *ni;
        unsigned long     flags;
        struct list_head *tmp;
        ptl_err_t         rc = PTL_FAIL;

        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        /* pretty useless; just return the NID of the first local interface,
         * that isn't LONAL (it has the same NID on all nodes) */

        PTL_LOCK(flags);

        list_for_each(tmp, &ptl_apini.apini_nis) {
                ni = list_entry(tmp, ptl_ni_t, ni_list);
                if (ni->ni_nal->nal_type == LONAL)
                        continue;

                id->nid = ni->ni_nid;
                id->pid = ptl_apini.apini_pid;
                rc = PTL_OK;
                break;
        }

        PTL_UNLOCK(flags);

        return rc;
}

ptl_err_t
PtlNIHandle(ptl_handle_any_t handle_in, ptl_handle_ni_t *ni_out)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        *ni_out = handle_in;
        return PTL_OK;
}

void
PtlSnprintHandle(char *str, int len, ptl_handle_any_t h)
{
        snprintf(str, len, LPX64, h.cookie);
}

ptl_err_t
PtlGetUid(ptl_handle_ni_t ni_handle, ptl_uid_t *uid)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);
        
        *uid = 0;                               /* fake it */
        return PTL_OK;
}

ptl_err_t
PtlNIDist(ptl_handle_ni_t interface_in, ptl_process_id_t process_in,
          unsigned long *distance_out)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        return 1;                               /* fake it */
}

ptl_err_t 
PtlNIStatus(ptl_handle_ni_t interface_in, ptl_sr_index_t register_in,
            ptl_sr_value_t *status_out)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        return PTL_FAIL;                        /* not supported */
}

ptl_err_t
PtlACEntry(ptl_handle_ni_t ni_in, ptl_ac_index_t index_in,
           ptl_process_id_t match_id_in, ptl_pt_index_t portal_in)
{
        LASSERT (ptl_apini.apini_init);
        LASSERT (ptl_apini.apini_refcount > 0);

        return PTL_FAIL;                        /* not supported */
}
