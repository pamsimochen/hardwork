#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x88d2b212, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xff178f6, "__aeabi_idivmod" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x6f5cfc06, "device_destroy" },
	{ 0x33ec6de7, "__register_chrdev" },
	{ 0xcd281907, "mutex_unlock" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xa4610bc6, "omap_rev" },
	{ 0xea147363, "printk" },
	{ 0x328a05f1, "strncpy" },
	{ 0x44bb71a6, "dma_free_coherent" },
	{ 0x45489617, "device_create" },
	{ 0x51c98907, "dma_alloc_coherent" },
	{ 0x8a1695cc, "mutex_lock_nested" },
	{ 0xd2311f3d, "PVRGetBufferClassJTable" },
	{ 0x37a0cba, "kfree" },
	{ 0xed7817b9, "remap_pfn_range" },
	{ 0x9d669763, "memcpy" },
	{ 0xc9104a44, "class_destroy" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x9db42785, "__class_create" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=pvrsrvkm";


MODULE_INFO(srcversion, "17026ADF448B26856326477");
