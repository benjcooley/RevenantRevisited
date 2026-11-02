// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   consoleexec.h - startup console-command queue (--exec)              *
// *************************************************************************
//
// --exec="cmd; cmd; sleep 24; cmd" queues console commands that run through
// the regular CommandInterpreter once the PlayScreen has a player, one entry
// per game tick. The context defaults to the player, so "say I1LOC00" makes
// Locke talk and "sardokr.pivotobject player" targets another object.
// "sleep N" pauses the queue for N game ticks. Used to drive script commands
// headlessly while they are being ported and verified.
#pragma once

#include <cstdint>
#include <string>

void PulseStartupExec();

// Whether --exec still has commands to run (or is sleeping before one):
// a harness that runs on the state the commands set up waits for this.
[[nodiscard]] bool StartupExecPending();

// One console command, now, through the CommandInterpreter with the player
// as its context (the playtest control channel's `exec`); its result code.
int32_t RunConsoleCommand(const std::string& cmd);
