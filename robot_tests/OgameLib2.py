from robot.api.deco import keyword
from selenium import webdriver
from selenium.webdriver import FirefoxOptions

from webdriver_manager.firefox import GeckoDriverManager
import time
import json
import os
import sys

BASE_URL = "http://localhost:8888/"

def human2pascal(inputStr):
    copy = inputStr[0].lower() + inputStr[1:]
    return "".join(copy.split(" "))

class OgameLib2:
    ROBOT_LIBRARY_SCOPE = 'TEST'
    ROBOT_AUTO_KEYWORDS = False

    def __init__(self, testConf):
        opts = FirefoxOptions()
        opts.add_argument("--headless")
        self.driver = webdriver.Firefox(executable_path=GeckoDriverManager().install(), firefox_options=opts)
        print(testConf)
        with open(testConf) as testConfFp:
            self.testConf = json.load(testConfFp)

    def open_buildings(self):
        self.click_id("goToBuildingsButton")

    def click_id(self, name):
        self.driver.find_element_by_id(name).click()
        time.sleep(0.1)


    @keyword
    def open_site(self):
        self.driver.get(BASE_URL)
        time.sleep(3)

    @keyword
    def close_site(self):
        self.driver.close()
        self.driver.quit()

    @keyword
    def get_ready(self):
        #self.open_site()
        #self.clear_database()
        #time.sleep(0.1)
        self.register_and_login_on()
        self.assert_logged_in()


    @keyword
    def register_and_login_on(self, login="elo", passcode="320"):
        self.driver.find_element_by_name("login-input").send_keys(login)
        self.driver.find_element_by_name("passcode-input").send_keys(passcode)
        self.driver.find_element_by_xpath('//input[@value="Register"]').click()
        time.sleep(0.1)
        self.driver.find_element_by_xpath('//input[@value="Login"]').click()
        time.sleep(0.1)

    @keyword
    def assert_logged_in(self):
        if any(self.driver.find_elements_by_name("login-input")):
            raise RuntimeError("not logged in")

    @keyword
    def assert_that_any_building_is_ongoing(self):
        self.open_buildings()
        if not any(self.driver.find_elements_by_id("timeToFinishBuilding")):
            raise RuntimeError("no ongoing building")

    @keyword
    def assert_that_no_building_is_ongoing(self):
        self.open_buildings()
        if any(self.driver.find_elements_by_id("timeToFinishBuilding")):
            raise RuntimeError("ongoing building")

    @keyword
    def forward_time(self, amount):
        rnd_caller_path = self.testConf["rnd_caller_path"]
        if rnd_caller_path not in sys.path:
            sys.path.append(os.path.abspath(rnd_caller_path))
        from rnd_caller import forward_time
        forward_time(int(amount), self.driver)

    @keyword
    def clear_database(self):
        rnd_caller_path = self.testConf["rnd_caller_path"]
        if rnd_caller_path not in sys.path:
            sys.path.append(os.path.abspath(rnd_caller_path))
        from rnd_caller import clear_database
        clear_database(self.driver)

    @keyword
    def start_building(self, building):
        self.open_buildings()
        self.click_id("build_" + human2pascal(building))

         
