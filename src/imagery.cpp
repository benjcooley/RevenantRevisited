// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      object.cpp - TObject module                      *
// *************************************************************************

#include "imagery.h"

#include "bitmap.h"
#include "display.h"
#include "graphics.h"
#include "logging.h"
#include "object.h"
#include "resource.h"

#include <sys/types.h>
#include <sys/stat.h>

// Builder array for imagery builder class
int32_t TImageryBuilder::numimagerytypes = 0;
PTImageryBuilder TImageryBuilder::builders[MAXIMAGERYTYPES];

// Imagery loading status values
#define QE_NONE      0
#define QE_QUEUED    1
#define QE_LOADING   2
#define QE_LOADED    3
#define QE_FAILED    4

extern uint32_t ImageryMemUsage;

static TVirtualArray <SImageryEntry> EntryArray;

static char imagerypath[FILENAMELEN];

static bool ContainsIgnoreCase(const char* haystack, const char* needle)
{
    if (!haystack || !needle || !*needle)
        return false;
    const size_t nlen = std::strlen(needle);
    for (const char* p = haystack; *p; ++p)
    {
        size_t i = 0;
        while (i < nlen && p[i] &&
               std::tolower((unsigned char)p[i]) == std::tolower((unsigned char)needle[i]))
            ++i;
        if (i == nlen)
            return true;
    }
    return false;
}

static bool IsWaterDebugImagery(const char* name)
{
    if (!name || !*name)
        return false;
    return ContainsIgnoreCase(name, "water.i3d") ||
           ContainsIgnoreCase(name, "stillwater.i3d") ||
           ContainsIgnoreCase(name, "flowwater.i3d") ||
           ContainsIgnoreCase(name, "bendwater1.i3d") ||
           ContainsIgnoreCase(name, "bendwater2.i3d") ||
           ContainsIgnoreCase(name, "sewerw.i3d") ||
           ContainsIgnoreCase(name, "wave.i3d") ||
           ContainsIgnoreCase(name, "waves.i3d") ||
           ContainsIgnoreCase(name, "wavem.i3d") ||
           ContainsIgnoreCase(name, "wfall.i3d") ||
           ContainsIgnoreCase(name, "wfall2.i3d") ||
           ContainsIgnoreCase(name, "wcap.i3d") ||
           ContainsIgnoreCase(name, "wcap2.i3d") ||
           ContainsIgnoreCase(name, "riverfall.i3d");
}

//bool LoaderWait = false;

// *******************
// * TObjectAnimator *
// *******************

TObjectAnimator::TObjectAnimator(TObjectInstance* oi)
{
    inst = oi;
    image = oi->GetImagery();

  // These values are copied from 'inst' each frame 
  // (frame/framerate are copied back after ani)
    prevstate = state = -1;
    frame = 0;      
    framerate = 1;

    newstate = true;
    complete = false;
}

TObjectAnimator::~TObjectAnimator()
{
    Close();
}

void TObjectAnimator::ResetState()
{
    state = inst->GetState();
    prevstate = inst->GetPrevState();
    frame = inst->GetFrame();
    framerate = inst->GetFrameRate();

    newstate = true;
    complete = false;
}

void TObjectAnimator::Animate(bool draw)
{
    state = inst->GetState();
    prevstate = inst->GetPrevState();
    frame = inst->GetFrame();
    framerate = inst->GetFrameRate();
}

// *******************
// * TImageryBuilder *
// *******************

TImageryBuilder::TImageryBuilder(int32_t newid)
{
    if (newid >= numimagerytypes)
        numimagerytypes = newid + 1;

    builders[newid] = this;
    imageryid = newid;
}

// ******************
// * TObjectImagery *
// ******************

TObjectImagery::TObjectImagery(int32_t id)
{
    imageryid = id;
    entry = &EntryArray[id];

    entry->headerdirty = false;
    entry->bodydirty = false;

    if (!entry->body)
        LoadBody(false); // Queue imagery load
}

TObjectImagery::~TObjectImagery()
{
    FreeBody();
}

