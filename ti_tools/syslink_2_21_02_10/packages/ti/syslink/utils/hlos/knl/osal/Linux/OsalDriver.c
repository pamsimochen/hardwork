/*
 *  @file   OsalDriver.c
 *
 *  @brief      Linux kernel side IOCTL manager interface implementation.
 *
 *              This abstracts the IOCTL calls interface in the Kernel
 *              code. This will expose the interface to register and unregister
 *              functions with their command ids. There will be interface also
 *              for getting the function from table. This will also interface
 *              with command ids arbitrator.
 *
 *
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */



/* Standard headers */
#include <ti/syslink/Std.h>

/* Linux specific header files */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/major.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>

#ifndef CONFIG_MMU
#include <linux/backing-dev.h>
#endif


/* OSAL and utils headers */
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/TraceDrv.h>
#include <ti/syslink/inc/TraceDrvDefs.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/inc/CacheDrv.h>
#include <ti/syslink/inc/CacheDrvDefs.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/inc/OsalDrvDefs.h>

#include <ti/syslink/utils/String.h>

#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>


/* =============================================================================
 * macros & defines
 * =============================================================================
 */
/*!
 *  @def    MAX_OSAL_DRIVERS
 *  @brief  Maximum number of os driver that can be created.
 */
#define MAX_OSAL_DRIVERS       (20)

/*!
 *  @def    OSALDRIVER_CLASSNAME
 *  @brief  Name of the driver class.
 */
#define OSALDRIVER_CLASSNAME   "syslinkipc"

/*!
 *  @brief  Driver minor number for OSAL.
 */
#define OSAL_DRV_MINOR_NUMBER   12u

#define NUM_CACHEMMAP_ENTRIES 16


/* =============================================================================
 * Structure & Enums
 * =============================================================================
 */

typedef struct OsalDrv_CachemmapParams {
    ULong physAddr;
} OsalDrv_CachemmapParams;

/* Structure for defining Driver state. */
typedef struct OsalDriver_ModuleObject {
    IGateProvider_Handle gateHandle;
    /* Handle for protecting the obj list operation */
    UInt32            refCount;
    /* Reference count for this module */
    struct class *    _class;
    /* class struct for device create. */
    UInt32            major;
    /* Dynamically allocated major number of the device */
    OsalDriver_Handle osalDrvHandle;
    /* */
    OsalDrv_CachemmapParams cachemmap[NUM_CACHEMMAP_ENTRIES];
    /* physical addrs to be cached for mmap */
} OsalDriver_ModuleObject;


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int OsalDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int OsalDrv_close (struct inode * inode, struct file * filp);

/* Linux driver function to map memory regions to user space. */
static int OsalDrv_mmap (struct file * filp, struct vm_area_struct * vma);

typedef struct OsalDrv_NormalMemory {
    UInt32       addr;
    UInt32       size;
} OsalDrv_NormalMemory;

static Bool _OsalDrv_isNormalMemory(UInt32 addr, UInt32 size);
/*
 * This table defines all memory ranges that need to be mapped as Normal memory
 */
