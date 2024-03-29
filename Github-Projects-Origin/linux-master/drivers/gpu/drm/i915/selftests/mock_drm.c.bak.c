#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/*
 * Copyright © 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include "mock_drm.h"

struct drm_file *mock_file(struct drm_i915_private *i915)
{
	struct file *filp;
	struct inode *inode;
	struct drm_file *file;
	int err;

	inode = kzalloc(sizeof(*inode), GFP_KERNEL);
	if (!inode) {
		err = -ENOMEM;
		goto err;
	}

	inode->i_rdev = i915->drm.primary->index;

	filp = kzalloc(sizeof(*filp), GFP_KERNEL);
	if (!filp) {
		err = -ENOMEM;
		goto err_inode;
	}

	err = drm_open(inode, filp);
	if (err)
		goto err_filp;

	file = filp->private_data;
	memset(&file->filp, POISON_INUSE, sizeof(file->filp));
	file->authenticated = true;

	kfree(filp);
	kfree(inode);
	return file;

err_filp:
	kfree(filp);
err_inode:
	kfree(inode);
err:
	return ERR_PTR(err);
}

void mock_file_free(struct drm_i915_private *i915, struct drm_file *file)
{
	struct file filp = { .private_data = file };

	drm_release(NULL, &filp);
}