int32_t TObjectImagery::RegisterImagery(char *filename, SImageryHeader* header, uint32_t headersize)
{
    for (int32_t c = 0; c < EntryArray.NumItems(); c++)
    {
        if (EntryArray.Used(c) && !stricmp(EntryArray[c].filename, filename))
        {
            if (header)
                free(header);
            return c;
        }
    }

    if (!header)
    {
        char buf[FILENAMELEN];
        strcpy(buf, imagerypath);
        strcat(buf, filename);
        header   = (SImageryHeader*)LoadResourceHeader(buf, -1, (uint32_t *)&headersize);
    }

    if (!header)
    {
        Status("WARNING: Unable to load imagery header for %s", filename);
        return -1;
    }

    SImageryEntry ie;
    memset(&ie, 0, sizeof(SImageryEntry));
    strncpy(ie.filename, filename, MAXIMFNAMELEN - 1);

    ie.status     = QE_NONE;
    ie.header     = header;
    ie.headersize = headersize;
    ie.body       = nullptr;
    ie.usecount   = 0;
    ie.imagery    = nullptr;

    ie.headerdirty = false;
    ie.bodydirty = false;

//  VirtualLock(ie.header, ie.headersize);

#ifdef CHECK_IMAGERY_HEADER_VALIDITY

// This stuff checks the header validity of the imagery by checking the size
// of the bounding box compared to the walkmap.  To turn checking off, comment
// out the define above.  To turn on auto-fix mode (rebuilds any corrupted headers
// found, killing whatever walkmaps they might have had) comment the three FatalError()
// calls out, below, and change the #if 0 to #if 1.

    int32_t size = sizeof(SImageryHeader);

    bool fixit = false;

    for (int32_t i = 0; i < ie.header->numstates; i++)
    {
        if (i > 0)
            size += sizeof(SImageryStateHeader);

        size += ie.header->states[i].wwidth * ie.header->states[i].wlength;
        if (ie.header->states[i].wwidth < 1 || ie.header->states[i].wlength < 1)
        {
            if (ie.header->states[i].walkmap.ptr() != nullptr)
            {
                FatalError("State has walkmap but no bounding box");
                fixit = true;
            }
        }
        else
        {
            if (ie.header->states[i].walkmap.ptr() == nullptr)
            {
                FatalError("State has bounding box but no walkmap");
                fixit = true;
            }
        }
    }

    size = (size + 3) & 0xFFFFFFFCL;         // Round to even 4 bytes

    if ((fixit || size != ie.headersize) && !strstr(ie.filename, "i3d"))
    {
        FatalError("'Holy shitballs, Batman!  The imagery is fucked up again!  Curse that infernal Joker!'");
#if 0
        uint8_t *newbuf = (uint8_t *)malloc(size);
        int32_t skipdist = sizeof(SImageryHeader) + (sizeof(SImageryStateHeader) * (ie.header->numstates - 1));
        uint8_t *ptr = newbuf + skipdist;

        SImageryHeader* head = (SImageryHeader*)newbuf;
        memcpy(newbuf, ie.header, sizeof(SImageryHeader));

        for (i = 0; i < ie.header->numstates; i++)
        {
            PSImageryStateHeader sh = &(head->states[i]);

            memcpy(sh, &ie.header->states[i], sizeof(SImageryStateHeader));

            int32_t w = ie.header->states[i].wwidth;
            int32_t l = ie.header->states[i].wlength;

            if (w && l)
            {
                sh->walkmap.set(ptr);
                memset(ptr, 0, w * l);
                ptr += w * l;
            }
            else
                sh->walkmap.set(nullptr);
        }

        ie.header = (SImageryHeader*)newbuf;
        ie.headersize = size;
        ie.headerdirty = true;
#endif
    }
#endif

    int32_t id = EntryArray.Add(ie);

    if (id < 0)
        FatalError("Not enough room for imagery in imagery entry array!");

    return id;
}

SImageryEntry* TObjectImagery::GetImageryEntry(int32_t id)
{
    return &(EntryArray[id]);
}

int32_t TObjectImagery::NumEntries()
{
    return EntryArray.NumItems();
}

bool TObjectImagery::IsUsed(int32_t id)
{
    return EntryArray.Used(id);
}