#if defined(SYSLINK_VARIANT_TI814X)
#define ADDR_TABLE_SIZE 6
static OsalDrv_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40300000,
            .size     = 0x20000
        },
        { /* DSP L2 RAM */
            .addr     = 0x40800000,
            .size     = 0x40000
        },
        { /* DSP L1P RAM */
            .addr     = 0x40E00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x40F00000,
            .size     = 0x8000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x10000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#elif defined(SYSLINK_VARIANT_TI816X)
#define ADDR_TABLE_SIZE 7
static OsalDrv_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC0 */
            .addr     = 0x40300000,
            .size     = 0x40000
        },
        { /* OCMC1 */
            .addr     = 0x40400000,
            .size     = 0x40000
        },
        { /* DSP L2 RAM */
            .addr     = 0x40800000,
            .size     = 0x40000
        },
        { /* DSP L1P RAM */
            .addr     = 0x40E00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x40F00000,
            .size     = 0x8000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x40000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#elif defined(SYSLINK_VARIANT_TI811X)
#define ADDR_TABLE_SIZE 6
static OsalDrv_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40300000,
            .size     = 0x40000
        },
        { /* DSP L2 RAM */
            .addr     = 0x40800000,
            .size     = 0x40000
        },
        { /* DSP L1P RAM */
            .addr     = 0x40E00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x40F00000,
            .size     = 0x8000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x10000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#elif defined(SYSLINK_VARIANT_TI813X)
#define ADDR_TABLE_SIZE 3
static OsalDrv_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40300000,
            .size     = 0x40000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x10000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#elif defined(SYSLINK_PLATFORM_OMAP3530)
#define ADDR_TABLE_SIZE 5
static OsalDrv_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40200000,
            .size     = 0x10000
        },
        { /* DSP L2 RAM */
            .addr     = 0x5C7F8000,
            .size     = 0x18000
        },
        { /* DSP L1P RAM */
            .addr     = 0x5CE00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x5CF04000,
            .size     = 0x14000
        },
        { /* SDRAM */
            .addr     = 0x70000000,
            .size     = 0x90000000
        },
    };

#else
#define ADDR_TABLE_SIZE 0
static OsalDrv_NormalMemory addrTable[] =
    {
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };

#endif
/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long OsalDrvDrv_ioctl (struct file *  filp,
                              unsigned int   cmd,
                              unsigned long  args);

#ifndef CONFIG_MMU
/*!
 *  @brief  Linux driver function to read of the driver object.
 */
static ssize_t
OsalDrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);

/*!
 *  @brief  Linux driver function to unmapp the driver object.
 */
static unsigned long OsalDrv_unmapped_area(struct file *filp,
                                   unsigned long orig_addr,
                                   unsigned long len,
                                   unsigned long pgoff,
                                   unsigned long flags);

static struct backing_dev_info OsalDrv_backing_dev_info;
#endif



/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    OsalDriver_state
 *
 *  @brief  OsalDriver state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
OsalDriver_ModuleObject OsalDriver_state =
{
    .refCount = 0
};

/*!
 *  @brief  File operations table for Osal.
 */
static struct file_operations OsalDrv_driverOps = {
    open:    OsalDrv_open,
    release: OsalDrv_close,
    mmap:    OsalDrv_mmap,
    unlocked_ioctl:   OsalDrvDrv_ioctl,
#ifndef CONFIG_MMU
    /* dummy functions to satisfy no MMU module in kernel */
    read:              OsalDrv_read,
    get_unmapped_area: OsalDrv_unmapped_area,
#endif
} ;

/* Extern declaration to Global trace flag. */
extern Int curTrace;


/* =============================================================================
 * APIs
 * =============================================================================
 */

#ifndef CONFIG_MMU
/*!
 *  @brief      dummy read function to satisfy no mmu mmap handling
 *
 *  @sa         Osal driver read function
 */
static ssize_t
OsalDrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return 0;
}

/*!
 *  @brief      dummy unmapped_area function to satisfy no mmu mmap handling
 *
 *  @sa         Osal driver unmapped_area function
 */
static unsigned long OsalDrv_unmapped_area(struct file *filp,
                                                  unsigned long orig_addr,
                                                  unsigned long len,
                                                  unsigned long pgoff,
                                                  unsigned long flags)
{
        return (pgoff << PAGE_SHIFT);
}
#endif

/*!
 *  @brief      Initialize the Driver module.
 *
 *  @sa         OsalDriver_destroy
 */
