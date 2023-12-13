/*
 * Copyright 2020 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __RK_MPI_MMZ_H__
#define __RK_MPI_MMZ_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void*               MB_BLK;
typedef int                 RK_S32;
typedef uint32_t            RK_U32;
typedef uint64_t            RK_U64;
typedef void                RK_VOID;

#define RK_MMZ_ALLOC_TYPE_IOMMU     0x00000000
#define RK_MMZ_ALLOC_TYPE_CMA       0x00000001

#define RK_MMZ_ALLOC_CACHEABLE      0x00000000
#define RK_MMZ_ALLOC_UNCACHEABLE    0x00000010

#define RK_MMZ_SYNC_READONLY        0x00000000
#define RK_MMZ_SYNC_WRITEONLY       0x00000001
#define RK_MMZ_SYNC_RW              0x00000002

/*
    申请buffer
    pBlk     返回分配的buffer信息
    u32Len   申请buffer的大小
    u32Flags 申请buffer类型
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_Alloc(MB_BLK *pBlk, RK_U32 u32Len, RK_U32 u32Flags);

/*
    释放buffer
 */
RK_S32 RK_MPI_MMZ_Free(MB_BLK mb);

/*
    获取物理地址
    对于物理连续内存，返回其物理地址
    对于非物理连续内存，返回-1
 */
RK_U64   RK_MPI_MMZ_Handle2PhysAddr(MB_BLK mb);

/*
    获取用户空间虚拟地址
    失败返回NULL
 */
RK_VOID *RK_MPI_MMZ_Handle2VirAddr(MB_BLK mb);

/*
    获取buffer的fd
    失败返回-1
 */
RK_S32   RK_MPI_MMZ_Handle2Fd(MB_BLK mb);

/*
    获取buffer大小
    失败返回 (RK_U64)-1
 */
RK_U64   RK_MPI_MMZ_GetSize(MB_BLK mb);

/*
    通过fd查找到对应的buffer
    成功 返回mb
    失败 返回NULL
 */
MB_BLK   RK_MPI_MMZ_Fd2Handle(RK_S32 fd);

/*
    通过vaddr查找到对应的buffer
    成功 返回mb
    失败 返回NULL
 */
MB_BLK   RK_MPI_MMZ_VirAddr2Handle(RK_VOID *pstVirAddr);

/*
    通过paddr查找到对应的buffer
    成功 返回mb
    失败 返回NULL
 */
MB_BLK   RK_MPI_MMZ_PhyAddr2Handle(RK_U64 paddr);

/*
    查询buffer是否cacheable
    是  返回1
    否  返回0
    不确定  返回-1
 */
RK_S32 RK_MPI_MMZ_IsCacheable(MB_BLK mb);

/*
    flush cache, 在cpu访问前调用
    当offset和length都等于0时候，执行full sync，否则执行partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCacheStart(MB_BLK mb, RK_U32 offset, RK_U32 length, RK_U32 flags);

/*
    flush cache, 在cpu访问结束后调用
    当offset和length都等于0时候，执行full sync，否则执行partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCacheEnd(MB_BLK mb, RK_U32 offset, RK_U32 length, RK_U32 flags);

/*
    flush cache, 在cpu访问前调用
      指定待刷新内存的虚拟地址及其长度，只支持partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCacheVaddrStart(RK_VOID* vaddr, RK_U32 length, RK_U32 flags);

/*
    flush cache, 在cpu访问结束后调用
      指定待刷新内存的虚拟地址及其长度，只支持partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCacheVaddrEnd(RK_VOID* vaddr, RK_U32 length, RK_U32 flags);

/*
    flush cache, 在cpu访问前调用
      指定待刷新内存的物理地址及其长度，只支持partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCachePaddrStart(RK_U64 vaddr, RK_U32 length, RK_U32 flags);

/*
    flush cache, 在cpu访问结束后调用
      指定待刷新内存的物理地址及其长度，只支持partial sync
    成功  返回0
    失败  返回负值
 */
RK_S32 RK_MPI_MMZ_FlushCachePaddrEnd(RK_U64 vaddr, RK_U32 length, RK_U32 flags);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __RK_MPI_MMZ_H__ */