void TObjectImagery::FreeImageryEntry(int32_t imageryentry)
{
    if (EntryArray[imageryentry].header)
        free(EntryArray[imageryentry].header);

    if (EntryArray[imageryentry].body)
        free(EntryArray[imageryentry].body);

    memset(&EntryArray[imageryentry], 0, sizeof(SImageryEntry));
}

void TObjectImagery::FreeAllImagery()
{
    if (Editor)
        SaveAllHeaders();

    for (int32_t c = 0; c < EntryArray.NumItems(); c++)
        FreeImageryEntry(c);
}

void TObjectImagery::ReloadImagery()
{
    BEGIN_CRITICAL();

    for (int32_t loop = 0; loop < EntryArray.NumItems(); loop++)
    {
        if (EntryArray.Used(loop))
        {
            EntryArray[loop].status  = QE_NONE;
            EntryArray[loop].imagery = nullptr;
            EntryArray[loop].body    = nullptr;
            EntryArray[loop].ressize = 0;
        }
    }

    END_CRITICAL();
}

void TObjectImagery::SetImageryPath(char *path)
{
    if (path)
        strcpy(imagerypath, path);
}

char *TObjectImagery::GetImageryPath()
{
    return imagerypath;
}

bool TObjectImagery::RenameImageryFile(int32_t imageryid, const char *newfile)
{
    if ((imageryid < 0) || (EntryArray.Used(imageryid) == false) ||
        (imageryid >= EntryArray.NumItems()) || (strlen(newfile) >= MAXIMFNAMELEN))
        return false;

    strcpy(EntryArray[imageryid].filename, newfile);
    EntryArray[imageryid].headerdirty = true;

    return true;
}

void TObjectImagery::SaveAllHeaders()
{
    for (int32_t c = 0; c < EntryArray.NumItems(); c++)
        SaveHeader(c);

    QuickSaveHeaders();  // Save a single file with all headers in it
}

void TObjectImagery::SetEntryReg(int32_t imageryid, int32_t state, int32_t regx, int32_t regy, int32_t regz)
{
    EntryArray[imageryid].header->states[state].regx = regx;
    EntryArray[imageryid].header->states[state].regy = regy;
    EntryArray[imageryid].header->states[state].regz = regz;
    EntryArray[imageryid].headerdirty = true;
}

int32_t TObjectImagery::FindImagery(const char *imageryname)
{
    for (int32_t c = 0; c < EntryArray.NumItems(); c++)
    {
        if (!EntryArray.Used(c))
            continue;

        RSImageryEntry ie = EntryArray[c];

        if (!stricmp(ie.filename, imageryname))
            return c;
    }

    return -1; 
}

TObjectImagery* TObjectImagery::LoadImagery(int32_t imgid)
{
    if (imgid < 0 || EntryArray.Used(imgid) == false ||
        imgid >= EntryArray.NumItems())
        return nullptr;

    RSImageryEntry ie = EntryArray[imgid];
    const bool debug_water = IsWaterDebugImagery(ie.filename);

    if (ie.imagery)
    {
        ie.usecount++;
        if (debug_water)
            log_info("[waterdbg] reuse imagery id=%d file='%s' type=%d usecount=%d",
                     imgid, ie.filename, ie.header ? int(ie.header->imageryid) : -1, ie.usecount);
    }

    else
    {
        PTImageryBuilder imbuilder = TImageryBuilder::GetBuilder(ie.header->imageryid);
        if (!imbuilder)
            return nullptr;

        if (debug_water)
            log_info("[waterdbg] build imagery id=%d file='%s' imageryid=%d",
                     imgid, ie.filename, ie.header ? int(ie.header->imageryid) : -1);

        ie.imagery = imbuilder->Build(imgid);
        if (!ie.imagery)
            return nullptr;

        ie.usecount = 1;
        if (debug_water)
            log_info("[waterdbg] built imagery id=%d file='%s' obj=%p",
                     imgid, ie.filename, (void*)ie.imagery);
    }

    ie.imagery->imageryid = imgid;

    return ie.imagery;
}