Int32
OsalDriver_setup (Void)
{
    Int32          status = OSALDRIVER_SUCCESS;
    Int32          osStatus = 0 ;
    IArg           key;
    dev_t          dev;
    Error_Block    eb;
    Int            i;

    GT_0trace (curTrace, GT_ENTER, "OsalDriver_setup");
    Error_init (&eb);

    if (OsalDriver_state.refCount == 0) {
        /* Initialize the refCount */
        OsalDriver_state.refCount = 1;

        for (i = 0; i < NUM_CACHEMMAP_ENTRIES; i++) {
            OsalDriver_state.cachemmap[i].physAddr = 0;
        }

        OsalDriver_state.gateHandle = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (OsalDriver_state.gateHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to create the gate */
            status = OSALDRIVER_E_FAIL;
            GT_setFailureReason(curTrace,
                                GT_4CLASS,
                                "OsalDriver_setup",
                                status,
                                "Failed to create the gate!");
        }
        else {
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/
            OsalDriver_state._class = class_create (THIS_MODULE,
                                                    OSALDRIVER_CLASSNAME);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (OsalDriver_state._class == NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                      &OsalDriver_state.gateHandle);
                /*! @retval OSALDRIVER_E_FAIL Failed to create the driver
                                              class */
                status = OSALDRIVER_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OsalDriver_setup",
                                     status,
                                     "Failed to create the driver class!");
            }
            else {
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/
                osStatus = alloc_chrdev_region (&dev,
                                                0,
                                                MAX_OSAL_DRIVERS,
                                                OSALDRIVER_CLASSNAME);
                OsalDriver_state.major = MAJOR(dev);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (osStatus != 0) {
                    class_destroy (OsalDriver_state._class);
                    GateMutex_delete ((GateMutex_Handle *)
                                      &OsalDriver_state.gateHandle);
                    /*! @retval OSALDRIVER_E_FAIL Failed to create region of
                                                  devices */
                    status = OSALDRIVER_E_FAIL;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OsalDriver_setup",
                                         status,
                                         "Failed to create region of devices!");
                }
            }
        }
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/

        /* Register the OSAL driver */
        if (status >= 0) {
            OsalDriver_state.osalDrvHandle = OsalDriver_registerDriver ("Osal",
                                                             &OsalDrv_driverOps,
                                                         OSAL_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (OsalDriver_state.osalDrvHandle == NULL) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OsalDriver_setup",
                                     OSALDRIVER_E_INVALIDARG,
                                     "OsalDriver_registerDriver failed!");
                OsalDriver_destroy ();
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
    }
    else {
        key = IGateProvider_enter (OsalDriver_state.gateHandle);
        OsalDriver_state.refCount++;
        IGateProvider_leave (OsalDriver_state.gateHandle, key);
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalDriver_setup");

    /*! @retval OSALDRIVER_SUCCESS operation successful */
    return status;
}


/*!
 *  @brief      Finalize the Driver module.
 *
 *  @sa         OsalDriver_setup
 */
