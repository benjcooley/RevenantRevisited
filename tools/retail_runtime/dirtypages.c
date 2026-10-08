/* Native dirty-page tracking for the retail runtime (runtime.py).
 *
 * A UC_HOOK_MEM_WRITE callback that keeps, for each guest page written
 * since the checkpoint, the page's content before its first write. The
 * Python write hook did the same with one Python call per guest store;
 * here a store to an already-saved page costs a table lookup.
 *
 * Built on first use by dirtypages.py (cc -O2 -shared); no Unicorn headers
 * or link: the host passes uc_mem_read's address.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PAGE 4096u
#define TOP 1024u                  /* 32-bit guest: 1024 x 1024 pages */

typedef int (*mem_read_fn)(void *uc, uint64_t address, void *bytes, uint64_t size);

typedef struct
{
    mem_read_fn mem_read;
    int active;                    /* 0: writes are not tracked */
    uint8_t **saved[TOP];          /* page -> its content before the first write */
    uint32_t *pages;               /* the saved pages, in first-write order */
    uint32_t count, capacity;
    uint32_t live;                 /* saved and not forgotten */
    int failed;                    /* a page could not be saved (out of memory / unreadable) */
} tracker;

tracker *dp_create(mem_read_fn mem_read)
{
    tracker *t = calloc(1, sizeof(tracker));
    if (t)
        t->mem_read = mem_read;
    return t;
}

static uint8_t **slot(tracker *t, uint32_t page, int create)
{
    uint32_t hi = page >> 22, lo = (page >> 12) & (TOP - 1);
    if (!t->saved[hi])
    {
        if (!create)
            return NULL;
        t->saved[hi] = calloc(TOP, sizeof(uint8_t *));
        if (!t->saved[hi])
            return NULL;
    }
    return &t->saved[hi][lo];
}

/* Save `page` (page-aligned) unless it already is. `uc` reads its content. */
static void save(tracker *t, void *uc, uint32_t page)
{
    uint8_t **s = slot(t, page, 1);
    if (!s)
    {
        t->failed = 1;
        return;
    }
    if (*s)
        return;
    uint8_t *copy = malloc(PAGE);
    if (!copy)
    {
        t->failed = 1;
        return;
    }
    if (t->mem_read(uc, page, copy, PAGE) != 0)
    {
        free(copy);                /* unmapped: the write itself faults, nothing to keep */
        return;
    }
    if (t->count == t->capacity)
    {
        uint32_t capacity = t->capacity ? t->capacity * 2 : 1024;
        uint32_t *pages = realloc(t->pages, capacity * sizeof(uint32_t));
        if (!pages)
        {
            free(copy);
            t->failed = 1;
            return;
        }
        t->pages = pages;
        t->capacity = capacity;
    }
    *s = copy;
    t->pages[t->count++] = page;
    t->live++;
}

/* uc_cb_hookmem_t */
void dp_hook(void *uc, int type, uint64_t address, int size, int64_t value, void *user)
{
    tracker *t = user;
    (void)type;
    (void)value;
    if (!t->active || size <= 0)
        return;
    uint32_t first = (uint32_t)address & ~(PAGE - 1);
    uint32_t last = (uint32_t)(address + (uint64_t)size - 1) & ~(PAGE - 1);
    save(t, uc, first);
    if (last != first)
        save(t, uc, last);
}

/* A host write about to happen to [address, address+size). */
void dp_touch(tracker *t, void *uc, uint32_t address, uint32_t size)
{
    if (!t->active || !size)
        return;
    uint64_t end = (uint64_t)address + size - 1;
    for (uint64_t page = address & ~(PAGE - 1); page <= end; page += PAGE)
        save(t, uc, (uint32_t)page);
}

void dp_set_active(tracker *t, int active) { t->active = active; }
uint32_t dp_count(tracker *t) { return t->count; }
uint32_t dp_live(tracker *t) { return t->live; }
int dp_failed(tracker *t) { return t->failed; }

/* The saved pages, in first-write order; a forgotten page is 0xffffffff. */
const uint32_t *dp_pages(tracker *t) { return t->pages; }

/* The saved content of `page`, or NULL. */
const uint8_t *dp_saved(tracker *t, uint32_t page)
{
    uint8_t **s = slot(t, page, 0);
    return s ? *s : NULL;
}

/* Drop `page` (its mapping went away). */
void dp_forget(tracker *t, uint32_t page)
{
    uint8_t **s = slot(t, page, 0);
    if (!s || !*s)
        return;
    free(*s);
    *s = NULL;
    t->live--;
    for (uint32_t i = 0; i < t->count; ++i)
        if (t->pages[i] == page)
            t->pages[i] = 0xffffffffu;
}

/* Drop every saved page (after a restore or a checkpoint). */
void dp_clear(tracker *t)
{
    for (uint32_t i = 0; i < t->count; ++i)
        if (t->pages[i] != 0xffffffffu)
        {
            uint8_t **s = slot(t, t->pages[i], 0);
            free(*s);
            *s = NULL;
        }
    t->count = 0;
    t->live = 0;
    t->failed = 0;
}

void dp_destroy(tracker *t)
{
    if (!t)
        return;
    dp_clear(t);
    for (uint32_t i = 0; i < TOP; ++i)
        free(t->saved[i]);
    free(t->pages);
    free(t);
}