void TObjectImagery::FreeImagery(TObjectImagery* imagery)
{
    if (!imagery || (uint32_t)imagery->imageryid >= (uint32_t)EntryArray.NumItems())
        return;

    if (!EntryArray.Used(imagery->imageryid))
        return;

    RSImageryEntry ie = EntryArray[imagery->imageryid];

    ie.usecount--;
    if (ie.usecount <= 0)
    {
        imagery->FreeBody();

        if (Editor && ie.imagery)
        {
            // Legacy editor persisted modified imagery headers back to
            // disk on free (load-time endian/pointer fixups marked the
            // header dirty). Modern editor handles serialization
            // separately, and Mac asset paths aren't writable from the
            // running app -- the SaveHeader fopen would FATAL on Cut /
            // Delete when the freed instance was the imagery's last
            // user. Skip the write; just release the in-memory record.
            ImageryMemUsage -= ie.imagery->GetResSize();

            delete ie.imagery;
            ie.imagery = nullptr;
        }

        ie.usecount = 0;
    }
}

// Restores all lost imagery surfaces, etc.
void TObjectImagery::RestoreAll()
{
    for (int32_t c = 0; c < EntryArray.NumItems(); c++)
    {
        RSImageryEntry ie = EntryArray[c];
        if (ie.imagery != nullptr)
            ie.imagery->Restore();
    }
}

int32_t TObjectImagery::FindState(const char *name, int32_t pcnt) const
{
    if (!name)
        return -1;

    int32_t lowest = 100000; // Lowest frequency value
    int32_t found = -1;      // State with closest percent

    for (int32_t loop = 0; loop < GetHeader()->numstates; loop++)
    {
        char *n = GetHeader()->states[loop].animname;
        char *pcntchar;
        int32_t freq;

        if (n[0] <= '9' && n[0] >= '0' &&           // Starts with a number AND
            (pcntchar = strchr(n, ':')) != nullptr)    // Number is a frequency percentage
        {
            freq = atoi(n);
            n = pcntchar + 1;
        }
        else
            pcntchar = nullptr;

        if (!stricmp(n, name))
        {
          // Found what we're looking for, and it doesn't have a frequency prefix.. DONE!
            if (!pcntchar)
            {
                found = loop;
                break;
            }
        
          // We have a frequency prefix.. do we have a random percentage value?
            if (pcnt < 0)
                pcnt = random(1, 100);  // No.. then get one!

          // Check to see if frequency is closest to, but still >= percentage value
            if (freq >= pcnt && freq < lowest)
            {
                lowest = freq;
                found = loop;
            }
        }
    }

    return found;
}

// This function checks string against statename allowing for the "or" seperator.
// For example, "one" would match up against "one or two or three".
bool StateMatch(const char *string, const char *statename)
{
    if (!string || !statename)
        return false;

    char buf[80];

    const char *ptr = statename;
    do
    {
        const char *sep = strstr(ptr, " or ");
        if (sep == nullptr)
        {
            if (stricmp(string, ptr) == 0)
                return true;

            break;
        }
        else
        {
            int32_t len = (int32_t)(sep - ptr);
            memcpy(buf, ptr, len);
            buf[len] = 0;
        }

        if (stricmp(string, buf) == 0)
            return true;

        ptr = sep + 4;          // skip past the "or"
    } while (ptr);

    return false;
}

