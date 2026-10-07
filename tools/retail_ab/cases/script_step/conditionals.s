// IF / ELSE / ELSE IF with and without blocks, and labels inside them.
OBJECT "Conditionals"
BEGIN
	DIALOG
	BEGIN
		IF STATE = 1
		BEGIN
			SAY A1
			:inif
			SAY A2
		END
		ELSE
		BEGIN
			SAY B1
			:inelse
			SAY B2
		END
		IF STATE = 2
			SAY C1
		ELSE
			SAY C2
		SAY D
		IF STATE = 3
		BEGIN
			SAY E1
		END
		ELSE IF STATE = 4
		BEGIN
			SAY E2
		END
		ELSE
		BEGIN
			SAY E3
		END
		SAY F
	END
	USE
	BEGIN
		IF STATE = 5
		BEGIN
			IF STATE = 6
			BEGIN
				:deep
				SAY G1
			END
			ELSE
			BEGIN
				SAY G2
			END
			SAY G3
		END
		ELSE
		BEGIN
			SAY G4
		END
		SAY H
	END
END

// ELSE with no IF before it, and ELSE after a jump into a block.
OBJECT "OrphanElse"
BEGIN
	DIALOG
	BEGIN
		SAY A
		ELSE
		BEGIN
			SAY B
		END
		SAY C
		JUMP intoblock
		IF STATE = 7
		BEGIN
			SAY D
			:intoblock
			SAY E
		END
		ELSE
		BEGIN
			SAY F
		END
		SAY G
	END
END
