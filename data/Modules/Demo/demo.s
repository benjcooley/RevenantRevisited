

OBJECT "LV30EX"
BEGIN
	CUBE 6440,1380,0 6465,1430,144
	BEGIN
		CONTROL OFF
		LOCKE.STOP
		FADESCREENOUT
		WAIT SCREENFADE
		WAIT 48
		FADESCREENIN
		ENDGAME
		CONTROL ON
	END
END


OBJECT "CAVEXIT"
BEGIN
	CUBE 10530,10360,0 10575,10390,100
	BEGIN
		NOWAIT CONTROL OFF
		PLAYER.STOP
		FADESCREENOUT
		WAIT SCREENFADE
		FADESCREENIN
		PLAYER.POS 5341 18600 42 10
		CONTROL ON
	END
END

OBJECT "CAVENTER"
BEGIN
	CUBE player 5260,18500,0 5365,18520,100
	BEGIN
		NOWAIT CONTROL OFF
		player.STOP
		FADESCREENOUT
		WAIT SCREENFADE
 		FADESCREENIN
		player.POS 10560 10496 16 30
		CONTROL ON
	END
END


OBJECT "Howard1"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		player.STOP
		player.COMBAT OFF
		NOWAIT PIVOTOBJECT player
		player.PIVOTOBJECT HOWARD1
		SAY II23TM100
		SAY II23TM101
		CONTROL ON
	END
	ALWAYS
	BEGIN
		GOTORELATIVEPOSITION HOWARDWAY1 0 0
		WAIT 24
		PIVOTOBJECT HOWARDWAY2
		WAIT 24
		GOTORELATIVEPOSITION HOWARDWAY2 0 0
		WAIT 24
		PIVOTOBJECT HOWARDWAY1
		WAIT 24
	END
END


OBJECT "Paul1"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		player.STOP
		player.COMBAT OFF
		NOWAIT PIVOTOBJECT player
		player.PIVOTOBJECT PAUL1
		SAY II23TM200
		SAY II23TM201
		SAY II23TM202
		PIVOT 45
		CONTROL ON
	END
END

OBJECT "Karl1"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		player.STOP
		player.COMBAT OFF
		NOWAIT PIVOTOBJECT player
		player.PIVOTOBJECT KARL1
		SAY II23TM400
		SAY II23TM401
		SAY II23TM402
		SAY II23TM403
		PIVOT 0
		CONTROL ON
	END
END

OBJECT "Amy1"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		player.STOP
		player.COMBAT OFF
		NOWAIT PIVOTOBJECT player
		player.PIVOTOBJECT AMY1
		SAY II23TW500
		SAY II23TW501
		SAY II23TW502
		CONTROL ON
	END
END

OBJECT "Pamela1"
BEGIN
	DIALOG
	BEGIN
		CONTROL OFF
		player.STOP
		player.COMBAT OFF
		NOWAIT PIVOTOBJECT player
		player.PIVOTOBJECT PAMELA1
		SAY II23TW200
		SAY II23TW201
		SAY II23TW202
		CONTROL ON
	END
END

//	SHOPS


OBJECT "ELAHNI1"
BEGIN
	DIALOG
	BEGIN
		face 120
		control off
		player.stop
		player.COMBAT OFF
		nowait pivotobject player
		player.pivotobject elahni1
		SAY II22ELA00
		SAY II22ELA01

		:start1

		buysellinit
		buysellsalesperson elahni1
		buysellnogolddialog II22ELA02
		buysellpurchasedialog II22ELA03

		choice buy1 BSBUY2
		choice sell1 BSSELL2
		choice stop1 BSEXIT2
		wait response

		:buy1
	
		player.SAY II22LOC01
		player.SAY II22LOC02
		buysellshoptype buy misc
		BuySellAdd "Lesser Healing"
		BuySellAdd "Lesser Mana"
		BuySellAdd "Lesser Restoration"
		BuySellAdd "Greater Healing"
		BuySellAdd "Greater Mana"
		BuySellAdd "Greater Restoration"
		BuySellAdd "Enchanted Healing"
		BuySellAdd "Enchanted Mana"
		BuySellAdd "Enchanted Restoration"
		buysellscreen
		wait buysell
		jump start1

		:sell1
		buysellshoptype sell misc
		BuySellAddbuyitem "Lesser Healing"
		BuySellAddbuyitem "Lesser Mana"
		BuySellAddbuyitem "Lesser Restoration"
		BuySellAddbuyitem "Greater Healing"
		BuySellAddbuyitem "Greater Mana"
		BuySellAddbuyitem "Greater Restoration"
		BuySellAddbuyitem "Enchanted Healing"
		BuySellAddbuyitem "Enchanted Mana"
		BuySellAddbuyitem "Enchanted Restoration"
		buysellscreen
		wait buysell
		jump start1

		:stop1

		SAY II22ELA05
		SAY II22ELA06
		control on
	END