// Finds a transition state, such as "stand to walk" if you passed in "stand" and "walk".
// Note that "stand or walk to walk", "stand to walk or run", and "stand or turn to walk" would also work.
int32_t TObjectImagery::FindTransitionState(const char *from, const char *to, int32_t pcnt) const
{
    if (!from || !to)
        return -1;

    bool samestate = (stricmp(from, to) == 0);

    char buf[80];

    int32_t lowest = 100000; // Lowest frequency value
    int32_t found  = -1;     // State with closest percent
    int32_t highest = -1;

    for (int32_t loop = 0; loop < GetHeader()->numstates; loop++)
    {
        char *left = GetHeader()->states[loop].animname;
        char *pcntchar;
        int32_t freq;

      // Check to see if state has frequency prefix
        if (left[0] <= '9' && left[0] >= '0' &&     // Starts with a number AND
            (pcntchar = strchr(left, ':')) != nullptr) // Number is a frequency percentage
        {
            freq = atoi(left);
            left = pcntchar + 1;
        }
        else
            pcntchar = nullptr;

        bool any_state;
        char *right;
        
        if (strncmp(left, "to ", 3) == 0)
        {
            any_state = true;
            right = left + 3;
        }
        else
        {
            any_state = false;
          // Find the " to " in the name if it has one
            right = strstr(left, " to ");

           // If has " to ", get separate right/left strings
            if (right)
            {
                int32_t len = (int32_t)(right - left);
                memcpy(buf, left, len);
                buf[len] = 0;
                left = buf;
                right += 4;     // skip the "to"
            }
        }

      // State names match?
        if ((StateMatch(from, left) || any_state) &&                  // Matches left AND...
            ((right == nullptr && samestate) || StateMatch(to, right)))  // no right, or matches right
        {
          // Found what we're looking for, and it doesn't have a frequency prefix.. DONE!
            if (!pcntchar)
            {
                found = loop;
                break;
            }
        
          // We have a frequency prefix.. do we have a random percentage value?
            if (pcnt < 0)
                pcnt = random(1, 100);  // No.. then get one!

          // Check to see if frequency is closest to, but still >= percentage value
            if (freq >= pcnt && freq < lowest)
            {
                lowest = freq;
                found = loop;
            }
        }
    }

    return found;
}

int32_t TObjectImagery::GetUseCount()
{
    return entry->usecount;
}

extern bool UpdatingBoundingRect;

// Returns screen update area and 'onscreen' intersection rectangle given data from state
void TObjectImagery::GetScreenRect(TObjectInstance* oi, SRect &r) const
{
    if ((uint32_t)oi->GetState() >= (uint32_t)NumStates())
    {
        r.left = r.right = r.top = r.bottom = 0;
        return;
    }

    int32_t x, y;
    oi->GetScreenPos(x, y);

    PSImageryStateHeader st = GetState(oi->GetState());

    if (!UpdatingBoundingRect && (st->width == 0 || st->height == 0)) // Set some kind of initial value here
    {
        st->regx = 16;
        st->regy = 32;
        st->width = 32;
        st->height = 32;
    }

    r.left   = x - st->regx;
    r.right  = r.left + st->width - 1;
    r.top    = y - st->regy;
    r.bottom = r.top + st->height - 1;
}

void TObjectImagery::GetAnimRect(TObjectInstance* oi, SRect &r) const
{
    PSImageryStateHeader st = GetState(oi->GetState());

    if (!st || (st->animregx == 0 && st->animregy == 0))
    {
        r.left = r.right = r.top = r.bottom = 0;
        return;
    }

    int32_t x, y;
    oi->GetScreenPos(x, y);

    r.left   = x + st->animregx;
    r.right  = r.left + st->width - 1;
    r.top    = y + st->animregy;
    r.bottom = r.top + st->height - 1;
}

void TObjectImagery::ResetScreenRect(TObjectInstance* oi, int32_t state, bool frontonly)
{
    if (state < 0)
    {
        for (int32_t i = 0; i < NumStates(); i++)
            ResetScreenRect(oi, i);
    }
    else if ((uint32_t)state < (uint32_t)NumStates())
    {
        PSImageryStateHeader st = GetState(state);
        st->regx = st->regy = 0;
        st->width = st->height = 0;

        SetHeaderDirty(true);
    }
}

void TObjectImagery::SaveHeader(int32_t imgid)
{
    if (imgid < 0 || EntryArray.Used(imgid) == false ||
        imgid >= EntryArray.NumItems())
         FatalError("Invalid id when saving header");

    RSImageryEntry ie = EntryArray[imgid];

    if (ie.headerdirty)
    {
        char buf[120];
        strcpy(buf, imagerypath);
        strcat(buf, ie.filename);
        SaveResourceHeader(buf, ie.header, ie.headersize);

        ie.headerdirty = false;
    }
}

