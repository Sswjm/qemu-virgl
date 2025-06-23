=======================
QEMU virgl optimization
=======================


Loongarch QEMU-VirtioGPU perfetto
=================================

将vperfetto工具集成到QEMU的virtio gpu中

Loongarch Kernel Interrupt
==========================

中断控制下移，vhost-user前置条件

Loongarch vhost-user-gpu
========================

fix: 

1. 花屏，考虑framebuffer问题
2. 启动grub后，guest drm无法初始化virtio gpu，考虑host侧配置问题？ 

