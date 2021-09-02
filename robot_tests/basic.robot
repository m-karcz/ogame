*** Settings ***
Library		Collections
#Library		RequestsLibrary
Library		Process
#Library     OgameLib
Library		OgameLib2	${testConfiguration}

*** Keywords ***
#Start App
#	No Operation
#	${servProcLoc} = 	Start Process	${serverFile}	--rnd 	stdout=noelo.txt 
#	Set Test Variable	${servProc}	${servProcLoc}
#	Sleep	1s
#
#Stop App
#	#No Operation
#	Terminate Process	handle=${servProc}	kill=True

#Get Logged
#	Register User
#	Login User

Start Router
	No Operation
	${servProcLoc} = 	Start Process 	${runRouter}
	Set Test Variable	${servProc}	${servProcLoc}
	Sleep	1s
	Open Site

Stop Router
	#No Operation
	Clear Database
	Close Site
	Terminate Process	handle=${servProc}	kill=True

*** Settings ***
Test Setup	Start Router
Test Teardown	Stop Router

*** Variables ***


*** Test Cases ***

#RegisterAndLogin
#	#Sleep	1s
#	Get Logged
#	Forward Time	20
#	Get Buildings
#	Queue Building	Metal Mine
#	Forward Time	20000000
#	Get Buildings
#	Check Building Level	Metal Mine	1

TestNewFramework
	Log	elo ${runRouter}
	Sleep	1s
	Register And Login On
	Assert Logged In

TestBuilding
	Get Ready
	Start Building	Metal Mine
	Assert That Any Building Is Ongoing
	Forward Time	240
	Assert That No Building Is Ongoing
	Forward Time	120