void TObjectImagery::SetWorldBoundBox(int32_t state, int32_t width, int32_t length, int32_t height)
{
    if ((uint32_t)state >= (uint32_t)entry->header->numstates)
        return;

    if (width != entry->header->states[state].wwidth ||
        length != entry->header->states[state].wlength)
    {
        // reallocating the walkmap requires changing the structure size;
        // therefore, must start over with a new buffer and copy over
        // all the old data
        int32_t size = sizeof(SImageryHeader);

        int32_t i;
        for (i = 0; i < entry->header->numstates; i++)
        {
            if (i > 0)
                size += sizeof(SImageryStateHeader);

            if (i == state)
                size += width * length;
            else
                size += entry->header->states[i].wwidth * entry->header->states[i].wlength;
        }

        size = (size + 3) & 0xFFFFFFFCL;         // Round to even 4 bytes
        uint8_t *newbuf = (uint8_t *)malloc(size);
        int32_t skipdist = sizeof(SImageryHeader) + (sizeof(SImageryStateHeader) * (entry->header->numstates - 1));
        uint8_t *ptr = newbuf + skipdist;

        SImageryHeader* head = (SImageryHeader*)newbuf;
        memcpy(newbuf, entry->header, sizeof(SImageryHeader));

        for (i = 0; i < entry->header->numstates; i++)
        {
            PSImageryStateHeader sh = &(head->states[i]);

            memcpy(sh, &entry->header->states[i], sizeof(SImageryStateHeader));

            if (i == state)
            {
                if (width && length)
                {
                    sh->walkmap.set(ptr);
                    memset(ptr, 0, width * length);
                    ptr += width * length;
                }
                else
                    sh->walkmap.set(nullptr);
            }
            else
            {
                int32_t w = entry->header->states[i].wwidth;
                int32_t l = entry->header->states[i].wlength;

                if (w && l)
                {
                    sh->walkmap.set(ptr);
                    memcpy(ptr, entry->header->states[i].walkmap.ptr(), w * l);
                    ptr += w * l;
                }
                else
                    sh->walkmap.set(nullptr);
            }
        }

//      VirtualUnlock(entry->header, entry->headersize);
        free(entry->header);

        entry->header = (SImageryHeader*)newbuf;
        entry->headersize = size;
//      VirtualLock(entry->header, entry->headersize);
    }

    entry->header->states[state].wwidth = width;
    entry->header->states[state].wlength = length;
    entry->header->states[state].wheight = height;

    entry->headerdirty = true;
}

void TObjectImagery::DrawInvItem(TObjectInstance* oi, int32_t x, int32_t y)
{
    int32_t state = oi->GetState();

    if (state >= NumStates())
        return;

    PTBitmap invitem = oi->InventoryImage();
    if (!invitem)
        return;

    Display.Put(x, y, invitem, DM_TRANSPARENT | DM_BACKGROUND);
}

// ******************* Progressive Load System ******************

#if 0 // TODO(port): Subsystem 4 — threading (Win32 events/mutex/CreateThread → worker pool)

static bool QuitThread;
static HANDLE LoadBodyEvent, LoadCompleteEvent, PauseLoaderMutex, LoaderThreadHandle;
static unsigned LoaderThreadId;

void TObjectImagery::BeginLoaderThread()
{
    LoadBodyEvent = CreateEvent(nullptr, false, false, nullptr);
    LoadCompleteEvent = CreateEvent(nullptr, false, false, nullptr);
    PauseLoaderMutex = CreateMutex(nullptr, false, nullptr);

    QuitThread = false;

    LoaderThreadHandle = (HANDLE)_beginthreadex(
        nullptr, 0,
        &TObjectImagery::LoaderThread, nullptr, true,
        &LoaderThreadId );
}

void TObjectImagery::EndLoaderThread()
{
    while (ReleaseMutex(PauseLoaderMutex));
    QuitThread = true;
    PulseEvent(LoadBodyEvent);

    WaitForSingleObject(LoaderThreadHandle, INFINITE);
}