END


OBJECT "GINA1"
BEGIN
	DATA
	BEGIN
		number VALUE 0
	END
	DIALOG
	BEGIN
		Control off
		player.stop
		player.COMBAT OFF
		nowait pivotobject player
		player.pivotobject GINA1
		SETPROTOVARIABLE VALUE = player.STAT "LEVEL"  + 16
		IF MISTSTATE < 4
		BEGIN

		SAY II9GIN00
		SAY II9GIN01

		:start1

		buysellinit
		buysellsalesperson GINA1
		buysellnogolddialog II9GIN03
		buysellpurchasedialog II9GIN04

		choice buy1 BSBUY2
		choice sell1 BSSELL2
		choice stop1 BSEXIT2
		wait response

		:buy1
		player.SAY II9LOC00
		player.SAY II9LOC01
		buysellshoptype buy WEAPON
		BuySellAddCriteria "minstrength" 1 VALUE
		buysellscreen
		wait buysell
		jump start1

		:sell1
		buysellshoptype sell WEAPON
		buyselladdbuyitems
		buysellscreen
		wait buysell
		jump start1

		:stop1

		SAY II9GIN06
		SAY II9GIN07
		SAY II9GIN08

		END
		
		IF MISTSTATE > 3
		BEGIN
		IF MISTSTATE < 5
		BEGIN 

		SAY II20GIN00
		SAY II20GIN01

		:start2

		buysellinit
		buysellsalesperson GINA1
		buysellnogolddialog II20GIN02
		buysellpurchasedialog II20GIN03

		choice buy2 BSBUY2
		choice sell2 BSSELL2
		choice stop2 BSEXIT2
		wait response

		:buy2
		NOWAIT player.SAY II20LOC00
		buysellshoptype buy WEAPON
		BuySellAddCriteria "minstrength" 1 VALUE
		buysellscreen
		wait buysell
		jump start2

		:sell2
		buysellshoptype sell WEAPON
		buyselladdbuyitems
		buysellscreen
		wait buysell
		jump start2

		:stop2

		SAY II20GIN05
		SAY II20GIN07

		END
		END

		IF MISTSTATE = 6
		BEGIN
		
		SAY V4GIN00
		SAY V4GIN01

		:start3

		buysellinit
		buysellsalesperson GINA1
		buysellnogolddialog V4GIN02
		buysellpurchasedialog V4GIN03

		choice buy3 BSBUY2
		choice sell3 BSSELL2
		choice stop3 BSEXIT2
		wait response

		:buy3
		NOWAIT player.SAY V4LOC00
		buysellshoptype buy weapon
		BuySellAddCriteria "minstrength" 1 VALUE
		buysellscreen
		wait buysell
		jump start3

		:sell3
		buysellshoptype sell WEAPON
		buyselladdbuyitems
		buysellscreen
		wait buysell
		jump start3

		:stop3

		SAY V4GIN05
		SAY V4GIN06
		SAY V4GIN07

		END

		IF MISTSTATE = 7
		BEGIN
		
		SAY VIII5GIN00
		SAY VIII5GIN01
		
		:start4

		buysellinit
		buysellsalesperson GINA1
		buysellnogolddialog VIII5GIN02
		buysellpurchasedialog VIII5GIN03

		choice buy4 BSBUY2
		choice sell4 BSSELL2
		choice stop4 BSEXIT2
		wait response

		:buy4
		NOWAIT player.SAY VIII5LOC00
		buysellshoptype buy WEAPON
		BuySellAddCriteria "minstrength" 1 VALUE
		buysellscreen
		wait buysell
		jump start4

		:sell4
		buysellshoptype sell WEAPON
		buyselladdbuyitems
		buysellscreen
		wait buysell
		jump start4

		:stop4

		SAY VIII5GIN05
		SAY VIII5GIN06
		SAY VIII5GIN07

		END
		CONTROL ON
	END
	ALWAYS
	BEGIN
		GOTORELATIVEPOSITION GINAWAY1 0 0
		WAIT 24
		PIVOTOBJECT GINAWAY2
		WAIT 24
		GOTORELATIVEPOSITION GINAWAY2 0 0
		WAIT 24
		PIVOTOBJECT GINAWAY1
		WAIT 24
		GOTORELATIVEPOSITION GINAWAY1 0 0
		WAIT 24
		PIVOTOBJECT GINAWAY3
		WAIT 24
		GOTORELATIVEPOSITION GINAWAY3 0 0
		WAIT 24
		PIVOTOBJECT GINAWAY1
		WAIT 24
	END
