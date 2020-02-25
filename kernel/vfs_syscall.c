/******************************************************************************/
/* Important Spring 2019 CSCI 402 usage information:                          */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.2 2018/05/27 03:57:26 cvsps Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/*
 * Syscalls for vfs. Refer to comments or man pages for implementation.
 * Do note that you don't need to set errno, you should just return the
 * negative error code.
 */

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read vn_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */

int isvalid_filedes(int fd) {
	if (fd < 0 || fd >= NFILES) {

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return 1;
	}	
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return 0;
}

int validate_file_obj(file_t* file_ptr, int mode1, int mode2){
	// if(file_ptr == NULL) {
 //       dbg(DBG_PRINT, "(GRADING2S VF_3)\n");
	// 	return -EBADF;
	// }

	if(S_ISDIR(file_ptr->f_vnode->vn_mode)) {
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EISDIR;
	}
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return 0;
}

int
do_read(int fd, void *buf, size_t nbytes)
{

	if(isvalid_filedes(fd)){
       dbg(DBG_PRINT, "(GRADING2B\n");
		return -EBADF;
	}

	file_t* file_ptr = fget(fd);
	if(file_ptr == NULL) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	if(!((file_ptr->f_mode & FMODE_READ) == FMODE_READ)) {
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	int error_code = validate_file_obj(file_ptr, FMODE_READ, FMODE_READ);
	if (error_code != 0){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return error_code;
	}
	
	int bytes_read = file_ptr->f_vnode->vn_ops->read(file_ptr->f_vnode, file_ptr->f_pos, buf, nbytes);
	file_ptr->f_pos += bytes_read;
	fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
        return bytes_read;
}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * vn_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */


int
do_write(int fd, const void *buf, size_t nbytes)
{
	if(isvalid_filedes(fd)) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	
	file_t *file_ptr = fget(fd);
	if(file_ptr == NULL) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}


	// if(S_ISDIR(file_ptr->f_vnode->vn_mode)) {
	// 	fput(file_ptr);
 //       dbg(DBG_PRINT, "(GRADING2S VF_13)\n");
	// 	return -EISDIR;
	// }
 
	if( ! ( (file_ptr->f_mode & FMODE_WRITE) == FMODE_WRITE || (file_ptr->f_mode & FMODE_APPEND) == FMODE_APPEND)) {
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	if ( (file_ptr->f_mode & FMODE_APPEND) == FMODE_APPEND) {
		file_ptr->f_pos = do_lseek(fd, 0, SEEK_END);
       dbg(DBG_PRINT, "(GRADING2B)\n");
	}
	
	int bytes_written = file_ptr->f_vnode->vn_ops->write(file_ptr->f_vnode, file_ptr->f_pos, buf, nbytes);
	if(bytes_written > 0) {
		file_ptr->f_pos += bytes_written;
       dbg(DBG_PRINT, "(GRADING2B)\n");
	}

	KASSERT((S_ISCHR(file_ptr->f_vnode->vn_mode)) ||
    (S_ISBLK(file_ptr->f_vnode->vn_mode)) ||
    ((S_ISREG(file_ptr->f_vnode->vn_mode)) && (file_ptr->f_pos <= file_ptr->f_vnode->vn_len))); /* cursor must not go past end of file for these file types */
    dbg(DBG_PRINT, "(GRADING2A 3.a)\n");	
	fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return bytes_written;
}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */
int
do_close(int fd)
{
	if(isvalid_filedes(fd) || curproc->p_files[fd] == NULL){

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	fput(curproc->p_files[fd]);
	curproc->p_files[fd] = NULL;
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return 0;
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
	if(isvalid_filedes(fd)) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	file_t *file_ptr = fget(fd);
	if(file_ptr == NULL) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	
	int fd1 = get_empty_fd(curproc);
	// if(fd1 < 0){
	// 	fput(file_ptr);
 //       dbg(DBG_PRINT, "(GRADING2S VF_22)\n");
	// 	return fd1;
	// }
	curproc->p_files[fd1] = file_ptr;
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return fd1;
}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
	if(isvalid_filedes(ofd) || isvalid_filedes(nfd)) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	
	file_t *file_ptr = fget(ofd);

	if(file_ptr == NULL){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	if (nfd == ofd) {
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return nfd;
	}

	// if(curproc->p_files[nfd] != NULL && nfd != ofd){
 //       dbg(DBG_PRINT, "(GRADING2S VF_27)\n");
	// 	do_close(nfd);
	// }
	
	curproc->p_files[nfd] = file_ptr;
       dbg(DBG_PRINT, "(GRADING2B)\n");
        return nfd;
}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mknod(const char *path, int mode, unsigned devid)
{
	
	// if (mode != S_IFCHR && mode != S_IFBLK) {
 //       dbg(DBG_PRINT, "(GRADING2S VF_29)\n");
	// 	return -EINVAL;
	// }
	
	// int length = strlen(path);
	// if(length > MAXPATHLEN || length <= 0){
 //       dbg(DBG_PRINT, "(GRADING2S VF_30)\n");
	// 	return -ENAMETOOLONG; 
	// }
	const char *name = NULL;
	size_t len = 0;
	vnode_t *v_node = NULL;
	int return_code = dir_namev(path, &len, &name, NULL, &v_node);
	// if(return_code < 0){
		
 //       dbg(DBG_PRINT, "(GRADING2S VF_31)\n");
	// 	return return_code; 
	// }
	//for dirname call we decreasing the reference call
	vput(v_node);
	
	return_code = lookup(v_node, name, len, &v_node);
	// if(return_code == 0) {
		
	// 	vput(v_node);
 //       dbg(DBG_PRINT, "(GRADING2S VF_32)\n");
	// 	return -EEXIST;
	// }
	KASSERT(NULL != v_node->vn_ops->mknod);

    dbg(DBG_PRINT, "(GRADING2A 3.b)\n");
	
		
	//vput(v_node);
	if(return_code == -ENOENT) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return v_node->vn_ops->mknod(v_node, name, len, mode, devid);
	}

       // dbg(DBG_PRINT, "(GRADING2S VF_34)\n");
	return return_code;
}

/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int do_mkdir(const char *path)
{

	// int length = strlen(path);
	// if(length > MAXPATHLEN || length <= 0){
 //       dbg(DBG_PRINT, "(GRADING2S VF_35)\n");
	// 	return -ENAMETOOLONG; 
	// }
	const char *name = NULL;
	size_t len = 0;
	vnode_t *v_node = NULL;
	vnode_t *rvn = NULL;
	int return_code = dir_namev(path, &len, &name, NULL, &v_node);
	if(return_code < 0){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_code; 
	}
	//for dirname call we decreasing the reference call

	return_code = lookup(v_node, name, len, &rvn);
	if(return_code == 0) {
		vput(v_node);
		vput(rvn);
		
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EEXIST;
	}
	//vput(v_node);
	if(NULL == v_node->vn_ops->mkdir){
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTDIR;
	}
	KASSERT(NULL != v_node->vn_ops->mkdir);
    dbg(DBG_PRINT, "(GRADING2A 3.c)\n");
	if(return_code == -ENOENT) {
		return_code = v_node->vn_ops->mkdir(v_node, name, len);
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_code;
	}
	// if(return_code == -ENOTDIR) {
	// 	vput(v_node);
 //       dbg(DBG_PRINT, "(GRADING2S VF_40)\n");
	// 	return -ENOTDIR;
	// }
       // dbg(DBG_PRINT, "(GRADING2S VF_41)\n");
	return return_code;
}

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its final component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_rmdir(const char *path)
{
	// int length = strlen(path);
	// if(length > MAXPATHLEN || length <= 0){

 //       dbg(DBG_PRINT, "(GRADING2S VF_42)\n");
	// 	return -ENAMETOOLONG; 
	// }
	const char *name = NULL;
	size_t len = 0;
	vnode_t *v_node = NULL;
	int return_code = dir_namev(path, &len, &name, NULL, &v_node);
	if(return_code < 0){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_code; 
	}

	if(name_match(".", name, len)) {
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}
	if(name_match("..", name, len)) {
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTEMPTY;
	}
	if(NULL == v_node->vn_ops->rmdir){
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTDIR;
	}
	KASSERT(NULL != v_node->vn_ops->rmdir);
    dbg(DBG_PRINT, "(GRADING2A 3.d)\n");
    dbg(DBG_PRINT, "(GRADING2B)\n");
	vput(v_node);
	return_code = v_node->vn_ops->rmdir(v_node, name, len);
       dbg(DBG_PRINT, "(GRADING2B)\n");
       
        return return_code;
}

/*
 * Similar to do_rmdir, but for files.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EPERM
 *        path refers to a directory.
 *      o ENOENT
 *        Any component in path does not exist, including the element at the
 *        very end.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
	// int length = strlen(path);
	// if(length > MAXPATHLEN || length <= 0){
 //       dbg(DBG_PRINT, "(GRADING2S VF_48)\n");
	// 	return -ENAMETOOLONG; 
	// }
	const char *name = NULL;
	size_t len = 0;
	vnode_t *v_node = NULL;
	int return_code = dir_namev(path, &len, &name, NULL, &v_node);
	
	// if(return_code < 0){

 //       dbg(DBG_PRINT, "(GRADING2S VF_49)\n");
	// 	return return_code; 
	// }
	
	vnode_t *v_node_1 = NULL;
	return_code = lookup(v_node, name, len, &v_node_1);

	if(return_code < 0){
		vput(v_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_code;
	}
	if(S_ISDIR(v_node_1->vn_mode)) {
		vput(v_node);
		vput(v_node_1);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EPERM;
	}
	// if(NULL == v_node->vn_ops->unlink){
	// 	vput(v_node_1);
 //       dbg(DBG_PRINT, "(GRADING2S VF_52)\n");
	// 	return -ENOTDIR;
	// }
	
	KASSERT(NULL != v_node->vn_ops->unlink); 
    dbg(DBG_PRINT, "(GRADING2A 3.e)\n");
    dbg(DBG_PRINT, "(GRADING2B)\n");
	return_code = v_node->vn_ops->unlink(v_node, name, len);
	vput(v_node);
	vput(v_node_1);
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return return_code;
}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EPERM
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
	int from_len = strlen(from);
	int to_len = strlen(to);
	
	// if(from_len > MAXPATHLEN || to_len  > MAXPATHLEN){
 //       dbg(DBG_PRINT, "(GRADING2S VF_54)\n");
	// 	return -ENAMETOOLONG; 
	// }

	const char *name = NULL;
	size_t len = 0;
	vnode_t *from_node = NULL;
	int open_status = open_namev(from, 0, &from_node, NULL);
	
	if(open_status < 0){
		//vput(from_node);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return open_status;
	}
	
	// if(S_ISDIR(from_node->vn_mode)) {
	// 	vput(from_node);
 //       dbg(DBG_PRINT, "(GRADING2S VF_56)\n");
	// 	return -EPERM;
	// }

	//release as soon done with it	
	vput(from_node); // seems proper

	vnode_t *to_node = NULL;
	int dir_status = dir_namev(to, &len, &name, NULL, &to_node);
	// if(dir_status < 0){
 //       dbg(DBG_PRINT, "(GRADING2S VF_57)\n");
	// 	return dir_status;
	// }
	//vput matched
	vput(to_node);

		
	//check whether to already exist
	vnode_t *is_duplicate = NULL;
	int dup_status = lookup(to_node, name, len, &is_duplicate);
	// if(dup_status == 0) {
	// 	vput(is_duplicate);
 //       dbg(DBG_PRINT, "(GRADING2S VF_58)\n");
	// 	return -EEXIST;
	// }
	// if(dup_status == -ENOENT) {
 //       dbg(DBG_PRINT, "(GRADING2S VF_59)\n");
	// 	return is_duplicate->vn_ops->link(from_node, is_duplicate, name, len);
	// }

       dbg(DBG_PRINT, "(GRADING2B)\n");
        return dup_status;
}

/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
	int return_code = 0;
	return_code = do_link(oldname, newname);
	if(return_code < 0){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_code;
	}

        return_code = do_unlink(oldname);

       // dbg(DBG_PRINT, "(GRADING2S VF_62)\n");
	return return_code;
}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
	if(strlen(path) > MAXPATHLEN) {

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENAMETOOLONG;
	}
	
	vnode_t *chdir;
	int return_value = open_namev(path, 0, &chdir, NULL);
	if (return_value < 0) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return return_value;
	}

	if (!S_ISDIR(chdir->vn_mode)) {
		vput(chdir);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTDIR;
	}
	vput(curproc->p_cwd);
	curproc->p_cwd = chdir;
       dbg(DBG_PRINT, "(GRADING2B)\n");
        return 0;
}

/* Call the readdir vn_op on the given fd, filling in the given dirent_t*.
 * If the readdir vn_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */
int
do_getdent(int fd, struct dirent *dirp)
{
	if(isvalid_filedes(fd)) {

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	file_t *file_ptr = fget(fd);
	if(file_ptr == NULL) {

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}
	if(!(S_ISDIR(file_ptr->f_vnode->vn_mode))) {
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENOTDIR;
	}

	int bytes_read = file_ptr->f_vnode->vn_ops->readdir(file_ptr->f_vnode, file_ptr->f_pos, dirp);
	file_ptr->f_pos += bytes_read;
	fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
	return (bytes_read == 0) ? 0 : sizeof(*dirp);
}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 */
int
do_lseek(int fd, int offset, int whence)
{
	if(isvalid_filedes(fd)) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	if((whence != SEEK_END) && (whence != SEEK_CUR) && (whence != SEEK_SET)) {
		
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}
	
	file_t* file_ptr = fget(fd);

	if(file_ptr == NULL) {

       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EBADF;
	}

	if(whence == SEEK_CUR){

		file_ptr->f_pos += offset; 
       dbg(DBG_PRINT, "(GRADING2B)\n"); 
	}

	else if(whence == SEEK_SET) {
		
		file_ptr->f_pos = offset;
       dbg(DBG_PRINT, "(GRADING2B)\n");
	}

	else {
		file_ptr->f_pos = file_ptr->f_vnode->vn_len + offset;
       dbg(DBG_PRINT, "(GRADING2B)\n");
	}

	

	if(file_ptr->f_pos < 0){
		file_ptr->f_pos = 0;
		fput(file_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}

	fput(file_ptr);

       dbg(DBG_PRINT, "(GRADING2B)\n");
	return file_ptr->f_pos;

}


/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o EINVAL
 *        path is an empty string.
 */
int
do_stat(const char *path, struct stat *buf)
{       

	// if(strlen(path) > MAXPATHLEN) {
 //       dbg(DBG_PRINT, "(GRADING2S VF_78)\n");
	// 	return -ENAMETOOLONG;
	// }

	if(strlen(path) <= 0) {
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return -EINVAL;
	}
	
	vnode_t* stat_ptr = NULL;
	const char *name = NULL;
	size_t len = 0;
	int st_status = dir_namev(path, &len, &name, NULL, &stat_ptr);
	// if(st_status < 0){
 //       dbg(DBG_PRINT, "(GRADING2S VF_80)\n");
	// 	return st_status;
	// }
	vput(stat_ptr);
	st_status = lookup(stat_ptr, name, len, &stat_ptr);
	if(st_status < 0){
       dbg(DBG_PRINT, "(GRADING2B)\n");
		return st_status;
	}
	KASSERT(NULL != stat_ptr->vn_ops->stat);
	    dbg(DBG_PRINT, "(GRADING2A 3.f)\n");
    dbg(DBG_PRINT, "(GRADING2B)\n");
	vput(stat_ptr);
       dbg(DBG_PRINT, "(GRADING2B)\n");
        return stat_ptr->vn_ops->stat(stat_ptr, buf);
}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
