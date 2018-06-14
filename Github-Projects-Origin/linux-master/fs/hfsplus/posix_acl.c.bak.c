#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
// SPDX-License-Identifier: GPL-2.0
/*
 * linux/fs/hfsplus/posix_acl.c
 *
 * Vyacheslav Dubeyko <slava@dubeyko.com>
 *
 * Handler for Posix Access Control Lists (ACLs) support.
 */

#include "hfsplus_fs.h"
#include "xattr.h"
#include "acl.h"

struct posix_acl *hfsplus_get_posix_acl(struct inode *inode, int type)
{
	struct posix_acl *acl;
	char *xattr_name;
	char *value = NULL;
	ssize_t size;

	hfs_dbg(ACL_MOD, "[%s]: ino %lu\n", __func__, inode->i_ino);

	switch (type) {
	case ACL_TYPE_ACCESS:
		xattr_name = XATTR_NAME_POSIX_ACL_ACCESS;
		break;
	case ACL_TYPE_DEFAULT:
		xattr_name = XATTR_NAME_POSIX_ACL_DEFAULT;
		break;
	default:
		return ERR_PTR(-EINVAL);
	}

	size = __hfsplus_getxattr(inode, xattr_name, NULL, 0);

	if (size > 0) {
		value = (char *)hfsplus_alloc_attr_entry();
		if (unlikely(!value))
			return ERR_PTR(-ENOMEM);
		size = __hfsplus_getxattr(inode, xattr_name, value, size);
	}

	if (size > 0)
		acl = posix_acl_from_xattr(&init_user_ns, value, size);
	else if (size == -ENODATA)
		acl = NULL;
	else
		acl = ERR_PTR(size);

	hfsplus_destroy_attr_entry((hfsplus_attr_entry *)value);

	return acl;
}

static int __hfsplus_set_posix_acl(struct inode *inode, struct posix_acl *acl,
				   int type)
{
	int err;
	char *xattr_name;
	size_t size = 0;
	char *value = NULL;

	hfs_dbg(ACL_MOD, "[%s]: ino %lu\n", __func__, inode->i_ino);

	switch (type) {
	case ACL_TYPE_ACCESS:
		xattr_name = XATTR_NAME_POSIX_ACL_ACCESS;
		break;

	case ACL_TYPE_DEFAULT:
		xattr_name = XATTR_NAME_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;

	default:
		return -EINVAL;
	}

	if (acl) {
		size = posix_acl_xattr_size(acl->a_count);
		if (unlikely(size > HFSPLUS_MAX_INLINE_DATA_SIZE))
			return -ENOMEM;
		value = (char *)hfsplus_alloc_attr_entry();
		if (unlikely(!value))
			return -ENOMEM;
		err = posix_acl_to_xattr(&init_user_ns, acl, value, size);
		if (unlikely(err < 0))
			goto end_set_acl;
	}

	err = __hfsplus_setxattr(inode, xattr_name, value, size, 0);

end_set_acl:
	hfsplus_destroy_attr_entry((hfsplus_attr_entry *)value);

	if (!err)
		set_cached_acl(inode, type, acl);

	return err;
}

int hfsplus_set_posix_acl(struct inode *inode, struct posix_acl *acl, int type)
{
	int err;

	if (type == ACL_TYPE_ACCESS && acl) {
		err = posix_acl_update_mode(inode, &inode->i_mode, &acl);
		if (err)
			return err;
	}
	return __hfsplus_set_posix_acl(inode, acl, type);
}

int hfsplus_init_posix_acl(struct inode *inode, struct inode *dir)
{
	int err = 0;
	struct posix_acl *default_acl, *acl;

	hfs_dbg(ACL_MOD,
		"[%s]: ino %lu, dir->ino %lu\n",
		__func__, inode->i_ino, dir->i_ino);

	if (S_ISLNK(inode->i_mode))
		return 0;

	err = posix_acl_create(dir, &inode->i_mode, &default_acl, &acl);
	if (err)
		return err;

	if (default_acl) {
		err = __hfsplus_set_posix_acl(inode, default_acl,
					      ACL_TYPE_DEFAULT);
		posix_acl_release(default_acl);
	}

	if (acl) {
		if (!err)
			err = __hfsplus_set_posix_acl(inode, acl,
						      ACL_TYPE_ACCESS);
		posix_acl_release(acl);
	}
	return err;
}