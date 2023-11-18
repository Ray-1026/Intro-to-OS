/*
Student No.: 110550093
Student Name: 蔡師睿
Email: srt093.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <sys/mman.h>
#include <unistd.h>

struct block {
    size_t size;
    int free;
    struct block *prev;
    struct block *next;
} *first = NULL;

void *memory = NULL;

void *malloc(size_t size)
{
    // pre-allocate memory pool of 20k bytes on the first malloc
    if (memory == NULL) {
        memory = mmap(NULL, 20000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (memory == MAP_FAILED)
            return NULL;

        first = memory;
        first->size = 20000 - sizeof(struct block);
        first->free = 1;
        first->prev = NULL;
        first->next = NULL;
    }

    struct block *current = first;
    size = (size + 31) / 32 * 32; // align to 32 bytes

    if (size == 0) {
        size_t max = 0;
        while (current) {
            if (current->free && current->size > max)
                max = current->size;
            current = current->next;
        }

        // output max free chunk size
        char output[40] = "Max Free Chunk Size = 0\n";
        if (max) {
            int flag = 0, ptr = 22;
            for (int i = 10000; i; i /= 10) {
                if (!flag && max / i > 0)
                    flag = 1;
                if (flag) {
                    output[ptr++] = max / i + '0';
                    max %= i;
                }
            }
            output[ptr] = '\n';
        }
        write(1, output, sizeof(output) - 1);

        munmap(memory, 20000);
        return NULL;
    }

    // first fit
    while (current) {
        if (current->free && current->size >= size) {
            current->free = 0;
            if (current->size > size) {
                struct block *temp = current + size / sizeof(struct block) + 1;
                temp->size = current->size - size - sizeof(struct block);
                temp->free = 1;
                temp->next = current->next;
                if (current->next)
                    current->next->prev = temp;
                temp->prev = current;
                current->next = temp;
            }
            current->size = size;

            return current + 1;
        }
        current = current->next;
    }

    // no free block found
    return NULL;
}

void free(void *ptr)
{
    struct block *current = ptr - sizeof(struct block);
    current->free = 1;

    // merge with previous block
    if (current->prev && current->prev->free) {
        current->prev->size += current->size + sizeof(struct block);
        current->prev->next = current->next;
        if (current->next)
            current->next->prev = current->prev;
        current = current->prev;
    }

    // merge with next block
    if (current->next && current->next->free) {
        current->size += current->next->size + sizeof(struct block);
        current->next = current->next->next;
        if (current->next)
            current->next->prev = current;
    }
}