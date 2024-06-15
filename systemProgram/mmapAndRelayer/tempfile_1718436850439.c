#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <string.h>

int main() {
    int src_fd, dst_fd;
    struct iovec iov_in[2], iov_out[1];
    ssize_t bytes_read, bytes_written;

    // 打开源文件和目标文件（示例）
    src_fd = open("source.txt", O_RDONLY);
    dst_fd = open("destination.txt", O_WRONLY | O_CREAT, 0644);

    if (src_fd == -1 || dst_fd == -1) {
        perror("Opening files failed");
        return EXIT_FAILURE;
    }

    // 设置输入缓冲区
    iov_in[0].iov_base = malloc(50); // 分配内存给第一个缓冲区
    iov_in[0].iov_len = 50;
    iov_in[1].iov_base = malloc(50); // 分配内存给第二个缓冲区
    iov_in[1].iov_len = 50;

    // 读取操作
    bytes_read = readv(src_fd, iov_in, 2);
    if (bytes_read == -1) {
        perror("readv error");
        return EXIT_FAILURE;
    }

    // 设置输出缓冲区，合并两个输入缓冲区
    iov_out[0].iov_base = malloc(bytes_read); // 分配足够的内存来容纳读取的所有数据
    iov_out[0].iov_len = bytes_read;
    memcpy(iov_out[0].iov_base, iov_in[0].iov_base, iov_in[0].iov_len);
    memcpy((char*)iov_out[0].iov_base + iov_in[0].iov_len, iov_in[1].iov_base, iov_in[1].iov_len);

    // 写入操作
    bytes_written = writev(dst_fd, iov_out, 1);
    if (bytes_written == -1) {
        perror("writev error");
        return EXIT_FAILURE;
    }

    // 清理工作
    free(iov_in[0].iov_base);
    free(iov_in[1].iov_base);
    free(iov_out[0].iov_base);
    close(src_fd);
    close(dst_fd);

    return 0;
}
