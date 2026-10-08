// A block whose IF never closes: the trigger's END closes the IF, the
// object's END closes the trigger, and the file ends inside the object.
OBJECT "Unbalanced"
BEGIN
	DIALOG
	BEGIN
		IF STATE = 1
		BEGIN
			SAY A
			:inside
			SAY B
		SAY C
	END
	USE
	BEGIN
		SAY D
	END
END
