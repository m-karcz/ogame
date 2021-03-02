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


    @keyword
    def open_site(self):
        self.driver.get(BASE_URL)

    @keyword
    def get_ready(self):
        self.open_site()
        self.register_and_login_on()
        self.assert_logged_in()


    @keyword
    def register_and_login_on(self, login="elo", passcode="320"):
        self.driver.find_element_by_name("login-input").send_keys(login)
        self.driver.find_element_by_name("passcode-input").send_keys(passcode)
        self.driver.find_element_by_xpath('//input[@value="Register"]').click()
        time.sleep(0.1)
        self.driver.find_element_by_xpath('//input[@value="Login"]').click()

    @keyword
    def assert_logged_in(self):
        if any(self.driver.find_elements_by_name("login-input")):
            raise RuntimeError("not logged in")

    @keyword
    def assert_that_any_building_is_ongoing(self):
        if not any(self.driver.find_elements_by_id("timeToFinishBuilding")):
            raise RuntimeError("no ongoing building")

    @keyword
    def assert_that_no_building_is_ongoing(self):
        self.driver.find_element_by_id("goToBuildingsButton").click()
        time.sleep(0.1)
        if any(self.driver.find_elements_by_id("timeToFinishBuilding")):
            raise RuntimeError("ongoing building")

    @keyword
    def forward_time(self, amount):
        time_forwarder_path = self.testConf["time_forwarder_path"]
        if time_forwarder_path not in sys.path:
            sys.path.append(os.path.abspath(time_forwarder_path))
        from time_forwarder import forward_time
        forward_time(int(amount), self.driver)

    @keyword
    def start_building(self, building):
        self.driver.find_element_by_id("goToBuildingsButton").click()
        time.sleep(0.1)
        self.driver.find_element_by_id("build_" + human2pascal(building)).click()
        time.sleep(0.1)

         
