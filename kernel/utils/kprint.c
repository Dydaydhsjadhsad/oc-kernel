#include <arch/memory.h>
#include <arch/idt.h>
#include <utils/kprint.h>
#include <lib/math.h>
#include <lib/string.h>
#include <lib/stdtypes.h>

const char* video_base_ptr = (char*)VIDEO_MEMORY_ADDR;
char* video_ptr = (char*)VIDEO_MEMORY_ADDR;

/*
 * Api - Clear kernel screen
 */
extern void kclear() {
    video_ptr = video_base_ptr;

    for (size_t i = 0; i < SCREEN_SIZE; ++i) {
        *video_ptr++ = ' '; /* blank character */
        *video_ptr++ = VIDEO_MEMORY_ATTR; /* attribute-byte */
    }

    video_ptr = video_base_ptr;
}

/*
 * Api - Print kernel message
 */
extern void kprint(const char *str, ...) {
    va_list list;
    va_start(list, str);

    kvprint(str, list);
}

/*
 * Api - Print kernel message
 */
extern void kvprint(const char *str, va_list list) {
    char ch;
    u_int num;
    char str_num[8];

    asm_lock();

    while (*str != '\0') {
        if (*str != '\n' && *str != '%') {
            // usual character
            if (video_ptr - video_base_ptr + 2 >= SCREEN_SIZE) {
                kscroll(1); /* scroll line up */
            }

            *video_ptr++ = *str++; /* the character's ascii */
            *video_ptr++ = VIDEO_MEMORY_ATTR; /* attribute-byte */
        } else if (*str == '%') {
            // control character
            switch (*++str) {
                case 'c':
                    if (video_ptr - video_base_ptr + 2 >= SCREEN_SIZE) {
                        kscroll(1); /* scroll line up */
                    }

                    ch = va_arg(list, char);
                    *video_ptr++ = ch;
                    *video_ptr++ = VIDEO_MEMORY_ATTR;
                    break;
                case 'u':
                    num = va_arg(list, u_int);
                    itoa(num, str_num, 10);

                    if (video_ptr - video_base_ptr + strlen(str_num) >= SCREEN_SIZE) {
                        kscroll(1); /* scroll line up */
                    }

                    video_ptr = strext(video_ptr, str_num, VIDEO_MEMORY_ATTR);
                    break;
                case 'X':
                    num = va_arg(list, u_int);
                    itoa(num, str_num, 16);

                    if (video_ptr - video_base_ptr + strlen(str_num) >= SCREEN_SIZE) {
                        kscroll(1); /* scroll line up */
                    }

                    video_ptr = strext(video_ptr, str_num, VIDEO_MEMORY_ATTR);
                    break;
            }
        } else if (*str == '\n') {
            // new line character
            size_t offset = (video_ptr - video_base_ptr) % SCREEN_WIDTH;

            if (video_ptr - video_base_ptr + offset >= SCREEN_SIZE) {
                kscroll(1); /* scroll line up */
            }

            video_ptr += (SCREEN_WIDTH - offset);
        }
    }

    va_end(list);

    asm_unlock();
}

/*
 * Api - Scroll console up
 */
extern void kscroll(u_int n) {
    char *ptr = video_base_ptr;
    for (int i = 1; i < SCREEN_HEIGHT * 2; ++i) {
        for (int j = 0; j < SCREEN_WIDTH * 2; ++j) {
            ptr[(i - 1) * SCREEN_WIDTH * 2 + j] = ptr[i * SCREEN_WIDTH * 2 + j];
        }
    }

    for (int j = 0; j < SCREEN_WIDTH; ++j) {
        ptr[(SCREEN_HEIGHT * 2 - 1) + j * 2] = ' ';
        ptr[(SCREEN_HEIGHT * 2 - 1) + j * 2 + 1] = VIDEO_MEMORY_ATTR;
    }

    video_ptr -= SCREEN_WIDTH * 2;
}
