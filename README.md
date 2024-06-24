# buptLab-dns_relay_server

这个仓库包含了北京邮电大学 2023-2024 春季学期《计算机网络课程设计》项目——DNS 中继服务器的相关代码和报告（见 Release）。

## 文件结构

```
.
├── include                 # 头文件目录
│   ├── data_structure              # 数据结构头文件目录
│   │   ├── forward_list.h                  # 单向链表头文件
│   │   ├── list.h                          # 双向链表头文件
│   │   └── trie.h                          # 字典树头文件
│   ├── dns_relay.h                 # DNS 中继服务器头文件
│   ├── module                      # 各模块头文件目录
│   │   ├── cmd_interpreter.h               # 命令行参数解析组件头文件
│   │   ├── dns_cache.h                     # DNS 缓存组件头文件
│   │   ├── id_translation.h                # ID 转换组件头文件
│   │   ├── logger.h                        # 日志组件头文件
│   │   └── rule_table.h                    # 对照表解析组件头文件
│   └── network                     # 网络相关组件头文件目录
│       ├── dns_utility.h                   # DNS 工具函数头文件
│       └── ipv4_utility.h                  # IPv4 工具函数头文件
├── LICENSE
├── Makefile
├── README.md
├── src                     # 源文件目录
│   ├── data_structure              # 数据结构源文件目录
│   │   ├── forward_list.c                  # 单向链表源文件
│   │   ├── list.c                          # 双向链表源文件
│   │   └── trie.c                          # 字典树源文件
│   ├── dns_relay.c                 # DNS 中继服务器源文件
│   ├── module                      # 各种模块源文件目录
│   │   ├── cmd_interpreter.c               # 命令行参数解析组件源文件
│   │   ├── dns_cache.c                     # DNS 缓存组件源文件
│   │   ├── id_translation.c                # ID 转换组件源文件
│   │   ├── logger.c                        # 日志组件源文件
│   │   └── rule_table.c                    # 对照表解析组件源文件
│   └── network                     # 网络相关组件源文件目录
│       ├── dns_utility.c                   # DNS 工具函数源文件
│       └── ipv4_utility.c                  # IPv4 工具函数源文件
└── test                    # 测试文件目录
    ├── hosts.txt                   # 测试对照表
    └── testdata.txt                # 测试域名列表
```
