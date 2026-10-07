// A label directly in a trigger block that isn't the object's last, as in
// keep.s DalyK and forest.s Jong1: after its END comes the next trigger.
OBJECT "FirstOfThree"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		CHOICE pick "A"
		WAIT RESPONSE
		:pick
		CONTROL ON
		JUMP done
		CONTROL OFF
		:done
		SETCDVOLUME FULL
	END
	USE
	BEGIN
		:usetop
		CONTROL ON
	END
	ALWAYS
	BEGIN
		WAIT 24
		IF LOOKSTATE = 3
		BEGIN
			PLAYER.STAT HEALTH = 10
		END
	END
END

// The label's trigger is followed by a trigger whose header has a parameter.
OBJECT "ProximityAfter"
BEGIN
	DIALOG
	BEGIN
		:only
		SAY HELLO
	END
	PROXIMITY 120 PLAYER
	BEGIN
		SAY NEAR
	END
	TRIGGER Wake
	BEGIN
		SAY AWAKE
	END
END