END



OBJECT "CRONUS1"
BEGIN
	DATA
	BEGIN
		number VALUE 0
	END
	DIALOG
	BEGIN
		Control off
		player.stop
		player.COMBAT OFF
		nowait pivotobject player
		player.pivotobject cronus1
		SETPROTOVARIABLE VALUE = player.STAT "LEVEL"  + 16
		IF MISTSTATE < 4
		BEGIN

		SAY II8CRO00
		SAY II8CRO01

		:start1

		buysellinit
		buysellsalesperson cronus1
		buysellnogolddialog II8CRO02
		buysellpurchasedialog II8CRO03

		choice buy1 BSBUY2
		choice sell1 BSSELL2
		choice stop1 BSEXIT2
		wait response

		:buy1
		player.SAY II8LOC00
		player.SAY II8LOC01
		buysellshoptype buy armor
		BuySellAddCriteria "minstrength" 1 VALUE
		BuySellRemoveCriteria "EQSLOT" 1 1
		BuySellRemoveCriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start1

		:sell1
		buysellshoptype sell armor
		buyselladdbuyitems
		buysellremovebuycriteria "EQSLOT" 1 1
		buysellremovebuycriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start1

		:stop1

		SAY II8CRO06
		SAY II8CRO07

		END
		
		IF MISTSTATE > 3
		BEGIN
		IF MISTSTATE < 5
		BEGIN 

		SAY II19CRO00
		SAY II19CRO01

		:start2

		buysellinit
		buysellsalesperson cronus1
		buysellnogolddialog II19CRO02
		buysellpurchasedialog II19CRO03

		choice buy2 BSBUY2
		choice sell2 BSSELL2
		choice stop2 BSEXIT2
		wait response

		:buy2
		player.SAY II19LOC00
		NOWAIT player.SAY II19LOC01
		buysellshoptype buy armor
		BuySellAddCriteria "minstrength" 1 VALUE
		BuySellRemoveCriteria "EQSLOT" 1 1
		BuySellRemoveCriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start2

		:sell2
		buysellshoptype sell armor
		buyselladdbuyitems
		buysellremovebuycriteria "EQSLOT" 1 1
		buysellremovebuycriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start2

		:stop2

		SAY II19CRO06
		SAY II19CRO07

		END
		END

		IF MISTSTATE = 6
		BEGIN
		
		SAY V5CRO00
		SAY V5CRO01

		:start3

		buysellinit
		buysellsalesperson cronus1
		buysellnogolddialog V5CRO02
		buysellpurchasedialog V5CRO03

		choice buy3 BSBUY2
		choice sell3 BSSELL2
		choice stop3 BSEXIT2
		wait response

		:buy3
		player.SAY V5LOC00
		NOWAIT player.SAY V5LOC01
		buysellshoptype buy armor
		BuySellAddCriteria "minstrength" 1 VALUE
		BuySellRemoveCriteria "EQSLOT" 1 1
		BuySellRemoveCriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start3

		:sell3
		buysellshoptype sell armor
		buyselladdbuyitems
		buysellremovebuycriteria "EQSLOT" 1 1
		buysellremovebuycriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start3

		:stop3

		SAY V5CRO06
		SAY V5CRO07

		END

		IF MISTSTATE = 7
		BEGIN
		
		SAY VIII6CRO00
		player.SAY VIII6LOC00
		SAY VIII6CRO01
		SAY VIII6CRO02


		:start4

		buysellinit
		buysellsalesperson cronus1
		buysellnogolddialog VIII6CRO03
		buysellpurchasedialog VIII6CRO04

		choice buy4 BSBUY2
		choice sell4 BSSELL2
		choice stop4 BSEXIT2
		wait response

		:buy4
		player.SAY VIII6LOC01
		NOWAIT player.SAY VIII6LOC02
		buysellshoptype buy armor
		BuySellAddCriteria "minstrength" 1 VALUE
		BuySellRemoveCriteria "EQSLOT" 1 1
		BuySellRemoveCriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start4

		:sell4
		buysellshoptype sell armor
		buyselladdbuyitems
		buysellremovebuycriteria "EQSLOT" 1 1
		buysellremovebuycriteria "EQSLOT" 5 6
		buysellscreen
		wait buysell
		jump start4

		:stop4

		SAY VIII6CRO06
		SAY VIII6CRO07

		END
		CONTROL ON
	END
	ALWAYS
	BEGIN
		GOTORELATIVEPOSITION CRONUSWAY1 0 0
		WAIT 24
		WAIT 24
		WAIT 24
		WAIT 24
		PIVOTOBJECT CRONUSWAY2
		WAIT 12
		GOTORELATIVEPOSITION CRONUSWAY2 0 0
		WAIT 12
		PIVOT 190
		WAIT 24
		WAIT 24
		WAIT 24
		PIVOTOBJECT CRONUSWAY3
		WAIT 12
		GOTORELATIVEPOSITION CRONUSWAY3 0 0
		WAIT 12
		PIVOTOBJECT CRONUSWAY1
		WAIT 12
	END
