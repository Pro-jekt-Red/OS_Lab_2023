/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

int table[32], status[32], use_time[32];
void ssd_init() {
    for (int i = 0; i < 32; i++) {
        table[i] = -1;
    }
}
int ssd_read(u_int logic_no, void *dst) {
    if (table[logic_no] == -1) {
        return -1;
    }
    ide_read(0, table[logic_no], dst, 1);
    return 0;
}
int find() {
    int min[2], min_no[2];
    min[0] = min[1] = 0x7fffffff;
    min_no[0] = min_no[1] = -1;
    for (int i = 0; i < 32; i++) {
        if (use_time[i] < min[status[i]]) {
            min[status[i]] = use_time[i], min_no[status[i]] = i;
        }
    }
    if (min[0] >= 5){
        char tmp[512];
        ide_read(0, min_no[1], tmp, 1);
        ide_write(0, min_no[0], tmp, 1);
        status[min_no[0]] = 1;
        for (int i = 0; i < 32; i++) {
            if (table[i] == min_no[1]) {
                ssd_erase(i);
                table[i] = min_no[0];
                break;
            }
        }
        return min_no[1];
    }
    return min_no[0];
}
void ssd_write(u_int logic_no, void *src) {
    if (table[logic_no] != -1) {
        ssd_erase(logic_no);
    }
    int pno = find();
    ide_write(0, pno, src, 1);
    status[pno] = 1;
    table[logic_no] = pno;
}
void ssd_erase(u_int logic_no) {
    static char empty[512] = {};
    if (table[logic_no] == -1) {
        return;
    }
    ide_write(0, table[logic_no], empty, 1);
    use_time[table[logic_no]]++;
    status[table[logic_no]] = 0;
    table[logic_no] = -1;
}

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
    u_int begin = secno * BY2SECT;
    u_int end = begin + nsecs * BY2SECT;

    for (u_int off = 0; begin + off < end; off += BY2SECT) {
        uint32_t temp = diskno;
        /* Exercise 5.3: Your code here. (1/2) */
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
        temp = begin + off;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
        temp = DEV_DISK_OPERATION_READ;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
        panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
        panic_on(!temp);
        panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
    }
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
    u_int begin = secno * BY2SECT;
    u_int end = begin + nsecs * BY2SECT;

    for (u_int off = 0; begin + off < end; off += BY2SECT) {
        uint32_t temp = diskno;
        /* Exercise 5.3: Your code here. (2/2) */
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
        temp = begin + off;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
        panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
        temp = DEV_DISK_OPERATION_WRITE;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
        panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
        panic_on(!temp);
    }
}
