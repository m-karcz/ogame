import urllib3
import json
from robot.api.deco import keyword

testAddress = "http://127.0.0.1:8080/"

def getCredentials(login="eloelo", passcode="pass320"):
    return {
        'credentials': {
            'login': login,
            'passcode': passcode
        }
    }


def makeTypedRequest(reqName, reqData):
    return {
        'type': reqName,
        'data': reqData
    }

def makeTimeForwardRequest(durationInMs):
    data = {
            'duration': durationInMs
        }
    return makeTypedRequest("TimeForwardRequest", data);

def makeStorageRequest(planet):
    data = {

    }

def makeBuildingsListRequest():
    return makeTypedRequest("BuildingsListRequest", {})

def makeBuildRequest(buildingName):
    return makeTypedRequest("BuildRequest", {"buildingName": buildingName})

def getRegisterReq(credentials=getCredentials()):
    return makeTypedRequest("RegisterRequest", credentials)


def getLoginReq(credentials=getCredentials()):
    return makeTypedRequest("LoginRequest", credentials)

def assertRegisteredSucessfully(response):
    if not response["status"] == "ok":
        raise RuntimeError("Unsuccessful registration")

def assertSucessfulLogin(response):
    if not response["playerId"]:
        raise RuntimeError("Unsuccessful login")

def getTypedResp(resp, type):
    for r in resp:
        if(r["type"] == type):
            return r["data"]

def camelCaseName(humanName):
    words = humanName.split(" ");
    words[0] = words[0].lower()
    return "".join(words)

class PlanetData:
    def __init__(self):
        self.buildings = dict()

class OgameLib:
    ROBOT_LIBRARY_SCOPE = 'TEST'
    ROBOT_AUTO_KEYWORDS = False

    def __init__(self):
        self.http = urllib3.PoolManager()
        self.playerId = None
        self.planets = []
        self.planetsData = []
        self.chosenPlanet = 0

    def sendRequest(self, body, suffix="test"):
        encoded_json = json.dumps(body).encode('utf-8')
        print(encoded_json)
        r = self.http.request('POST', testAddress + suffix, body=encoded_json, headers={'Content-type': 'application/json'})
        response = json.loads(r.data.decode('utf-8'))
        print(response)
        return response

    @keyword
    def forward_time(self, duration):
        self.sendRequest(makeTimeForwardRequest(int(duration)), "rnd")

    @keyword
    def register_user(self):
        assertRegisteredSucessfully(self.sendRequest(getRegisterReq(), "general"))

    @keyword
    def login_user(self):
        resp = self.sendRequest(getLoginReq(), "general")
        assertSucessfulLogin(resp)
        self.playerId = resp["playerId"]
        self.planets = resp["planets"]
        for i, planet in enumerate(self.planets):
            self.planetsData.append(PlanetData())

    @keyword
    def get_buildings(self):
        resp = self.sendOnPlanetRequest(makeBuildingsListRequest())
        buildingList = getTypedResp(resp, "BuildingsListResponse")["buildings"]
        print(buildingList)
        print(buildingList)
        self.planetsData[self.chosenPlanet].buildings = dict(buildingList)

    @keyword
    def queue_building(self, humanBuildingName):
        buildingName = camelCaseName(humanBuildingName)
        resp = self.sendOnPlanetRequest(makeBuildRequest(buildingName))

    def getPlanet(self):
        return self.planets[self.chosenPlanet];

    def sendOnPlanetRequest(self, typedReq):
        req = {
            "playerId" : self.playerId,
            "planet" : self.getPlanet(),
            "requests" : [typedReq]
        }
        return self.sendRequest(req, "on_planet")

    @keyword
    def checkBuildingLevel(self, humanBuildingName, level):
        buildingName = camelCaseName(humanBuildingName)
        if (self.planetsData[self.chosenPlanet].buildings[str(buildingName)] != int(level)):
            raise RuntimeError("Wrong building level")

    @keyword
    def show_storage(self):
        planet = self.planets[0]