END


OBJECT "Rubold1"
BEGIN
	DIALOG
	BEGIN
		COntrol off
		player.STOP	
		player.COMBAT OFF
		nowait player.pivotobject Rubold1
		pivotobject player
	
		say II4RUB00
	
			:Start

				Choice town II4LOC00
				Choice dancing II4LOC01
				Choice cult II4LOC02
				Choice look II4LOC03
				wait response
	
			:town
         
				player.say II4LOC00
				say II4RUB01
				say II4RUB02
				say II4RUB03
				jump Start

			:dancing
	
				player.say II4LOC01
				say II4RUB04
				say II4RUB05
				jump Start

			:cult
	
				player.say II4LOC02
				say II4RUB06
				say II4RUB07
				say II4RUB08
				jump Start

			:look
	
				player.say II4LOC03
				say II4RUB09
		
		CONTROL ON
	END
	ALWAYS
	BEGIN
		gotorelativeposition playerway 0 0
		pivot 120
		try wipebar
		gotorelativeposition kegway 0 0
		pivot 0
		try wipebar
		gotorelativeposition karlway 0 0
		pivot 120
		try wipebar
		gotorelativeposition lucasway 0 0
		pivot 190
		try wipebar
		gotorelativeposition keelanway 0 0
		pivot 120
		try say
	END
END


OBJECT "Kylie1"
BEGIN
	DIALOG
	BEGIN
		control off
		player.STOP	
		player.COMBAT OFF
		nowait player.pivotobject Kylie1
		pivotobject player
		
			If KYLIESTATE = 1
			BEGIN
				say II5KYL08
			END
			If KYLIESTATE = 0 
 			BEGIN
				player.say II5LOC00
				say II5KYL00
				player.say II5LOC01
				say II5KYL01
				player.say II5LOC02
				say II5KYL02
				say II5KYL03
	
				:Start1

				Choice years II5LOC03 
				Choice kidding II5LOC04 
				Choice embarrass II5LOC05
				Choice go II5LOC06
				wait response

				:years
				player.say II5LOC03
				say II5KYL04
				jump FINISH1

				:kidding
				player.say II5LOC04
				say II5KYL05
				player.say II5LOC07
				say II5KYL06
				jump FINISH1

				:embarrass
				player.say II5LOC05
				say II5KYL07
				player.say II5LOC08
				player.say II5LOC09
				jump Finish1

				:go
				player.say II5LOC06
				say II5KYL08
				jump Finish1
	
				:Finish1
				set Kyliestate = 1
			END
	
		control on
		PIVOT 50
	END	
END



