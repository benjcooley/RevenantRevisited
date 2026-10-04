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

void PulseStartupExec();
