*** Settings ***
Library		Collections
#Library		RequestsLibrary
Library		Process
Library     OgameLib

*** Keywords ***
Start App
	No Operation
	${servProcLoc} = 	Start Process 	${serverFile}	--rnd 	stdout=noelo.txt 
	Set Test Variable	${servProc}	${servProcLoc}
	Sleep	1s

Stop App
	#No Operation
	Terminate Process	handle=${servProc}	kill=True

Get Logged
	Register User
	Login User


*** Settings ***
Test Setup	Start App
Test Teardown	Stop App

*** Variables ***


*** Test Cases ***

RegisterAndLogin
	#Sleep	1s
	Get Logged
	Forward Time	20
	Get Buildings
	Queue Building	Metal Mine
	Forward Time	20000000
	Get Buildings
	Check Building Level	Metal Mine	1