Int32
OsalDriver_destroy (Void)
{
    Int32       status = OSALDRIVER_SUCCESS;
    IArg        key;

    GT_0trace (curTrace, GT_ENTER, "OsalDriver_destroy");

    if (OsalDriver_state.refCount == 1) {
        /* Unregister the Osal Drviver */
        OsalDriver_unregisterDriver (
                         (OsalDriver_Handle *) &OsalDriver_state.osalDrvHandle);

        unregister_chrdev_region (MKDEV(OsalDriver_state.major, 0),
                                  MAX_OSAL_DRIVERS);
        class_destroy (OsalDriver_state._class);

        /* Delete the Gate handle */
        status = GateMutex_delete ((GateMutex_Handle *)
                                    &OsalDriver_state.gateHandle);
        GT_assert (curTrace, (status >= 0));

        OsalDriver_state.refCount = 0;
    }
    else {
        key = IGateProvider_enter (OsalDriver_state.gateHandle);
        OsalDriver_state.refCount--;
        IGateProvider_leave (OsalDriver_state.gateHandle, key);
    }

    GT_0trace (curTrace, GT_LEAVE, "OsalDriver_destroy");

    /*! @retval OSALDRIVER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      function to register a driver with the OS kernel
 *
 *  @param      name  Name of the driver.
 *  @param      fops  File operations table.
 *  @param      minor Minor numberof the driver.
 *
 *  @sa         OsalDriver_unregisterDriver
 */
OsalDriver_Handle
OsalDriver_registerDriver (String                   name,
                           struct file_operations * fops,
                           UInt32                   minor)
{
    struct OsalDriver_Object * object = NULL;
    IArg                       key;
    Int32                      osStatus;
    Int32                      devNo;

    Char                       tempStr [50];


    GT_3trace (curTrace,
               GT_ENTER,
               "OsalDriver_registerDriver",
               name,
               fops,
               minor);

    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (fops != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (OsalDriver_state.refCount == 0) {
        /*! @retval NULL   Module is in invalid state */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalDriver_registerDriver",
                             OSALDRIVER_E_INVALIDSTATE,
                             "Module is in invalid state!");
    }
    else if (name == NULL) {
        /*! @retval NULL   Passed name is null */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalDriver_registerDriver",
                             OSALDRIVER_E_INVALIDARG,
                             "Passed name is null!");
    }
    else if (fops == NULL) {
        /*! @retval NULL   Passed fops is null */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalDriver_registerDriver",
                             OSALDRIVER_E_INVALIDARG,
                             "Passed fops is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate a handle */
        object = Memory_alloc (NULL, sizeof (OsalDriver_Object), 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (object == NULL) {
            /*! @retval NULL   Failed to allocate memory for handle */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OsalDriver_registerDriver",
                                 OSALDRIVER_E_MEMORY,
                                 "Failed to allocate memory for handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            key = IGateProvider_enter (OsalDriver_state.gateHandle);
            cdev_init (&object->cdev, fops);
            object->cdev.owner = THIS_MODULE;
            object->cdev.ops   = fops;
            object->minor = minor;
            devNo = MKDEV(OsalDriver_state.major,
                          minor);
            osStatus = cdev_add (&object->cdev, devNo, 1);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (osStatus) {
                /*! @retval NULL   Failed to add the device */
                GT_setFailureReason(curTrace,
                                    GT_4CLASS,
                                    "OsalDriver_registerDriver",
                                    OSALDRIVER_E_FAIL,
                                    "Failed to add the device!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                String_cpy (tempStr, OSALDRIVER_CLASSNAME);
                String_cat (tempStr, "_");
                String_cat (tempStr, name);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,13)
                class_device_create (OsalDriver_state._class,
                               NULL,
                               devNo,
                               NULL,
                               tempStr);
#else
                device_create (OsalDriver_state._class,
                               NULL,
                               devNo,
                               NULL,
                               tempStr);
#endif
#ifndef CONFIG_MMU
                /* Initialize the backing_dev_info for Osal device */
                memcpy(&OsalDrv_backing_dev_info,
                       &default_backing_dev_info,
                       sizeof(struct backing_dev_info));
                OsalDrv_backing_dev_info.capabilities |= BDI_CAP_MAP_DIRECT |
                                    BDI_CAP_READ_MAP | BDI_CAP_WRITE_MAP;
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            IGateProvider_leave (OsalDriver_state.gateHandle, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE,"OsalDriver_registerDriver", object);

    /*! @retval NULL   Operation failed */
    /*! @retval Handle Operation was successful */
    return object;
}


/*!
 *  @brief      function to unregister a driver
 *
 *  @param      name  Name of the driver.
 *  @param      fops  File operations table.
 *  @param      minor Minor numberof the driver.

 *  @sa         OsalIOCTL_Create
 */
Int32
OsalDriver_unregisterDriver (OsalDriver_Handle * drvHandle)
{
    Int32                      status = OSALDRIVER_SUCCESS;
    struct OsalDriver_Object * object = NULL;
    /* IArg                       key; */

    GT_1trace (curTrace,
               GT_ENTER,
               "OsalDriver_unregisterDriver",
               drvHandle);

    GT_assert (curTrace, (drvHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (OsalDriver_state.refCount == 0) {
        /*! @retval OSALDRIVER_E_INVALIDSTATE Module is in invalid state */
        status = OSALDRIVER_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "OsalDriver_unregisterDriver",
                            status,
                            "Module is in invalid state!");
    }
    else if (drvHandle == NULL) {
        /*! @retval OSALDRIVER_E_INVALIDARG Passed drvHandle is null */
        status = OSALDRIVER_E_INVALIDARG;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "OsalDriver_unregisterDriver",
                            status,
                            "Passed drvHandle is null!");
    }
    else if (*drvHandle == NULL) {
        /*! @retval OSALDRIVER_E_INVALIDARG *drvHandle passed is NULL */
        status = OSALDRIVER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalDriver_unregisterDriver",
                             status,
                             "*drvHandle passed is NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        object = *drvHandle;
        /* Gate protection creates error msg because of udev in kernel */
        /* key = IGateProvider_enter (OsalDriver_state.gateHandle); */
        device_destroy (OsalDriver_state._class,
                        MKDEV(OsalDriver_state.major, object->minor));
        cdev_del (&object->cdev);
        /* IGateProvider_leave (OsalDriver_state.gateHandle, key); */
        Memory_free (NULL, object, sizeof (OsalDriver_Object));
        *drvHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*!defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE,"OsalDriver_unregisterDriver", status);

    return status;
}


/* Driver model for OSAL */

/*!
 *  @brief  Linux specific function to open the driver.
 *
 *  @param  inode   Inode pointer.
 *  @param  filp    File structure pointer.
 *
 *  @sa     ProcMgrDrv_release
 */
static
int
OsalDrv_open (struct inode * inode, struct file * filp)
{
#ifndef CONFIG_MMU
        if (filp && filp->f_mapping) {
            filp->f_mapping->backing_dev_info = &OsalDrv_backing_dev_info;
            filp->f_mode |= FMODE_WRITE;
        }
#endif
    return 0;
}


/*!
 *  @brief  Linux driver function to close the driver object.
 *
 *  @param  inode   Inode pointer.
 *  @param  filp    File structure pointer.
 *
 *  @sa     ProcMgrDrv_open
 */
static
int
OsalDrv_close (struct inode * inode, struct file * filp)

{
    return 0;
}


/*!
 *  @brief  Linux driver function to map memory regions to user space.
 *
 *  @param  filp    File structure pointer.
 *  @param  vma     User virtual memory area structure pointer.
 *
 *  @sa     OsalDrv_open
 */
static
int
OsalDrv_mmap (struct file * filp, struct vm_area_struct * vma)
{
#ifdef CONFIG_MMU
    UInt16 regionId;
    Bool   isCached = FALSE;
    ULong  physAddr;
    Int    i;

    /*
     * Set all regions as non-cached except for sharedRegion based on
     * cacheEnable setting
     */

    vma->vm_page_prot = pgprot_noncached (vma->vm_page_prot);
    physAddr = vma->vm_pgoff << PAGE_SHIFT;

    regionId = _SharedRegion_getIdPhys((Ptr)physAddr);
    if (regionId != SharedRegion_INVALIDREGIONID) {
        isCached = SharedRegion_isCacheEnabled(regionId);
    }
    else {
        for (i = 0; i < NUM_CACHEMMAP_ENTRIES; i++) {
            if (OsalDriver_state.cachemmap[i].physAddr == physAddr) {
                isCached = TRUE;
                OsalDriver_state.cachemmap[i].physAddr = 0;

                break;
            }
        }
    }

    GT_2trace(curTrace, GT_2CLASS,
               "OsalDrv_mmap(): setting cache %s for physical address %p",
               isCached ? "enabled" : "disabled", physAddr);

    if (_OsalDrv_isNormalMemory((UInt32) physAddr,
                                 vma->vm_end - vma->vm_start)) {
        /*
         * Map as Normal Memory (as opposed to Strongly Ordered)
         */
        if (isCached == TRUE) {
            vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) |
                L_PTE_MT_WRITEBACK);
        }
        else {
            /* Un-cached */
            vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) |
                L_PTE_MT_BUFFERABLE );
        }
    }
    else {
        /* Do not map as Normal Memory */
        if (isCached == TRUE) {
            vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) |
                (L_PTE_MT_WRITETHROUGH | L_PTE_MT_BUFFERABLE));
        }
    }
