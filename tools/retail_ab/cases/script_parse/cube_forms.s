// CUBE: named, unnamed (the prototype's name), NULL, a named region, reversed corners
OBJECT "CubeBox"
BEGIN
	CUBE player 10,20,30 40,50,60
	BEGIN
		CONTROL OFF
	END
	CUBE 400,500,600 100,200,300
	BEGIN
		CONTROL ON
	END
	CUBE NULL 1,2,3 4,5,6
	BEGIN
		CONTROL ON
	END
	CUBE player GateRegion
	BEGIN
		CONTROL ON
	END
	CUBE "Quoted Name" 7,8,9 1,2,3
	BEGIN
		CONTROL ON
	END
	CUBE ANameThatIsLongerThanNineteenChars 1,1,1 2,2,2
	BEGIN
		CONTROL ON
	END
	CUBE player 1,2 3,4
	BEGIN
		CONTROL ON
	END
END