unsigned _stdcall TObjectImagery::LoaderThread(void *)
{
    for (;;)
    {
        WaitForSingleObject(LoadBodyEvent, INFINITE);

        if (QuitThread)
            break;

        for (int32_t loop = 0; loop < EntryArray.NumItems(); loop++)
        {
            WaitForSingleObject(PauseLoaderMutex, INFINITE);

            if (!EntryArray.Used(loop))
                continue;

            RSImageryEntry ie = EntryArray[loop];

            BEGIN_CRITICAL();

            if (ie.status == QE_QUEUED)
            {
                ie.status = QE_LOADING;

                END_CRITICAL();

                char buf[120];
                strcpy(buf, imagerypath);
                strcat(buf, ie.filename);

                uint32_t ressize;
                SImageryBody* body = (SImageryBody*)LoadResource(buf, -1, &ressize);

                BEGIN_CRITICAL();

                if (!body)                              // Mark load as failed
                {
                    ie.status = QE_FAILED;

                    PulseEvent(LoadCompleteEvent);
                }
                if (ie.imagery != nullptr) // Did system delete imagery while we were loading body
                {
                    ie.ressize = ressize;           // No, then fill out data
                    ie.body = body;
                    ImageryMemUsage += ressize;
                    ie.status = QE_LOADED;

//                  VirtualLock(body, ressize);

                    PulseEvent(LoadCompleteEvent);
                }
                else
                {
                    delete body;                        // YES?.. delete body and continue
                    ie.status = QE_NONE;
                }
            }

            END_CRITICAL();

            while (ReleaseMutex(PauseLoaderMutex));
        }
    }

  // Kill the events
    while (ReleaseMutex(PauseLoaderMutex)); // Release if I own it just in case
    CloseHandle(LoadCompleteEvent);
    CloseHandle(LoadBodyEvent);
    CloseHandle(PauseLoaderMutex);

    _endthreadex( 0 );

    return 0;
}

void TObjectImagery::PauseLoader()
{
    WaitForSingleObject(PauseLoaderMutex, INFINITE);
}

void TObjectImagery::ResumeLoader()
{
    ReleaseMutex(PauseLoaderMutex);
}

SImageryBody* TObjectImagery::LoadBody(bool wait)
{
    if (entry->status == QE_LOADED)
        return entry->body;

    ReleaseMutex(PauseLoaderMutex); // Just in case somebody forgot to unpause the loader thread

    BEGIN_CRITICAL();
    if (entry->status == QE_NONE || entry->status == QE_QUEUED)
    {
        if (wait)
        {
            entry->status = QE_LOADING;

            END_CRITICAL();

            char buf[120];
            strcpy(buf, imagerypath);
            strcat(buf, entry->filename);

            entry->body = (SImageryBody*)LoadResource(buf, -1, (uint32_t *)&entry->ressize);

            BEGIN_CRITICAL();

            if (!entry->body)                               // Mark load as failed
            {
                entry->status = QE_FAILED;
            }
            else
            {
                ImageryMemUsage += entry->ressize;

                entry->status = QE_LOADED;
            }
        }
        else
        {
            entry->status = QE_QUEUED;
            QuitThread = false;
            PulseEvent(LoadBodyEvent);
        }
    }
    END_CRITICAL();

    if (entry->status == QE_LOADING)
    {
        if (wait)
        {
            while (entry->status == QE_LOADING)
            {
                WaitForSingleObject(LoadCompleteEvent, INFINITE);
            }
        }
    }

    if (entry->status == QE_FAILED)
    {
        FatalError("Imagery body load failed!");
    }

    return entry->body;
}

void TObjectImagery::FreeBody()
{
    BEGIN_CRITICAL();

    if (entry->status == QE_LOADING)
    {
        END_CRITICAL();

        while (entry->status == QE_LOADING)
        {
            WaitForSingleObject(LoadCompleteEvent, INFINITE);
        }

        BEGIN_CRITICAL();
    }

    if (entry->status == QE_QUEUED || entry->status == QE_FAILED || entry->status == QE_NONE)
    {
        entry->status = QE_NONE;
        entry->body = nullptr;

        END_CRITICAL();
        return;
    }

    if (entry->status == QE_LOADED)
    {
        entry->status = QE_NONE;
//      VirtualUnlock(entry->body, entry->ressize);
        free(entry->body);
        entry->body = nullptr;

        END_CRITICAL();
        return;
    }
}

#else

void TObjectImagery::BeginLoaderThread() {}
void TObjectImagery::EndLoaderThread() {}
uint_fast32_t TObjectImagery::LoaderThread(void *) { return 0; }
void TObjectImagery::PauseLoader() {}
void TObjectImagery::ResumeLoader() {}

