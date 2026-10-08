OBJECT "Talker"
BEGIN
	DIALOG
	BEGIN
		CHOICE one "A"
		WAIT RESPONSE
		:one
		CONTROL OFF
		IF MISTSTATE = 1
		BEGIN
			: spaced
			CONTROL OFF
			IF MISTSTATE = 2
			BEGIN
				:Deep
				CONTROL ON
			END
		END
		:ONE
		CONTROL ON
	END
	USE
	BEGIN
		:usetop
		CONTROL ON
	END
END
