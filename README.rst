# QEMU virgl optimization


## Virgl QEMU侧优化branch

- 命令转发零拷贝
- 实时异步反馈

### 命令转发零拷贝

目标: 减少qemu侧virtio-gpu命令转发的内存拷贝次数