SImageryBody* TObjectImagery::LoadBody(bool wait)
{
    if (entry->status == QE_LOADED)
        return entry->body;

    char buf[120];
    strcpy(buf, imagerypath);
    strcat(buf, entry->filename);

    entry->body = (SImageryBody*)LoadResource(buf, -1, (uint32_t *)&entry->ressize);

    if (!entry->body)
    {
        entry->status = QE_FAILED;
        FatalError("Imagery body load failed!");
    }
    else
    {
        ImageryMemUsage += entry->ressize;
        entry->status = QE_LOADED;
    }

    return entry->body;
}

void TObjectImagery::FreeBody()
{
    if (entry->status == QE_LOADED)
    {
        free(entry->body);
        entry->body = nullptr;
    }
    entry->status = QE_NONE;
}

#endif

// ******************* END OF Progressive Load System ******************

// *******************************
// * Imagery Quickload Functions *
// *******************************

struct SQuickLoadHeader
{
    uint32_t id;
    uint32_t version;
    uint32_t numheaders;
};

#define QUICKLOADFILEID   (('H') | ('D' << 8) | ('R' << 16) | ('S' << 24))
#define QUICKLOADFILEVER  (1)

bool TObjectImagery::QuickLoadHeaders(time_t iflater)
{
    char filename[FILENAMELEN];
    struct stat s;

  // We can only do a quickload if there's nothing in there yet!!
    if (EntryArray.NumItems() > 0)
        return false;

    strcpy(filename, ResourcePath);
    strcat(filename, "IMAGERY.DAT");

  // Is file later than the iflater time?
    if (stat(filename, &s) != 0 || s.st_mtime <= iflater)
        return false;   // No: don't use file, it's out of date

    FILE *f = fopen(filename, "rb");
    if (!f)
        return false;

    SQuickLoadHeader qh;

    if (fread(&qh, sizeof(SQuickLoadHeader), 1, f) < 1)
    {
        fclose(f);
        return false;
    }

    if (qh.id != QUICKLOADFILEID || qh.version != QUICKLOADFILEVER)
    {
        fclose(f);
        return false;
    }

    for (int32_t c = 0; c < (int32_t)qh.numheaders; c++)
    {
        char filename[MAXIMFNAMELEN];

        if (fread(&filename, MAXIMFNAMELEN, 1, f) < 1)
        {
            fclose(f);
            return false;
        }

        uint32_t headersize;
        if (fread(&headersize, 4, 1, f) < 1)
        {
            fclose(f);
            return false;
        }

        SImageryHeader* header = (SImageryHeader*)malloc(headersize);
        if (fread(header, headersize, 1, f) < 1)
        {
            fclose(f);
            return false;
        }

        RegisterImagery(filename, header, headersize);
    }

    fclose(f);

    return true;
}

bool TObjectImagery::QuickSaveHeaders()
{
    char filename[FILENAMELEN];
    int32_t c;

    strcpy(filename, ResourcePath);
    strcat(filename, "IMAGERY.DAT");

    FILE *f = fopen(filename, "wb");
    if (!f)
        return false;

    SQuickLoadHeader qh;
    memset(&qh, 0, sizeof(SQuickLoadHeader));

    qh.id = QUICKLOADFILEID;
    qh.version = QUICKLOADFILEVER;
    qh.numheaders = 0;

    for (c = 0; c < EntryArray.NumItems(); c++)
    {
        if (EntryArray.Used(c))
            qh.numheaders++;
    }

    if (fwrite(&qh, sizeof(SQuickLoadHeader), 1, f) < 1)
    {
        fclose(f);
        return false;
    }

    for (c = 0; c < EntryArray.NumItems(); c++)
    {
        if (!EntryArray.Used(c))
            continue;

        RSImageryEntry ie = EntryArray[c];

        if (fwrite(&ie.filename, MAXIMFNAMELEN, 1, f) < 1)
        {
            fclose(f);
            return false;
        }

        if (fwrite(&ie.headersize, 4, 1, f) < 1)
        {
            fclose(f);
            return false;
        }

        if (fwrite(ie.header, ie.headersize, 1, f) < 1)
        {
            fclose(f);
            return false;
        }
    }

    fclose(f);

    return true;
}
