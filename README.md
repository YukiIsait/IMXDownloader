# IMX6U Downloader

## 简介

本软件是针对 NXP 的 IMX6U 系列芯片的 Windows 平台裸机烧写工具，用于将 Bin 文件烧写到 SD 卡中，会自动添加 IVT、DCD 等信息。

> 本软件的实现参考了正点原子的 Linux 平台 IMXDownload 工具。

## IVT 和 DCD 表

不同的开发板其 IVT 和 DCD 表的数据是不同的，因此需要获取所使用的开发板的 IVT 和 DCD 表信息，最简单的方法就是读取开发板配套资料里面的 `u-boot.imx` 的前 1KB 数据，理论上应该读取 3KB 的数据，但是表信息远远没有 3KB 这么多，因此读取 1KB 即可。

## 使用方法

1. 编译好原始的二进制bin文件，如 `u-boot.bin` 等，并将编译好的 `.bin` 文件和本软件放置到同一个目录下。
2. **以管理员身份**执行命令 `imxdownload <soucre_bin> <sd_device>`
    - 如烧写 `u-boot.bin` 到 `\\.\PhysicalDrive2` 中即可使用如下所示命令:

        CMD
        ```text
        imxdownload u-boot.bin \\.\PhysicalDrive2
        ```
        PowerShell
        ```text
        .\imxdownload u-boot.bin \\.\PhysicalDrive2
        ```

> 也可以将本软件加入到环境变量中，这样就可以在任意目录下使用 `imxdownload` 命令。

> 注意：本软件需要以管理员身份运行，否则无法打开磁盘设备。

## 附录

### 可以使用 WinObj 查看磁盘设备名称

WinObj 是一个程序，它使用本机 Windows API (由 NTDLL.DLL 提供) 来访问和显示 NT 对象管理器命名空间上的信息。

[WinObj - Sysinternals | Microsoft Learn](https://learn.microsoft.com/zh-cn/sysinternals/downloads/winobj)
