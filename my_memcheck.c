#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void (*real_free)(void*) = NULL;
static void* (*real_malloc)(size_t) = NULL;

struct node
{
        int free_count;
        void * malloc_address;
        struct node *next;
}*head, *tail;

void check_memory_leak ()
{
        struct node *temp;
        temp = head -> next;
        while(temp)
        {
                if(temp -> free_count == 0)
                {
                        fprintf(stderr, "Memory leak at address (%p)\n", temp -> malloc_address);
                }
                temp = temp -> next;
        }
}

bool check_double_free (void *p)
{
        struct node * temp;
        temp = head -> next;
        while(temp)
        {
                if(temp -> malloc_address == p)
                {
                        temp -> free_count++;
                        if(temp -> free_count == 1)
                        {
                                return 1;
                        }
                        else
                        {
                                return 0;
                        }
                }
                temp = temp -> next;
        }
        return 1;
}

void add_to_linked_list (void *p)
{
        struct node *temp;
        temp = (struct node *)real_malloc(sizeof(struct node));
        temp -> malloc_address = p;
        temp -> next = NULL;
        temp -> free_count = 0;
        tail -> next = temp;
        tail = temp;
        temp = head -> next;
        fprintf(stderr, "Current linked list:");
        while(temp)
        {
                fprintf(stderr, " -> %p", temp->malloc_address);
                temp = temp -> next;
        }
        fprintf(stderr, "\n");
}

void free_linked_list ()
{
        struct node * current, * prev;
        prev = head;
        current = head -> next;
        while (current)
        {
                real_free(prev);
                prev = current;
                current = current -> next;
        }
        real_free(prev);
}

void* malloc(size_t size)
{
        static int recursively_called = 0;
        void *p;
        int f;
        if (!recursively_called)
        {
                recursively_called=1;
                if (!real_malloc)
                        real_malloc = dlsym(RTLD_NEXT, "malloc");

                p = real_malloc(size);
                fprintf(stderr, "malloc(%d) = %p\n", size, p);
                add_to_linked_list(p);
                recursively_called=0;
        }
        else
        {
                p = real_malloc(size);
        }
        return p;
}

void free(void *p)
{
        static int recursively_called = 0;
        char backtracestr[4096];
        if (!recursively_called)
        {
                recursively_called=1;
                if (!real_free)
                        real_free = dlsym(RTLD_NEXT, "free");
                fprintf(stderr, "free(%p)\n", p);
                if(check_double_free(p))
                        real_free(p);
                else
                        fprintf(stderr, "Calling free for more than once for pointer (%p)\n", p);
                recursively_called=0;
        }
        else
        {
                real_free(p);
        }
}
void __attribute__ ((constructor)) premain()
{
        fprintf(stderr,"premain\n");
        head = (struct node *)calloc(1, sizeof(struct node));
        head -> next = NULL;
        head -> malloc_address = NULL;
        head -> free_count = 0;
        tail = head;
}
void __attribute__ ((destructor)) postmain()
{
        fprintf(stderr,"postmain\n");
        check_memory_leak();
        free_linked_list();
}