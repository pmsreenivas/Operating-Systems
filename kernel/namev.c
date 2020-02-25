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

#include "kernel.h"
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function.
 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{
        KASSERT(NULL != dir); /* the "dir" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.a)\n");
        KASSERT(NULL != name); /* the "name" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.a)\n");
        KASSERT(NULL != result); /* the "result" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.a)\n");
	
        if(dir->vn_ops->lookup == NULL){
            dbg(DBG_PRINT, "(GRADING2B)\n");
            return -ENOTDIR;
        }
        // if(len > NAME_LEN){
        //     dbg(DBG_PRINT, "(GRADING2S NV_3)\n");
        //    return -ENAMETOOLONG;

        // }
        dbg(DBG_PRINT, "(GRADING2B)\n");
        return dir->vn_ops->lookup(dir,name,len,result);
       
}


/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
 */



int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{      
        KASSERT(NULL != pathname); /* the "pathname" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
        KASSERT(NULL != namelen); /* the "namelen" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
        KASSERT(NULL != name); /* the "name" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
        KASSERT(NULL != res_vnode); /* the "res_vnode" argument must be non-NULL */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
         vnode_t* localBase = base;
        // if(strlen(pathname) > MAXPATHLEN){
        //     dbg(DBG_PRINT, "(GRADING2S NV_5)\n");
        //     return -ENAMETOOLONG;
        // }
        // if(pathname[0] == '\0'){
        //     dbg(DBG_PRINT, "(GRADING2S NV_6)\n");
        //     return -EINVAL;
        // }
        if(base == NULL){
            localBase = curproc->p_cwd;
            dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        if(pathname[0] == '/'){
            localBase = vfs_root_vn;
            dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        int left, right, lur;
        lur = left = right = 0;
        lur = 5;
        size_t len = 0;
        int EXL = 0;
        int EXR = 0;
        left = 0;
        if(pathname[0] == '/' && pathname[1] == '\0'){
            dbg(DBG_PRINT, "(GRADING2B)\n");
            goto L;
        }
        if(pathname[1] == '\0'){
            left = 0;
            len = 1;
            dbg(DBG_PRINT, "(GRADING2B)\n");
            goto L;
        }
        while(pathname[EXR] != '\0'){
            EXR++;
            dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        EXR--;
        while(pathname[EXR] == '/'){
            EXR--;
            dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        while(pathname[EXL] == '/'){
            EXL++;
            dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        right = EXL - 1;
        while(right!=EXR){
            for(left = right + 1; pathname[left] == '/'; left++){
                dbg(DBG_PRINT, "(GRADING2B)\n");
            }
            for(right = left; pathname[right + 1] != '/' && right != EXR; right++){
                dbg(DBG_PRINT, "(GRADING2B)\n");
            }
            len = right - left + 1;
	    if(len > NAME_LEN){
            dbg(DBG_PRINT, "(GRADING2B)\n");
		return -ENAMETOOLONG;
		}
	    if(right == EXR){
	//	if(pathname[EXR+1] == '/') {
	//		vnode_t *temp = NULL;
	//		lur = lookup(localBase, &pathname[left], len, &temp);
	//		if(lur == 0) {
	//	 		if(!S_ISDIR(temp->vn_mode)){
	//				vput(temp);
        //        			return -ENOTDIR;
	//			}
	//		vput(temp);
	//		}
        //    	}
            dbg(DBG_PRINT, "(GRADING2B)\n");
                 goto L;
            }
            // if(!S_ISDIR(localBase->vn_mode)){
            //     dbg(DBG_PRINT, "(GRADING2S NV_19)\n");
            //     return -ENOTDIR;
            // }
            KASSERT(NULL != localBase); /* pathname resolution must start with a valid directory */
    dbg(DBG_PRINT, "(GRADING2A 2.b)\n");
            lur = lookup(localBase, &pathname[left], len, res_vnode);
            if(lur != 0){
            dbg(DBG_PRINT, "(GRADING2B)\n");
                return lur;
            }
	   localBase = *res_vnode;
           vput(localBase);
           dbg(DBG_PRINT, "(GRADING2B)\n");
        }
        L:
	*namelen = len;
        *name = &pathname[left];
        *res_vnode = vget(localBase->vn_fs,localBase->vn_vno);
        dbg(DBG_PRINT, "(GRADING2B)\n");
        return 0;
}


/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fcntl.h>.  If the O_CREAT flag is specified and the file does
 * not exist, call create() in the parent directory vnode. However, if the
 * parent directory itself does not exist, this function should fail - in all
 * cases, no files or directories other than the one at the very end of the path
 * should be created.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)
{         
//         if(strlen(pathname) > MAXPATHLEN){
//             dbg(DBG_PRINT, "(GRADING2S NV_22)\n");
//             return -ENAMETOOLONG;
//         }
//         if(pathname[0] == '\0'){
//             dbg(DBG_PRINT, "(GRADING2S NV_23)\n");
//             return -EINVAL;
//         }
        size_t len;
        const char *filename;
        int returned;
	vnode_t *parent = NULL;
        if((returned = dir_namev(pathname, &len, &filename, base, &parent)) < 0){
	   dbg(DBG_PRINT, "(GRADING2B)\n");
            return returned;
        }

        returned = lookup(parent, filename, len, res_vnode);
        if(returned == -ENOENT){
            dbg(DBG_PRINT, "(GRADING2B)\n");
            if((flag & O_CREAT) == O_CREAT){
                if ((flag & O_CREAT)) { /* if file does not exist inside dir_vnode, need to make sure you can create the file */
                    KASSERT(NULL != parent->vn_ops->create);

    dbg(DBG_PRINT, "(GRADING2A 2.c)\n");

    
                    dbg(DBG_PRINT, "(GRADING2B)\n");
                }
                returned = parent->vn_ops->create(parent,filename,len,res_vnode);
	 	
        	vput(parent);
            dbg(DBG_PRINT, "(GRADING2B)\n");
		return 0;
            }
	    else {
			vput(parent);
            dbg(DBG_PRINT, "(GRADING2B)\n");
			return returned;
		}
        
        } else if(returned < 0){
            //vput(*res_vnode);
	    vput(parent);
        dbg(DBG_PRINT, "(GRADING2B)\n");
            return returned;
        }

	vput(parent);
    dbg(DBG_PRINT, "(GRADING2B)\n");
        return 0;
}


#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */
