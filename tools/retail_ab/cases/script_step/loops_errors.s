// WHILE loops, a jump loop the iteration guard ends, block errors.
OBJECT "Loops"
BEGIN
	DIALOG
	BEGIN
		SAY A
		WHILE COUNT < 3
		BEGIN
			SAY B
			SET COUNT = COUNT + 1
		END
		SAY C
		WHILE COUNT < 5
			SAY D
		SAY E
	END
	USE
	BEGIN
		:again
		SAY F
		IF COUNT < 9
		BEGIN
			JUMP again
		END
		SAY G
	END
	ALWAYS
	BEGIN
		:forever
		JUMP forever
	END
END

OBJECT "BlockErrors"
BEGIN
	DIALOG
	BEGIN
		SAY A
		SAY B
	END
	USE
	BEGIN
		SAY C
		12 SAY NUMBER
		"QUOTED".SAY D
		SAY E
		JUMP nosuchlabel
		SAY F
	END
END

OBJECT "Deep"
BEGIN
	DIALOG
	BEGIN
		IF S = 1
		BEGIN
			IF S = 2
			BEGIN
				IF S = 3
				BEGIN
					IF S = 4
					BEGIN
						IF S = 5
						BEGIN
							IF S = 6
							BEGIN
								IF S = 7
								BEGIN
									IF S = 8
									BEGIN
										:depth9
										SAY NINE
										IF S = 9
										BEGIN
											SAY TEN
										END
										ELSE
										BEGIN
											SAY NOTTEN
										END
									END
								END
							END
						END
					END
				END
			END
		END
		SAY OUT
	END
END