#endif

    if (remap_pfn_range (vma,
                         vma->vm_start,
                         vma->vm_pgoff,
                         vma->vm_end - vma->vm_start,
                         vma->vm_page_prot)) {
        return -EAGAIN;
    }
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long OsalDrvDrv_ioctl (struct file *  filp,
                       unsigned int   cmd,
                       unsigned long  args)
{
    int    osStatus = 0;
    int    rv;
    Int32  status = OSALDRIVER_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "OsalDrvDrv_ioctl",
               filp, cmd, args);

    switch (cmd) {
        case CMD_TRACEDRV_SETTRACE:
        {
            TraceDrv_ioctl ((UInt32) cmd, (Ptr) args);
        }
        break;

        case CMD_CACHE_INV:
        case CMD_CACHE_WB:
        case CMD_CACHE_WBINV:
        case CMD_CACHE_WAIT:
        case CMD_CACHE_SETMODE:
        {
            status = CacheDrv_ioctl ((UInt32) cmd, (Ptr) args);
        }
        break;

        case CMD_OSALDRV_CACHEMMAP:
        {
            OsalDrv_CmdArgs *cargs = (OsalDrv_CmdArgs *)args;
            OsalDrv_CmdArgs cachemmapArgs;
            Int index;

            rv = copy_from_user(&cachemmapArgs, cargs,
                                sizeof (OsalDrv_CmdArgs));
            if (rv != 0) {
                osStatus = -EFAULT;

                break;
            }

            index = 0;
            while (index < NUM_CACHEMMAP_ENTRIES &&
                   OsalDriver_state.cachemmap[index].physAddr != 0) {

                if (OsalDriver_state.cachemmap[index].physAddr ==
                    cachemmapArgs.physAddr) {

                    break;
                }
                index++;
            }

            if (index == NUM_CACHEMMAP_ENTRIES) {
                cachemmapArgs.apiStatus = OSALDRIVER_E_MEMORY;
            }
            else {
                GT_1trace(curTrace, GT_2CLASS,
                          "OsalDrvDrv_ioctl(): CMD_OSALDRV_CACHEMMAP - "
                          "tagging physAddr %p as cacheable for mmap()\n",
                          cachemmapArgs.physAddr);

                OsalDriver_state.cachemmap[index].physAddr =
                    cachemmapArgs.physAddr;

                cachemmapArgs.apiStatus = OSALDRIVER_SUCCESS;
            }

            rv = copy_to_user(cargs, &cachemmapArgs,
                              sizeof (OsalDrv_CmdArgs));
            if (rv != 0) {
                osStatus = -EFAULT;
            }
        }

        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = OSALDRIVER_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OsalDrvDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    GT_1trace (curTrace, GT_LEAVE, "OsalDrvDrv_ioctl", osStatus);

    /*! @retval 0 Operation successfully completed. */
    return osStatus;
}

/*!
 *  @brief      Checks if memory range should be mapped as Normal memory
 */
static Bool
_OsalDrv_isNormalMemory(UInt32 addr, UInt32 size) {
    Bool   ret = FALSE;
    UInt32 upperBound;
    Int    i;

    for (i = 0; i < ADDR_TABLE_SIZE; i++) {
        upperBound = addrTable[i].addr + addrTable[i].size - 1;
        /*
         * If the range to be mapped is entirely within a range to be mapped as
         * Normal, then do it. Otherwise be conservative and mapped as strongly
         * ordered.
         */
        if ((addr >= addrTable[i].addr) &&
            (addr + size - 1 <= upperBound)) {
            ret = TRUE;
            break;
        }
    }

    return (ret);
}
