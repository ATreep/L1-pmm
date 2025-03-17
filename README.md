# L1 pmm：实现 kalloc/kfree

> 实验来自南京大学蒋炎炎的操作系统课实验 L1。实验手册：https://jyywiki.cn/OS/2024/labs/L1.md https://jyywiki.cn/OS/2023/labs/L1.html

因为没有办法使用OJ评测，无法确定此实验代码是否满足 Hard test 的要求。

不过此实验已在 `main.c` 的 `stress_test` 压力测试下正常运行，因此可以确定此实验至少满足了实验手册的 Safety 和 Liveness 要求。

注意：如需运行，请先确认修改根目录 Makefile 的 `CROSS_COMPILE` 环境变量。

经过无数的找 bug 与改 bug，此实验在三个日夜煎熬下终于完成，只能说编程经验还是有待提高，菜就多练  ,,Ծ‸Ծ,,