/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (C) 2001-2003 Cluster File Systems, Inc.
 *   Author: Andreas Dilger <adilger@clusterfs.com>
 *
 *   This file is part of Lustre, http://www.lustre.org.
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
 *
 * OST<->MDS recovery logging infrastructure.
 *
 * Invariants in implementation:
 * - we do not share logs among different OST<->MDS connections, so that
 *   if an OST or MDS fails it need only look at log(s) relevant to itself
 */

#define DEBUG_SUBSYSTEM S_LOG

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/fs.h>
#include <linux/obd_class.h>
#include <linux/lustre_log.h>
#include <portals/list.h>

/* Allocate a new log or catalog handle */
struct llog_handle *llog_alloc_handle(void)
{
        struct llog_handle *loghandle;
        ENTRY;

        OBD_ALLOC(loghandle, sizeof(*loghandle));
        if (loghandle == NULL)
                RETURN(ERR_PTR(-ENOMEM));

        OBD_ALLOC(loghandle->lgh_hdr, LLOG_CHUNK_SIZE);
        if (loghandle->lgh_hdr == NULL) {
                OBD_FREE(loghandle, sizeof(*loghandle));
                RETURN(ERR_PTR(-ENOMEM));
        }

        INIT_LIST_HEAD(&loghandle->lgh_list);
        sema_init(&loghandle->lgh_lock, 1);

        RETURN(loghandle);
}
EXPORT_SYMBOL(llog_alloc_handle);

void llog_free_handle(struct llog_handle *loghandle)
{
        if (!loghandle)
                return;

        list_del_init(&loghandle->lgh_list);
        OBD_FREE(loghandle->lgh_hdr, LLOG_CHUNK_SIZE);
        OBD_FREE(loghandle, sizeof(*loghandle));
}
EXPORT_SYMBOL(llog_free_handle);

int llog_buf2reclen(int len)
{
        int size;

        size = sizeof(struct llog_rec_hdr) + size_round(len) + sizeof(__u32);
        return size;
}




/* Remove a log entry from the catalog.
 * Assumes caller has already pushed us into the kernel context and is locking.
 */
int llog_delete_log(struct llog_handle *cathandle,struct llog_handle *loghandle)
{
        struct llog_cookie *lgc = &loghandle->lgh_cookie;
        int catindex = lgc->lgc_index;
        struct llog_log_hdr *llh = cathandle->lgh_hdr;
        loff_t offset = 0;
        int rc = 0;
        ENTRY;

        CDEBUG(D_HA, "log "LPX64":%x empty, closing\n",
               lgc->lgc_lgl.lgl_oid, lgc->lgc_lgl.lgl_ogen);

        if (!ext2_clear_bit(catindex, llh->llh_bitmap)) {
                CERROR("catalog index %u already clear?\n", catindex);
                LBUG();
        } else {
                rc = lustre_fwrite(cathandle->lgh_file, llh, sizeof(*llh),
                                   &offset);

                if (rc != sizeof(*llh)) {
                        CERROR("log %u cancel error: rc %d\n", catindex, rc);
                        if (rc >= 0)
                                rc = -EIO;
                } else
                        rc = 0;
        }
        RETURN(rc);
}
EXPORT_SYMBOL(llog_delete_log);

int llog_process_log(struct llog_handle *loghandle, llog_cb_t cb, void *data)
{
        struct llog_log_hdr *llh = loghandle->lgh_hdr;
        void *buf;
        __u64 cur_offset = LLOG_CHUNK_SIZE;
        int rc = 0, index = 0;
        ENTRY;

        OBD_ALLOC(buf, PAGE_SIZE);
        if (!buf)
                RETURN(-ENOMEM);

        while (rc == 0) {
                struct llog_rec_hdr *rec;

                /* there is likely a more efficient way than this */
                while (index < LLOG_BITMAP_BYTES * 8 &&
                       !ext2_test_bit(index, llh->llh_bitmap))
                        ++index;

                if (index >= LLOG_BITMAP_BYTES * 8)
                        break;

                rc = llog_next_block(loghandle, 0, index, 
                                     &cur_offset, buf, PAGE_SIZE);
                if (rc)
                        RETURN(rc);

                rec = buf;

                /* skip records in buffer until we are at the one we want */
                while (rec->lrh_index < index) {
                        if (rec->lrh_index == 0)
                                RETURN(0); /* no more records */

                        cur_offset += rec->lrh_len;
                        rec = ((void *)rec + rec->lrh_len);

                        if ((void *)rec > buf + PAGE_SIZE) {
                                CERROR("log index %u not in log @ "LPU64"\n",
                                       index, cur_offset);
                                LBUG(); /* record not in this buffer? */
                        }

                        rc = cb(loghandle, rec, data);
                        ++index;
                }
        }

        RETURN(rc);
}
EXPORT_SYMBOL(llog_process_log);


int llog_write_header(struct llog_handle *loghandle, int size)
{
        struct llog_log_hdr *llh;

        LASSERT(sizeof(*llh) == LLOG_CHUNK_SIZE);

        if (loghandle->lgh_file->f_dentry->d_inode->i_size)
                RETURN(-EBUSY);

        llh = loghandle->lgh_hdr;
        llh->llh_size = size;
        llh->llh_hdr.lrh_type = LLOG_OBJECT_MAGIC;
        llh->llh_hdr.lrh_len = llh->llh_tail.lrt_len = sizeof(*llh);
        llh->llh_timestamp = LTIME_S(CURRENT_TIME);
        llh->llh_bitmap_offset = offsetof(typeof(*llh), llh_bitmap);
        memcpy(&llh->llh_tgtuuid, tgtuuid, sizeof(llh->llh_tgtuuid));
        loghandle->lgh_tgtuuid = &llh->llh_tgtuuid;

        /* write the header record in the log */
        rc = llog_write_record(loghandle, &llh, NULL, NULL, 0);
        if (rc > 0) 
                rc = 0;
        RETURN(rc);
}
EXPORT_SYMBOL(llog_write_header);
